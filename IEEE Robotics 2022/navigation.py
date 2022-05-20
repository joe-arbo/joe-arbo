import string
from cv2 import stereoCalibrate
import steppermotortest
import serial
from time import sleep

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=3)
ser.reset_input_buffer()

# def centreOnTrack():
#     pass
def moveCloseToNet(robot):
    reading= True
    # read until something is read
    stringToSend = 'N'
    
    if robot.net_on_right:
        stringToSend += 'R'
    else:
        stringToSend += 'L'
    
    stringToSend += '\n'
    
    while reading:
        ser.write(stringToSend.encode('utf-8'))
        # ser.reset_input_buffer()
        line = ser.readline().decode('utf-8').rstrip()
        # print(f'received from arduino: {line}')
        # print(line)
        if line:
            break

    while line != "S":
        if line == "F":
            steppermotortest.forwards()
        elif line == "B":
            steppermotortest.backwards()
        elif line == "C":
            steppermotortest.clockwise()
        elif line == "CC":
            steppermotortest.counterClockwise()
        elif line == "L":
            steppermotortest.left()
        elif line == "R":
            steppermotortest.right()

        line = ser.readline().decode('utf-8').rstrip()
    
    steppermotortest.stopMoving()
    
def moveCenter(robot):
    reading= True
    # read until something is read
    stringToSend = 'C'
    
    if robot.net_on_right:
        stringToSend += 'L'
    else:
        stringToSend += 'R'
    
    stringToSend += '\n'
    
    while reading:
        ser.write(stringToSend.encode('utf-8'))
        # ser.reset_input_buffer()
        line = ser.readline().decode('utf-8').rstrip()
        # print(f'received from arduino: {line}')
        # print(line)
        if line:
            break

    while line != "S":
        if line == "F":
            steppermotortest.forwards()
        elif line == "B":
            steppermotortest.backwards()
        elif line == "C":
            steppermotortest.clockwise()
        elif line == "CC":
            steppermotortest.counterClockwise()
        elif line == "L":
            steppermotortest.left()
        elif line == "R":
            steppermotortest.right()

        line = ser.readline().decode('utf-8').rstrip()
    
    steppermotortest.stopMoving()

def navigate(robot):
    reading= True
    # read until something is read
    stringToSend = ''
    if robot.next_location == 1:
        stringToSend += '1'
    elif robot.next_location == 2:
        stringToSend += '2'
    elif robot.next_location == 3:
        stringToSend += '3'
    elif robot.next_location == 4:
        stringToSend += '4'
    elif robot.next_location == 5:
        stringToSend += '5'
    elif robot.next_location == 6:
        stringToSend += '6'
    elif robot.next_location == 7:
        stringToSend += '7'
    elif robot.next_location == 8:
        stringToSend += '8'
    elif robot.next_location == 9:
        stringToSend += '9'
    elif robot.next_location == 10:
        stringToSend += 'A'
    elif robot.next_location == 11:
        stringToSend += 'B'

    if robot.forward:
        stringToSend += 'F'
    else:
        stringToSend += 'B'

    stringToSend += '\n'
    
    
    
    while reading:
        ser.write(stringToSend.encode('utf-8'))
        # ser.reset_input_buffer()
        line = ser.readline().decode('utf-8').rstrip()
        # print(f'received from arduino: {line}')
        # print(line)
        if line:
            break

    while line != "S":
        if line == "F":
            steppermotortest.forwards()
        elif line == "B":
            steppermotortest.backwards()
        elif line == "C":
            steppermotortest.clockwise()
        elif line == "CC":
            steppermotortest.counterClockwise()
        elif line == "L":
            steppermotortest.left()
        elif line == "R":
            steppermotortest.right()

        line = ser.readline().decode('utf-8').rstrip()
    
    steppermotortest.stopMoving()
