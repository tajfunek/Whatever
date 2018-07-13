import calculate as c
import math
extracted = c.extract()
if extracted is not None:
    output = []
    calibration = open("calibration.txt", "r")

    for point in extracted:

        output.append(c.calculate(point[0], point[1], point[2]))

    print(output)
