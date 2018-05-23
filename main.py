import driver
import os
import RPi.GPIO as GPIO
import subprocess as s
import shlex
import sys
import time as t

_t=t.time()

GPIO.setwarnings(False)
os.nice(-5)
driver.setup()
GPIO.setup((21, 20, 26), GPIO.OUT)
print("Starting...\n")
stepdeg = 4 # resolution - sth is wrong 64*8 is 360deg. not 64*64
motor_time = 10 # in ms, '10' Can be changed to lower if works
turns = int(256/stepdeg)

try:
    for i in range(turns):
        print("ITERATION: ", i)
        GPIO.output(21, 0)
        GPIO.output(20, 1)
        GPIO.output(26, 1)
        print("GPIO set")

        print("CAM1")
        error = 1
        while error:
            error = 0
            #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             --set brightness=5 --set contrast=255 --rotate 270 \
             images_new/1_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
                error1 = error1.decode('utf-8')
                #print(error1)
            except:
                cam1.kill()
                print("ERROR")
                sys.exit()
            finally:
                pass
                #print("Done")
            #error1 = error1.decode('utf-8')
            if "Writing PNG image to 'images_new/" not in error1:
                error = 1
        # Next camera
        print("CAM2")

        GPIO.output(21, 1)
        GPIO.output(20, 1)
        GPIO.output(26, 0)

        error = 1
        while error:
            error = 0
                #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             --set brightness=5 --set contrast=255 --rotate 270 \
             images_new/1_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
                error1 = error1.decode('utf-8')
                #print(error1)
            except:
                cam1.kill()
                print("ERROR")
                sys.exit()
            finally:
                pass
                #print("Done")
            #error1 = error1.decode('utf-8')
            if "Writing PNG image to 'images_new/" not in error1:
                error = 1

        print("CAM3")
        GPIO.output(21, 1)
        GPIO.output(20, 0)
        GPIO.output(26, 1)
        error = 1
        while error:
            error = 0
            #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             --set brightness=5 --set contrast=255 --rotate 270 \
             images_new/1_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
                error1 = error1.decode('utf-8')
                #print(error1)
            except:
                cam1.kill()
                print("ERROR")
                sys.exit()
            finally:
                pass
                #print("Done")
            #error1 = error1.decode('utf-8')
            if "Writing PNG image to 'images_new/" not in error1:
                error = 1

        driver.forward(motor_time/1000, stepdeg) # '10' Can be changed to lower if works
except KeyboardInterrupt:
    GPIO.cleanup()
    sys.exit()

print("TIME: ", t.time() - _t)
GPIO.cleanup()
