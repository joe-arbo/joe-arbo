from tracemalloc import stop
import integration
import steppermotortest
from time import sleep
import arm
import pigpio
import sys
# import oled
# import os
# import time
# git access token: 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

pi = pigpio.pi()

START_GPIO = 7
STOP_GPIO = 8

pi.set_mode(START_GPIO, pigpio.INPUT)
pi.set_pull_up_down(START_GPIO, pigpio.PUD_UP)


def stop_handler():
   steppermotortest.turnOffMotors()
   integration.stopCamera()
   arm.servosOff()
   print("\nCtrl-C pressed. Stopping PIGPIO and exiting...")
   print('motors off')
   sys.exit()


# wait for start button
try:
    # declare handler for estop
    #cb1 = pi.callback(STOP_GPIO, pigpio.EITHER_EDGE, stop_handler)
    arm.startPosition()
    #oled.dispVT()
    buttonNotPressed = True
    print("wait for start button")
    while buttonNotPressed:
        buttonState = pi.read(START_GPIO)
        if not buttonState:
            buttonNotPressed = False
    print('STARTING INTEGRATION MAIN')
    sleep(2)
    integration.main()
except Exception as e:
    print('The following error occured: ')
    print(e)
    stop_handler()
