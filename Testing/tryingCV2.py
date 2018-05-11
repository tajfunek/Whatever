import cv2
import time
_time = time.time()
for i in range(10000):
    img = cv2.imread('try.png')
print("Time: ", time.time() - _time, "\n")
print("open CV")
#print(img)
