import driver
import os
import RPi.GPIO as GPIO
import subprocess as s
import shlex
import sys
import time as t

GPIO.setwarnings(False)
os.nice(-5)
driver.setup()
GPIO.setup((21, 20), GPIO.OUT)
print("Starting...\n")
stepdeg = 16 # resolution
turns = 64*64/stepdeg

for i in range(1):
    GPIO.output(21, 1)
    GPIO.output(20, 0)

    error = 1
    while error:
        _t = t.time()
        error = 0
        #print("Iteration")
        cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
         --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
         1_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
        try:
            #print("Communication")
            _, error1 = cam1.communicate(timeout = 10)
        except s.TimeoutExpired:
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
        error = 1
        while error:
            _t = t.time()
            error = 0
            #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video1 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             2_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
            except s.TimeoutExpired:
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

        GPIO.output(21, 0)
        GPIO.output(20, 1)
        error = 1
        while error:
            _t = t.time()
            error = 0
            #print("Iteration")
            cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video2 \
             --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
             3_{}.png".format(i)), stdout = s.PIPE, stderr = s.PIPE)
            try:
                #print("Communication")
                _, error1 = cam1.communicate(timeout = 10)
            except s.TimeoutExpired:
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

        driver.forward(10, stepdeg) # Can be changed to lower if works


print("TIME: ", t.time() - _t)
GPIO.cleanup()
