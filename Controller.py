activate_this = "env/scripts/activate_this.py"
with open(activate_this) as f:
        code = compile(f.read(), activate_this, 'exec')
        exec(code, dict(__file__=activate_this))

import serial
import time
import pygame
from threading import *

ser = serial.Serial('COM3', baudrate = 9600, timeout = 1)
time.sleep(1)

move_forward = None
move_backward = None
joy_left = None
joy_right = None
axis_1 = None
axis_3 = None

class serialRead(Thread):
    def run(self):
        while True:
            if ser.is_open == False:
                ser.open()

            s = ser.readline()
            s = s.decode("utf-8")
            s = s.replace('\r', '').replace('\n','')
            print(s)
            time.sleep(0.7)

class joystickClass(Thread):

    def __init__(self):

        Thread.__init__(self)

        self.button=None

    def run(self):
        global move_forward
        global move_backward
        global joy_left
        global joy_right
        global axis_1
        global axis_3

        pygame.init()
        pygame.joystick.init()
        joystick_count = [pygame.joystick.Joystick(i) for i in range(pygame.joystick.get_count())]
        while True:
            for event in pygame.event.get():
                if event.type == pygame.JOYBUTTONDOWN:
                    if (event.button == 3):
                        move_forward = True
                    
                    if (event.button == 0):
                        move_backward = True

                elif event.type == pygame.JOYBUTTONUP:
                    if (event.button == 3):
                        move_forward = False

                    if (event.button == 0):
                        move_backward = False

                elif event.type == pygame.JOYAXISMOTION:
                    if (event.axis == 2):
                        joy_left = True
                        axis_1 = event.value
                    if (event.axis == 3):
                        joy_right = True
                        axis_3 = event.value

            time.sleep(0.1)

class serialOut(Thread):
    def __init__(self):

        Thread.__init__(self)
        self.button=None

    def run(self):
        global move_forward
        global move_backward
        global joy_left
        global joy_right
        global axis_1
        global axis_3

        while True:
            if ser.is_open == False:
                ser.open()

            if move_forward:
                print("Y: Pressed")
                ser.write(b'f|')
                ser.flush()
                time.sleep(0.65)

            if move_backward:
                print("A: Pressed")
                ser.write(b'r|')
                ser.flush()
                time.sleep(0.65)

            if joy_left:
                string_Z_Data = 'z'+str(axis_1)+'|'
                byte_Z_Data = bytes(string_Z_Data, 'utf-8')
                ser.write(byte_Z_Data)
                time.sleep(0.05)

            if joy_right:
                string_X_Data = 'x'+str(axis_3)+'|'
                byte_X_Data = bytes(string_X_Data, 'utf-8')
                ser.write(byte_X_Data)
                time.sleep(0.05)

jB = joystickClass()
sO = serialOut()
sR = serialRead()

jB.start()
sO.start()
sR.start()
