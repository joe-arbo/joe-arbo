from time import sleep
import pigpio
# import serial
# from arm import *

# ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
# ser.reset_input_buffer()

# define digital IO pins
STEP1 = 12 # PWM signal

# direction pins
DIR1 = 6
DIR2 = 17
DIR3 = 16
DIR4 = 26

# Define directions
CW = 1
CCW = 0

# Driver Enable Pins (active low)
EN1 = 27
EN2 = 22
EN3 = 23
EN4 = 24

# Set up microstepping
MODE = (20, 5, 21) # Microstep resolution GPIO pins
RESOLUTION = {'Full': (0, 0, 0),
              'Half': (1, 0, 0),
              '1/4': (0, 1, 0),
              '1/8': (1, 1, 0),
              '1/16': (1, 0, 1),
              '1/32': (1, 0, 1)}

# Conect the pigpiod daemon
pi = pigpio.pi()


# Disable all motor drivers
pi.write(EN1, 1)
pi.write(EN2, 1)
pi.write(EN3, 1)
pi.write(EN4, 1)
sleep(1)

# set microstep value
for i in range(3):
    pi.write(MODE[i], RESOLUTION['1/32'][i])

# Enable motor drivers one at a time
pi.write(EN1, 0)
sleep(1)
pi.write(EN2, 0)
sleep(1)
pi.write(EN3, 0)
sleep(1)
pi.write(EN4, 0)


def forwards():
    frequency = 4500
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

    pi.write(DIR1, 1)
    pi.write(DIR2, 1)
    pi.write(DIR3, 0)
    pi.write(DIR4, 0)

def stopMoving():
    frequency = 0
    duty_cycle = 0
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

def forwardsSmall():
    forwards()
    sleep(0.25)
    stopMoving()
    
def forwardsSmaller():
    frequency = 1000
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

    pi.write(DIR1, 1)
    pi.write(DIR2, 1)
    pi.write(DIR3, 0)
    pi.write(DIR4, 0)
    sleep(0.15)
    stopMoving()

def clockwise():
    frequency = 1000
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

    pi.write(DIR1, 0)
    pi.write(DIR2, 0)
    pi.write(DIR3, 0)
    pi.write(DIR4, 0)

def counterClockwise():
    frequency = 1000
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

    pi.write(DIR1, 1)
    pi.write(DIR2, 1)
    pi.write(DIR3, 1)
    pi.write(DIR4, 1)

def backwards():
    frequency = 4500
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

    pi.write(DIR1, 0)
    pi.write(DIR2, 0)
    pi.write(DIR3, 1)
    pi.write(DIR4, 1)

def backwardsSmall():
    backwards()
    sleep(0.25)
    stopMoving()
    
def backwardsSmaller():
    frequency = 2000
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)

    pi.write(DIR1, 0)
    pi.write(DIR2, 0)
    pi.write(DIR3, 1)
    pi.write(DIR4, 1)
    sleep(0.15)
    stopMoving()

# turns off motors
def turnOffMotors():
    frequency = 500 # 0 (off) to 1-125M
    duty_cycle = 0 # 0 (off) to 1M (fully on)
    pi.hardware_PWM(STEP1, frequency, duty_cycle)
    # Disable all motor drivers
    pi.write(EN1, 1)
    pi.write(EN2, 1)
    pi.write(EN3, 1)
    pi.write(EN4, 1)
    sleep(1)
    pi.stop()
    
def left():
    frequency = 2000
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)
    pi.write(DIR1, 0)
    pi.write(DIR2, 1)
    pi.write(DIR3, 1)
    pi.write(DIR4, 0)

def right():
    frequency = 2000
    duty_cycle = 500000
    pi.hardware_PWM(STEP1, frequency, duty_cycle)
    pi.write(DIR1, 1)
    pi.write(DIR2, 0)
    pi.write(DIR3, 0)
    pi.write(DIR4, 1)
