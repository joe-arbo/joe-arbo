'''
This is skeleton code for the main FSM of the robot.



Rough quick reference for competition board
1,3,5,6,7,910 = Net/Cup
4 = Turn
P = powerline pole
2T,8T = Trees
"""""""""""""""""""""""""""""""""""""""""""""""""""""""
|                                    |          |     |
|    10                 P    7       |          |1    |
|""""""""""""         """"""""""""""""          |     |
|                                               |2T   |
|                                               |3    |
|                                               |     |
|""""""""""""         """""""""""""""""""""""""""     |
|    9   8T             P     6              5  4     |
|                                                     |
"""""""""""""""""""""""""""""""""""""""""""""""""""""""
'''


from time import sleep
import camera
import navigation
import arm
import steppermotortest

# variables to be used for readability when changing states
start_up = 0
grab_beads = 1
fire_catapult = 2
nav = 3
align_with_tree = 4
align_with_net = 5
look_at_right_side = 6
detect_net = 7
return_to_start = 8

# arbitrary class currently being used for readability, it may be
# decided later on to put something here
class Startup():
    def __init__(self):
        pass

    def execute(self, robot):
        # print("Start")
        # arm.startPosition()
        robot.state = nav

# arm
class GrabBeads():
    def __init__(self):
        pass

    def execute(self, robot):
        # move arm to grab beads and loads catapult
        print(f"robot tree: {robot.next_tree}")
        if robot.next_tree == 1:
            print("arm aligned pre cam")
            arm.retrieveBraceletsPreCam()
            print("arm aligned pre cam")
            direction = camera.treeAlign()
            print("camera aligning arm")
            while direction != "G":
                print(direction)
                if direction == "L":
                    steppermotortest.forwardsSmaller()
                if direction == "R":
                    steppermotortest.backwardsSmaller()
                sleep(0.5)
                direction = camera.treeAlign()
            print("Camera aligned")
            # steppermotortest.forwardsSmall()
            arm.retrieveBraceletsPostCam()

        elif robot.next_tree == 2:
            print("arm aligned pre cam")            
            arm.retrieveBracelets2PreCam()
            direction = camera.treeAlign()
            print("camera aligning arm")
            while direction != "G":
                print(direction)
                if direction == "L":
                    steppermotortest.forwardsSmaller()
                if direction == "R":
                    steppermotortest.backwardsSmaller()
                sleep(0.5)
                direction = camera.treeAlign()
            print("Camera aligned")
            # steppermotortest.forwardsSmall()
            arm.retrieveBracelets2PostCam()
            
        robot.catapult_loaded = True
        # print("Grab Beads")
        robot.state = nav

'''
arm
class PutBeadsInCatapult():
    def __init__(self):
        pass

    def execute(self, robot):
        # move arm over and place beads in catapult
        arm.beadsToCatapult()
        print("Place beads in catapult")
        robot.catapult_loaded = True
        robot.state = center_robot
        pass
'''

# arm
class FireCatapult():
    def __init__(self):
        pass

    def execute(self, robot):
        navigation.moveCloseToNet(robot)
        if not robot.net_on_right:
            arm.catapultSwingLeft()
#             if robot.next_location == 6:
#                 arm.catapultSwingPos5()
#             else:
#                 arm.catapultSwingLeft()

        # print("Fire catapult")
        arm.launchBracelets()
        # swing catapult back to the left
        if not robot.net_on_right:
            arm.catapultSwingRight()
            
        navigation.moveCenter(robot)
        
        if robot.arm_on_right:
            # arm.lookLeft()
            arm.lookLeftInstant()
            robot.arm_on_right = False

        robot.catapult_loaded = False
        robot.state = nav

# camera/arm
class LookAtRightSide():
    def __init__(self):
        pass

    def execute(self, robot):
        # aim arm at right side of robot for camera to detect net
        # arm.lookRight()
        arm.lookRightInstant()
        robot.arm_on_right = True
        sleep(1)
        robot.state = detect_net

# camera
class DetectNet():
    def __init__(self):
        pass

    def execute(self, robot):
        # print("Detecting net...")
        if camera.detectNet():
            # print("Net detected")
            robot.net_on_right = robot.arm_on_right
            robot.state = fire_catapult
        else:
            # print("No net detected")
            # move to next cup/net location?
            if robot.arm_on_right:
                # arm.lookLeft()
                arm.lookLeftInstant()
                robot.arm_on_right = False
            robot.state = nav

# camera/navigation
class AlignWithTree():
    def __init__(self):
        pass

    def execute(self, robot):
        # using camera, more precisely line up robot with first tree and move robot as close as possible
        dir = camera.treeAlign()
        # change this to use serial read/write with arduino
        while dir != "Good":
            print("Stuck in loop?")
            if dir == "F":
                steppermotortest.forwardSmall()
            elif dir == "B":
                steppermotortest.backwardsSmall()
        # print("Align with tree")
        robot.state = grab_beads

class AlignWithNet():
    def __init__(self):
        pass

    def execute(self, robot):
        # using camera, more precisely line up robot with first tree and move robot as close as possible
        dir = camera.netAlign()
        # change this to use serial read/write with arduino
        while dir != "Good":
            print("Stuck in loop?")
            if dir == "F":
                pass
            elif dir == "B":
                pass
        # print("Align with tree")
        robot.state = fire_catapult

