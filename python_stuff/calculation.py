import math
import calculateTEMP as cd

folder = "testimages/"
filename = "testpic1000.png"
#prawdziwe wymiary:
#wspólne stałe parametry kamery i skanera oraz zdjęcia:
w = 1000        # szerokość zdjęcia
h = 1000        # wysokość zdjęcia
f = 500         # f to ogniskowa w pixelach (odległość "matryca-obiektyw")
laserDEG = 45   # kąt nachylenia lasera (należący do trójkąta z punktem skanowanym)
laserDIS = 100  # odległość kamera-laser w mm
k = 250         # k - odległość kamera-środek "tacki" w mm
camH = 50       # wysokość na której znajduje się kamera w mm

"""w = 1000     # szerokość zdjęcia
h = 1000        # wysokość zdjęcia
f = 500         # f to ogniskowa w pixelach (odległość "matryca-obiektyw")
laserDEG = 45   # kąt nachylenia lasera (należący do trójkąta z punktem skanowanym)
laserDIS = 100  # odległość kamera-laser w mm
k = 250         # k - odległość kamera-środek "tacki" w mm
camH = 50       # wysokość na której znajduje się kamera w mm"""
output = open("msm.txt", "w")
i = 0
extracted = cd.extract(filename, folder, 0)
for point in extracted:
    print("extracted points: ", point, "iteration: ", i)
    r = laserDIS / (math.tan(math.radians(laserDEG)) + (point[0] / f))
    a = point[0] * r / f
    H = r * point[1] / f + camH
    print(r, "\t", a, "\t", H)
    msg = "point: " + str(point) + "\t" + str(r) + "\t" + str(a) + "\t" + str(H) + "\n"
    output.write(msg)
    i += 1
output.close()
