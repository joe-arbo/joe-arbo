# Import packages
import os
import argparse
import cv2
import numpy as np
import sys
import time
from threading import Thread
import importlib.util

# Define VideoStream class to handle streaming of video from webcam in separate processing thread
# Source - Adrian Rosebrock, PyImageSearch: https://www.pyimagesearch.com/2015/12/28/increasing-raspberry-pi-fps-with-python-and-opencv/
class VideoStream:
    """Camera object that controls video streaming from the Picamera"""
    def __init__(self,resolution=(640,480),framerate=30):
        # Initialize the PiCamera and the camera image stream
        self.stream = cv2.VideoCapture(0)
        ret = self.stream.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
        ret = self.stream.set(3,resolution[0])
        ret = self.stream.set(4,resolution[1])
            
        # Read first frame from the stream
        (self.grabbed, self.frame) = self.stream.read()

	# Variable to control when the camera is stopped
        self.stopped = False

    def start(self):
	# Start the thread that reads frames from the video stream
        Thread(target=self.update,args=()).start()
        return self

    def update(self):
        # Keep looping indefinitely until the thread is stopped
        while True:
            # If the camera is stopped, stop the thread
            if self.stopped:
                # Close camera resources
                self.stream.release()
                return

            # Otherwise, grab the next frame from the stream
            (self.grabbed, self.frame) = self.stream.read()

    def read(self):
	# Return the most recent frame
        return self.frame

    def stop(self):
	# Indicate that the camera and thread should be stopped
        self.stopped = True


min_conf_threshold_net = 0.7
min_conf_threshold_bracelet = 0.5
resW, resH = 640, 480
imW, imH = int(300), int(300)
use_TPU = False

# Import TensorFlow libraries
# If tflite_runtime is installed, import interpreter from tflite_runtime, else import from regular tensorflow
# If using Coral Edge TPU, import the load_delegate library
pkg = importlib.util.find_spec('tflite_runtime')
if pkg:
    from tflite_runtime.interpreter import Interpreter
    if use_TPU:
        from tflite_runtime.interpreter import load_delegate
else:
    from tensorflow.lite.python.interpreter import Interpreter
    if use_TPU:
        from tensorflow.lite.python.interpreter import load_delegate

# If using Edge TPU, assign filename for Edge TPU model
if use_TPU:
    # If user has specified the name of the .tflite file, use that name, otherwise use default 'edgetpu.tflite'
    if (GRAPH_NAME == 'detect.tflite'):
        GRAPH_NAME = 'edgetpu.tflite'       

# Get path to current working directory
CWD_PATH = os.getcwd()

# Path to .tflite file, which contains the model that is used for object detection
PATH_TO_NET_CKPT = './robot_net_model.tflite'
# Path to label map file
PATH_TO_NET_LABELS = './net_label_map.txt'

# Path to .tflite file, which contains the model that is used for object detection
PATH_TO_BRACELET_CKPT = './bracelet_model.tflite'
# Path to label map file
PATH_TO_BRACELET_LABELS = './bracelet_label_map.txt'

# Load the label map
with open(PATH_TO_NET_LABELS, 'r') as f:
    net_labels = [line.strip() for line in f.readlines()]
# Load the label map
with open(PATH_TO_BRACELET_LABELS, 'r') as f:
    bracelet_labels = [line.strip() for line in f.readlines()]

# Load the Tensorflow Lite model.
# If using Edge TPU, use special load_delegate argument
if use_TPU:
    net_interpreter = Interpreter(model_path=PATH_TO_NET_CKPT,
                              experimental_delegates=[load_delegate('libedgetpu.so.1.0')])
    print(PATH_TO_BRACELET_CKPT)
else:
    net_interpreter = Interpreter(model_path=PATH_TO_NET_CKPT)

net_interpreter.allocate_tensors()

# Get model details
net_input_details = net_interpreter.get_input_details()
net_output_details = net_interpreter.get_output_details()
height = net_input_details[0]['shape'][1]
width = net_input_details[0]['shape'][2]

net_floating_model = (net_input_details[0]['dtype'] == np.float32)

# Load the Tensorflow Lite model.
# If using Edge TPU, use special load_delegate argument
if use_TPU:
    bracelet_interpreter = Interpreter(model_path=PATH_TO_BRACELET_CKPT,
                              experimental_delegates=[load_delegate('libedgetpu.so.1.0')])
    print(PATH_TO_BRACELET_CKPT)
