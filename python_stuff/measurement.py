import driver
import RPi.GPIO as GPIO
import subprocess as s
import shlex
import sys
import calculateTEMP as cd
GPIO.setwarnings(False)
driver.setup()
GPIO.setup((21, 20, 26), GPIO.OUT)
print("Starting...\n")

GPIO.output(21, 0)
GPIO.output(20, 0)
GPIO.output(26, 0)
print("GPIO set: laser ON")
device = input("device number (0/1/2): ")
laser = input("laser number (21/20/26): ")
if laser == 21:
    GPIO.output(21, 0)
    GPIO.output(20, 1)
    GPIO.output(26, 1)
elif laser == 20:
    GPIO.output(21, 1)
    GPIO.output(20, 0)
    GPIO.output(26, 1)
elif laser == 26:
    GPIO.output(21, 1)
    GPIO.output(20, 1)
    GPIO.output(26, 0)
#zrobienie zdjęcia:
error = 1
while error:
    error = 0
    cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video{} \
     --no-banner --png --no-title --no-subtitle --no-timestamp --no-info \
     --set brightness=128 --set contrast=128 \
     ./msm.png".format(device)), stdout = s.PIPE, stderr = s.PIPE)#change brightness and contrast
    try:
        _, error1 = cam1.communicate(timeout = 15)
        error1 = error1.decode('utf-8')
        if "Writing PNG image to" not in error1:
            error = 1
            print(error1)
    except:
        error = 1

GPIO.output(21, 1)
GPIO.output(20, 1)
GPIO.output(26, 1)
print("GPIO set: laser OFF")
print("Picture taken, directory: Whatever/python_stuff/msm.png")
