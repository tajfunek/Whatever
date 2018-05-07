import png
import time
_time = time.time()
for i in range(10000):
    reader = png.Reader("try.png")
    w, h, pixels, metadata = reader.read_flat()
print("Time: ", time.time() - _time, "\n")
print('PyPNG')
#print(pixels)