else:
    bracelet_interpreter = Interpreter(model_path=PATH_TO_BRACELET_CKPT)

bracelet_interpreter.allocate_tensors()

# Get model details
bracelet_input_details = bracelet_interpreter.get_input_details()
bracelet_output_details = bracelet_interpreter.get_output_details()
height = bracelet_input_details[0]['shape'][1]
width = bracelet_input_details[0]['shape'][2]

bracelet_floating_model = (bracelet_input_details[0]['dtype'] == np.float32)

input_mean = 127.5
input_std = 127.5
# Initialize video stream
videostream = VideoStream(resolution=(imW,imH),framerate=30).start()
# time.sleep(1)

def stopVideoThread():
    videostream.stop()

def isNet():

    # Grab frame from video stream
    frame1 = videostream.read()

    # Acquire frame and resize to expected shape [1xHxWx3]
    frame = frame1.copy()
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame_resized = cv2.resize(frame_rgb, (width, height))
    input_data = np.expand_dims(frame_resized, axis=0)

    # Normalize pixel values if using a floating model (i.e. if model is non-quantized)
    if net_floating_model:
        input_data = (np.float32(input_data) - input_mean) / input_std

    # Perform the actual detection by running the model with the image as input
    net_interpreter.set_tensor(net_input_details[0]['index'],input_data)
    net_interpreter.invoke()

    # Retrieve detection results
    boxes = net_interpreter.get_tensor(net_output_details[1]['index'])[0] # Bounding box coordinates of detected objects
    classes = net_interpreter.get_tensor(net_output_details[3]['index'])[0] # Class index of detected objects
    scores = net_interpreter.get_tensor(net_output_details[0]['index'])[0] # Confidence of detected objects
    #num = interpreter.get_tensor(output_details[3]['index'])[0]  # Total number of detected objects (inaccura
    
    # Loop over all detections and draw detection box if confidence is above minimum threshold
    for i in range(len(scores)):
        if ((scores[i] > min_conf_threshold_net) and (scores[i] <= 1.0)):
            # NET DETECTED
            print("Net detected")
            return True
    # NET NOT DETECTED
    print("Net NOT detected")
    return False

def directionOfBracelet():
    # Grab frame from video stream
    frame1 = videostream.read()

    # Acquire frame and resize to expected shape [1xHxWx3]
    frame = frame1.copy()
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame_resized = cv2.resize(frame_rgb, (width, height))
    input_data = np.expand_dims(frame_resized, axis=0)

    # Normalize pixel values if using a floating model (i.e. if model is non-quantized)
    if bracelet_floating_model:
        input_data = (np.float32(input_data) - input_mean) / input_std

    # Perform the actual detection by running the model with the image as input
    bracelet_interpreter.set_tensor(bracelet_input_details[0]['index'],input_data)
    bracelet_interpreter.invoke()

    # Retrieve detection results
    boxes = bracelet_interpreter.get_tensor(bracelet_output_details[1]['index'])[0] # Bounding box coordinates of detected objects
    classes = bracelet_interpreter.get_tensor(bracelet_output_details[3]['index'])[0] # Class index of detected objects
    scores = bracelet_interpreter.get_tensor(bracelet_output_details[0]['index'])[0] # Confidence of detected objects
    #num = interpreter.get_tensor(output_details[3]['index'])[0]  # Total number of detected objects (inaccura
    
    # Loop over all detections and draw detection box if confidence is above minimum threshold
    for i in range(len(scores)):
        if ((scores[i] > min_conf_threshold_bracelet) and (scores[i] <= 1.0)):
            # Get bounding box coordinates and draw box
            # Interpreter can return coordinates that are outside of image dimensions, need to force them to be within image using max() and min()
            ymin = int(max(1,(boxes[i][0] * imH)))
            xmin = int(max(1,(boxes[i][1] * imW)))
            ymax = int(min(imH,(boxes[i][2] * imH)))
            xmax = int(min(imW,(boxes[i][3] * imW)))

            # Draw circle in center
            xcenter = xmin+(int(round((xmax - xmin) / 2)))
            ycenter = ymin+(int(round((ymax - ymin) / 2)))

            # TODO: output differently depending on result THIS DOES NOT WORK RIGHT NOW
            if (xcenter < 147):
                return "R"
            if (xcenter > 153):
                return "L"
            return "G"
    # BRACELET NOT DETECTED
    return "N"
