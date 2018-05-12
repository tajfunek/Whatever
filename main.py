import driver
import os
import RPi.GPIO as GPIO
import subprocess as s
import shlex
import sys
import time as t

_t=t.time()

# GPIO.setwarnings(False)
os.nice(-5)
driver.setup()
GPIO.setup((21, 20), GPIO.OUT)
print("Starting...\n")
stepdeg = 16 # resolution
motor_time = 10 # in ms
turns = int(64*64/stepdeg)

for i in range(turns):
    print("ITERATION: ", i)
    GPIO.output(21, 1)
    GPIO.output(20, 0)

    #print("CAM1")
    error = 1
    while error:
        error = 0
        #print("Iteration")
        cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
         --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
         images_new/1_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
        try:
            #print("Communication")
            _, error1 = cam1.communicate(timeout = 10)
        except:
            cam1.kill()
            print("ERROR")
            sys.exit()
        finally:
            pass
            #print("Done")
        error1 = error1.decode('utf-8')
        for line in error1.splitlines():
            if line.startswith('Error') or line.startswith('stat'):
                error = 1
                break

        # Next camera
        #print("CAM2")
        error = 1
        while error:
            error = 0
            #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video1 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             images_new/2_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
            except:
                cam1.kill()
                print("ERROR")
                sys.exit()
            finally:
                pass
                #print("Done")
            error1 = error1.decode('utf-8')
            for line in error1.splitlines():
                if line.startswith('Error') or line.startswith('stat'):
                    error = 1
                    break

        #print("CAM3")
        GPIO.output(21, 0)
        GPIO.output(20, 1)
        error = 1
        while error:
            error = 0
            #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video2 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             images_new/3_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
            except:
                cam1.kill()
                print("ERROR")
                sys.exit()
            finally:
                pass
                #print("Done")
            error1 = error1.decode('utf-8')
            for line in error1.splitlines():
                if line.startswith('Error') or line.startswith('stat'):
                    error = 1
                    break

        driver.forward(motor_time/1000, stepdeg) # '10' Can be changed to lower if works


print("TIME: ", t.time() - _t)
GPIO.cleanup()