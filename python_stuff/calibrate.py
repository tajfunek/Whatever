import calculate as cd
import png
import math
plateR = 75
camdis = 231.75
expectedDIS = camdis - plateR
#parametry z kamery:
f = 1155
laserDIS = 100
laserDEG = 28.5
resY = 1280
expectedX = f*math.tan(math.radians(laserDEG)) - (f*laserDIS/expectedDIS)
output_old = []
output_new = []
calibration = []
errors_old = []
errors_new = []
pixels_new = []
pixels = []
AVGerror_old = 0
AVGerror_new = 0
pixels = cd.extract("1_000","testimages/", 0)
longY = resY / len(pixels)

for pixel in pixels:
    output_old.append(cd.calculate(*pixel))
for pixel in pixels:
    calibration.append(expectedX - pixel[0])
for i in range(len(pixels)):
    pixelx = pixels[i]
    pixel = [(pixelx[0] + calibration[i]) , pixelx[1], pixelx[2]]
    pixels_new.append(pixel)
for pixel in pixels_new:
    output_new.append(cd.calculate(*pixel))

for result in output_old:
    result = math.sqrt((result[0]**2) + (result[1]**2))
    errors_old.append(expectedDIS - result)
    AVGerror_old += (expectedDIS - result)
for result in output_new:
    result = math.sqrt((result[0]**2) + (result[1]**2))
    errors_new.append(expectedDIS - result)
    AVGerror_new += (expectedDIS - result)
AVGerror_old = AVGerror_old / len(output_old)
AVGerror_new = AVGerror_new / len(output_new)
print("AVGerror_old: ", AVGerror_old)
print("AVGerror_new: ", AVGerror_new)


file = open("calibration.txt", "w")
file.write(str(longY) + "\n")
for i in calibration:
    file.write(str(i) + "\n")
file.close()
