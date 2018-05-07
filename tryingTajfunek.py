import PNG_read
import time
_time = time.time()
for i in range(1000):
    pixels = PNG_read.read("try.png")
print("Time: ", time.time() - _time, "\n")
print("Tajfunek's PNG_read")
#print(pixels)
