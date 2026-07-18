#include <LiquidCrystal.h>
#include <millisDelay.h>

// Hardware configuration
constexpr uint8_t PISTON_PIN   = 2;    // drives the crusher piston relay/solenoid
constexpr uint8_t LIGHT_SENSOR = A4;   // photoresistor watching the drop chute
constexpr uint8_t ENABLE_PIN   = 9;    // master on/off switch: HIGH=on (light+relay live), LOW=off

// Timing (milliseconds)
constexpr unsigned long PRE_CRUSH_DELAY_MS  = 1000;  // let the can settle into crush position
constexpr unsigned long CRUSH_MS            = 1500;  // piston-extended dwell (mechanical hold)
constexpr unsigned long RETRACT_TIMEOUT_MS  = 3000;  // jam guard: max wait for the light to recover
constexpr unsigned long BASELINE_SAMPLE_MS  = 250;   // how often the idle baseline is resampled
constexpr unsigned long LIGHT_AVG_MS        = 20;    // average each read over ~1 mains cycle

// Detection tuning (all relative to the live baseline, so ambient drift is OK)
constexpr float CAN_DROP_RATIO = 0.20;  // trip at >=20% below baseline
constexpr float CLEAR_RATIO    = 0.15;  // "clear" once recovered to within 15% of baseline
constexpr float BASELINE_ALPHA = 0.10;  // EMA weight per sample (~2-3s time constant @250ms)

// Light-driven state machine that reads the light sensor and drives the piston.
// Detection floats on a rolling baseline, so it tracks changing room light.
class CanCrusher {
public:
  CanCrusher() : _lcd(12, 11, 4, 5, 6, 7) {}

  /// Initialize hardware and paint the initial display. Call from setup().
  void begin() {
    pinMode(PISTON_PIN, OUTPUT);
    pinMode(ENABLE_PIN, INPUT);   // external pull-down; switch pulls HIGH when on
    _pistonOff();
    _lcd.begin(16, 2);
    _lcd.setCursor(0, 0);
    _lcd.print("Cans Crushed: ");
    _showCount();
  }

  /// Advance the state machine one tick. Call every loop().
  void update() {
    // Master on/off switch. When off: disable and hold in Off (no sensing).
    if (digitalRead(ENABLE_PIN) != HIGH) {
      if (_state != State::Off) {
        _pistonOff();
        _setState(State::Off);
      }
      return;
    }

    _light = _readLight();   // averaged over a mains cycle to reject AC flicker

    switch (_state) {
      case State::Off:
        // Just switched on: snap the baseline to the current lit level, then
        // start watching. The idle EMA refines it before the first can arrives.
        _baseline = _light;
        _startWaiting();
        break;

      case State::WaitingForCan:
        // A drop below trip is a can; otherwise track the idle baseline.
        if (_light < _tripLevel()) {
          _enter(State::PreCrushDelay, PRE_CRUSH_DELAY_MS);
        } else if (_light > _clearLevel() && _baselineTimer.justFinished()) {
          _updateBaseline();
          _baselineTimer.start(BASELINE_SAMPLE_MS);
          if (++_hbCount % 4 == 0) {  // ~1s idle heartbeat
            _trace(F("idle "));
          }
        }
        break;

      case State::PreCrushDelay:
        // Once detected we commit: ignore the sensor for the settle, then crush.
        // A false trip just costs a harmless empty crush (switch-off still aborts).
        if (_timer.justFinished()) {
          _pistonOn();
          _enter(State::Crushing, CRUSH_MS);
        }
        break;

      case State::Crushing:
        // Fixed dwell; sensor ignored -- the head passing the sensor mid-stroke
        // makes the light rise again, so there is no clean optical "done".
        if (_timer.justFinished()) {
          _pistonOff();
          ++_cansCrushed;
          _showCount();
          _enter(State::RetractPassing, RETRACT_TIMEOUT_MS);
        }
        break;

      case State::RetractPassing:
        // Head sits below the sensor, so it reads clear now. Wait to see it
        // swing back UP past the sensor (a dip) before trusting a "clear".
        if (_light < _tripLevel()) {
          _setState(State::RetractClearing);
        } else if (_timer.justFinished()) {
          Serial.println(F("  ** JAM: head never re-crossed sensor, re-seeding **"));
          _updateBaseline();   // jam guard: adopt current level as new idle
          _startWaiting();
        }
        break;

      case State::RetractClearing:
        // Head is crossing the sensor now; wait for it to pass and the light to
        // recover before resuming. Jam guard keeps a stuck head from hanging us.
        if (_light > _clearLevel()) {
          _timer.stop();
          _startWaiting();
        } else if (_timer.justFinished()) {
          Serial.println(F("  ** JAM: light never recovered, re-seeding **"));
          _updateBaseline();
          _startWaiting();
        }
        break;
    }
  }

private:
  enum class State {
    Off,              // machine switch off: light + relay disabled, baseline frozen
    WaitingForCan,
    PreCrushDelay,
    Crushing,
    RetractPassing,   // waiting to see the crush head re-cross the sensor
    RetractClearing,  // waiting for the light to recover after it passes
  };

