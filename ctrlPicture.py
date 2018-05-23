import os
import subprocess as s
import shlex
import sys

print("CAM1")
error = 1
while error:
    error = 0
    cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
     --no-banner --png --no-title --no-subtitle --no-timestamp --no-info --rotate 90 \
     ctrlPicture.png"), stdout = s.PIPE, stderr = s.PIPE)
    try:
        #print("Communication")
        _, error1 = cam1.communicate(timeout = 10)
        error1 = error1.decode('utf-8')
        print(error1)
    except:
        cam1.kill()
        print("ERROR")
        sys.exit()
    finally:
        pass
        #print("Done")
    error1 = error1.decode('utf-8')
    if "Writing PNG image to 'images_new/" not in error1:
        error = 1
print("Done")
