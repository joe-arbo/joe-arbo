import os
import time
import Adafruit_SSD1306
from PIL import Image
from board import SCL, SDA
import busio

i2c = busio.I2C(board.SCL, board.SDA)

# 128x32 display with hardware I2C:
#disp = Adafruit_SSD1306.SSD1306_128_32(rst=None)

# 128x64 display with hardware I2C:
#disp = Adafruit_SSD1306.SSD1306_128_64(rst=None, i2c, addr=0x3D)
disp = Adafruit_SSD1306.SSD1306_I2C(128, 64, rst=None, i2c, addr=0x3D)

# Initialize library.
disp.begin()
# Clear display.
disp.clear()
disp.display()

def dispVT():

    # Clear display.
    disp.clear()
    disp.display()

    # Open image file
    image = Image.open(vtlogoresized.png)

    # Check if image size matches display size
    if image.size==(disp.width, disp.height):
        # Convert image to 1-bit colour
        image=image.convert('1')
    else:
        # Resize to match display and convert to 1-bit colour
        image=image.resize((disp.width, disp.height), Image.ANTIALIAS).convert('1')

    # Display image.
    disp.image(image)
    disp.display()

def dispImages():
        # Default delay of 1 second unless provided
    # via command line parameter
    delay=5

    # List of image file extensions to look for
    imageExtensions=[".jpg",".pgm",".ppm",".pbm",".bmp",".png"]

    # 128x32 display with hardware I2C:
    #disp = Adafruit_SSD1306.SSD1306_128_32(rst=None)

    # 128x64 display with hardware I2C:
    disp = Adafruit_SSD1306.SSD1306_128_64(rst=None)

    # Initialize library.
    disp.begin()

    # Clear display.
    disp.clear()
    disp.display()

    while True:

        # Find all files in current directory
        for root, dirs, files in os.walk("."):
            for filename in files:
                # Split filename to find file extension
                head, ext=os.path.splitext(filename)

            # Check if file has suitable image extension
            if ext in imageExtensions:

                print(filename)

                # Open image file
                image = Image.open(filename)

                # Check if image size matches display size
                if image.size==(disp.width, disp.height):
                    # Convert image to 1-bit colour
                    image=image.convert('1')
                else:
                    # Resize to match display and convert to 1-bit colour
                    image=image.resize((disp.width, disp.height), Image.ANTIALIAS).convert('1')

                # Display image.
                disp.image(image)
                disp.display()

                # Wait before showing next image
                time.sleep(delay)