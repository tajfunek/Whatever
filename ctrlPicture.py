import os
import subprocess as s
import shlex
import sys

cam1 = s.Popen(shlex.split("sudo fswebcam --resolution 1280x1024 --device /dev/video0 \
 --no-banner --png --no-title --no-subtitle --no-timestamp --no-info --rotate 90 \
 ctrlPicture.png", stdout = s.PIPE, stderr = s.PIPE))