'''
navigation
class CenterRobot():
    def __init__(self):
        pass

    def execute(self, robot):
        navigation.centreOnTrack()
        print("Center robot on track")
        robot.state = nav
'''

# navigation
class Navigation():
    def __init__(self):
        pass

    def execute(self, robot):
        print(robot.next_location)
        if robot.next_location == 1:
            # print("Reverse to position 1")
            # navigation.reverseTo1()
            navigation.navigate(robot)
            robot.state = return_to_start
        elif robot.next_location == 2:
            if robot.forward:
                # print("Forward to tree 1")
                # navigation.forwardTree1()
                navigation.navigate(robot)
            else:
                # navigation.reverseTree1()
                navigation.navigate(robot)
            if robot.next_tree < 2:
                robot.state = grab_beads
                robot.next_tree += 1
        elif robot.next_location == 3:
            if robot.forward:
                # print("Forward to location 3")
                # navigation.forwardTo3()
                navigation.navigate(robot)
            else:
                # print("Reverse to location 3")
                # navigation.reverseTo3()
                navigation.navigate(robot)
            if robot.catapult_loaded:
                robot.state = detect_net
        elif robot.next_location == 4:
            if robot.forward:
                # print("Turn1")
                # navigation.turn1()
                navigation.navigate(robot)
            else:
                # print("Turn2 (Turn1 in reverse)")
                # navigation.turn2()
                navigation.navigate(robot)
        elif robot.next_location == 5:
            if robot.forward:
                # print("Forward to location 5")
                # navigation.forwardTo5()
                navigation.navigate(robot)
            else:
                # print("Reverse to location 5")
                # navigation.reverseTo5()
                navigation.navigate(robot)
            if robot.catapult_loaded:
                robot.state = detect_net
        elif robot.next_location == 6:
            if robot.forward:
                # print("Forward to location 6")
                # navigation.forwardTo6()
                navigation.navigate(robot)
            # else:
                # print("Robot is going backwards and is already at position 6")
            if robot.catapult_loaded:
                robot.state = detect_net
        elif robot.next_location == 7:
            # if robot.forward:
            #     print("Robot is already at position 6, so if\n\tcatapult is loaded, check for net at location 7")
            # else:
            if not robot.forward:
                # print("Reverse to location 7")
                # navigation.reverseTo7()
                navigation.navigate(robot)
            if robot.catapult_loaded:
                robot.state = look_at_right_side
        elif robot.next_location == 8:
            if robot.forward:
                # print("Forward to tree 1")
                # navigation.forwardTree2()
                navigation.navigate(robot)
            else:
                # navigation.reverseTree2()
                navigation.navigate(robot)
            if robot.next_tree < 2:
                robot.state = grab_beads
                robot.next_tree += 1
        elif robot.next_location == 9:
            if robot.forward:
                # print("Forward to location 9")
                # navigation.forwardTo9()
                navigation.navigate(robot)
            # else:
                # print("Robot already at location 9, do nothing")
            if robot.catapult_loaded and robot.forward:
                robot.state = detect_net
        elif robot.next_location == 10:
            # print("If catapult is loaded, check for net at location 10")
            if not robot.forward:
                navigation.navigate(robot)
                
            if robot.catapult_loaded and robot.forward:
                robot.state = look_at_right_side
        elif robot.next_location == 11:
            navigation.navigate(robot)
            robot.forward = False
        
        #print(f"Robot forward: {robot.forward}")
        if robot.forward:
            robot.next_location += 1
        else:
            robot.next_location -= 1
        
        
# navigation
class ReturnToStart():
    def __init__(self):
        pass

    def execute(self, robot):
        pass

class RobotFSM():
    def __init__(self):
        self.states =  [Startup(),
                        GrabBeads(),
                        # PutBeadsInCatapult(),
                        FireCatapult(),
                        # CenterRobot(),
                        Navigation(),
                        AlignWithTree(),
                        AlignWithNet(),
                        LookAtRightSide(),
                        DetectNet(),
                        # MoveTree1(),
                        # MoveTree2(),
                        # Turn1(),
                        # Turn2(),
                        ReturnToStart()]
        self.catapult_loaded = False
        self.next_location = 2
        self.arm_on_right = False
        self.net_on_right = False
        self.next_tree = 0
        self.forward = True
        self.state = 0

def main():
    robot = RobotFSM()
    
    while True:
        robot.states[robot.state].execute(robot)
        if robot.state == return_to_start:
            print("Success!")
            break

#     arm.captapultSwingRight()
#     sleep(1)
#     arm.startPosition()
#     arm.deinitialize()

    camera.stopCameraThread()
    steppermotortest.turnOffMotors()
    arm.servosOff()


def stopCamera():
    camera.stopCameraThread()


# try:
#    OLED()
#    main()

# except:
#     steppermotortest.turnOffMotors()
#     camera.stopCameraThread()
#     arm.servosOff()
#     print("\nCtrl-C pressed. Stopping PIGPIO and exiting...")
#     print('motors off')