import camera_helper

def detectNet():
    return camera_helper.isNet()

def treeAlign():
    return camera_helper.directionOfBracelet()

def netAlign():
    return "Good"

def stopCameraThread():
    camera_helper.stopVideoThread()

