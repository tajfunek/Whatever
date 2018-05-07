from skimage import io
import time
_time = time.time()
for i in range(10000):
    file = io.imread("try.png")
print("Time: ", time.time() - _time, "\n")
print('Scikit')
#print(file)