  /// Transition to a timed state and (re)arm the shared timer.
  void _enter(State next, unsigned long duration_ms) {
    _setState(next);
    _timer.start(duration_ms);
  }

  /// Return to idle watching and restart the baseline sampling cadence.
  void _startWaiting() {
    _setState(State::WaitingForCan);
    _baselineTimer.start(BASELINE_SAMPLE_MS);
  }

  /// Print the "[<ms>] " timestamp prefix shared by every trace line.
  void _stamp(unsigned long ms) { Serial.print('['); Serial.print(ms); Serial.print(F("ms] ")); }

  /// Change state, logging the transition with full context.
  void _setState(State next) {
    const unsigned long now = millis();
    _stamp(now);
    Serial.print(_stateName(_state)); Serial.print(F(" -> ")); Serial.print(_stateName(next));
    Serial.print(F(" (")); Serial.print(now - _stateSince); Serial.print(F("ms) "));
    _traceValues();
    _stateSince = now;
    _state = next;
  }

  /// Print one context line (tag + current light/baseline/thresholds/count).
  void _trace(const __FlashStringHelper *tag) {
    _stamp(millis());
    Serial.print(tag);
    _traceValues();
  }

  /// Shared tail: light, baseline, trip, clear, crushed -- one line.
  void _traceValues() {
    Serial.print(F("light=")); Serial.print(_light);
    Serial.print(F(" base=")); Serial.print(static_cast<int>(_baseline));
    Serial.print(F(" trip=")); Serial.print(static_cast<int>(_tripLevel()));
    Serial.print(F(" clear=")); Serial.print(static_cast<int>(_clearLevel()));
    Serial.print(F(" crushed=")); Serial.println(_cansCrushed);
  }

  /// Human-readable state name for the trace.
  static const char *_stateName(State s) {
    switch (s) {
      case State::Off:             return "Off";
      case State::WaitingForCan:   return "WaitingForCan";
      case State::PreCrushDelay:   return "PreCrushDelay";
      case State::Crushing:        return "Crushing";
      case State::RetractPassing:  return "RetractPassing";
      case State::RetractClearing: return "RetractClearing";
    }
    return "?";
  }

  /// Average over ~one mains cycle (rejects AC flicker). Samples are spaced ~1ms
  /// so the high-impedance LDR can recharge the ADC S/H -- back-to-back reads stick.
  int _readLight() {
    analogRead(LIGHT_SENSOR);          // throwaway: prime the sample-and-hold for this pin
    long sum = 0;
    int n = 0;
    const unsigned long start = millis();
    do {
      delay(1);                        // let the LDR settle before each real read
      sum += analogRead(LIGHT_SENSOR);
      ++n;
    } while (millis() - start < LIGHT_AVG_MS);
    return static_cast<int>(sum / n);
  }

  /// Fold one sample into the exponential moving-average baseline.
  void _updateBaseline() {
    _baseline += (_light - _baseline) * BASELINE_ALPHA;
  }

  /// Reading below this (fraction of baseline) counts as a can shadowing the sensor.
  float _tripLevel()  const { return _baseline * (1.0f - CAN_DROP_RATIO); }

  /// Reading above this (closer to baseline) counts as the sensor being clear.
  float _clearLevel() const { return _baseline * (1.0f - CLEAR_RATIO); }

  void _pistonOn()  { digitalWrite(PISTON_PIN, HIGH); }
  void _pistonOff() { digitalWrite(PISTON_PIN, LOW); }

  /// Repaint just the count on the second LCD row.
  void _showCount() {
    _lcd.setCursor(0, 1);
    _lcd.print(_cansCrushed);
  }

  State _state = State::Off;
  millisDelay _timer;          // active timed state (settle / crush / retract guard)
  millisDelay _baselineTimer;  // paces idle baseline resampling
  float _baseline    = 0.0f;   // rolling idle light level
  int _cansCrushed = 0;
  int _light = 0;              // latest sensor reading (shared by logic + trace)
  unsigned long _stateSince = 0;  // millis() at last transition (elapsed timing)
  unsigned int _hbCount = 0;   // idle-heartbeat throttle counter
  LiquidCrystal _lcd;
};

CanCrusher crusher;

void setup() {
  Serial.begin(9600);
  crusher.begin();
}

void loop() {
  crusher.update();
}
