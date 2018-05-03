"""Funkcje do obliczeń na danych punktach - pewnego rodzaju biblioteka."""
"""Wymagają modułu math!"""

import math
import png


def convert(x, y, deg=None):
    """przygotowywuje punkty do obliczeń"""
    resX = 480
    resY = 640
    x -= (resX / 2)
    y = (resY / 2) - y
    return x, y, deg


def calculate(x=0, y=0, deg=0):
    """używa układy współrzędnych o punkcie (0,0) na środku zdjęcia"""
    """do obliczeń na pomiarach kamer na bokach"""
    # stałe parametry kamery:
    x, y, deg = convert(x,y,deg)
    f = 577  # f to ogniskowa w pixelach (odległość "matryca-obiektyw")
    k = 173  # k - odległość kamera-środek "tacki" w mm
    laserDEG = 30  # kąt nachylenia lasera (należący do trójkąta z punktem skanowanym)
    laserDIS = 30  # odległość kamera-laser w mm
    camH = 100  # wysokość na której znajduje się kamera w mm

    # obliczenia:
    r = laserDIS / (math.tan(math.radians(laserDEG)) - (x / f))
    a = x * r / f
    H = y * r / f + camH
    R = math.sqrt((k - r) ** 2 + a ** 2)
    if r == k:
        if x != 0:
            beta = 90
        elif x == 0:
            beta = 0
    else:
        beta = math.degrees(math.atan(a / (k - r)))

    if x >= 0:
        Alpha = deg + beta
    elif x < 0:
        Alpha = deg - beta

    point = [round(H, 2), round(R, 2), round(Alpha, 2)]
    return point


def calculateTOP(x=0, y=0, deg=0):
    """Do obliczeń na pomiarach z kamery na górze"""
    # stałe parametry kamery:
    f = 577
    k = 250
    laserDEG = 30
    laserDIS = 30

    # obliczenia:
    h1 = laserDIS / (math.tan(math.radians(laserDEG)) - (x / f))
    h = k - h1
    a = x * h / f
    r = y * h / f
    R = math.sqrt(a ** 2 + r ** 2)
    if r != 0:
        beta = math.degrees(math.atan(a / r))

    # ustalenie kąta (dużo przypadków)
    if y == 0:
        if x > 0:
            alpha = deg - 90
        if x < 0:
            alpha = deg + 90
        elif x == 0:
            alpha = 0  # 0 ale to w sumie nie ma znaczenia, jest w puncie (0,0)
    else:
        if y > 0:
            if x >= 0:
                alpha = deg - beta
            if x < 0:
                alpha = deg + beta
        if y < 0:
            if x >= 0:
                alpha = deg + beta + 180
            if x < 0:
                alpha = deg - beta + 180

    if alpha < 0:
        alpha += 360

    point = [round(h, 2), round(R, 2), round(alpha, 2)]
    return point


def extract(filename, folder, stepDEGR=1):
    pic = []
    points = []
    reader = png.Reader(folder + filename + '.png')
    w, h, pixels, metadata = reader.read_flat()
    pixels = list(pixels)

    # This is done in other function
    # filename = filename[:-4]  # Leaves file without expansion
    # filename = filename.lstrip('images/')  # Gets rid of directory name
    cam_no = int(filename[0])
    deg = int(filename[2:]) * stepDEGR
    #deg = int(deg)
    if cam_no == 2:
        deg += 180
        if deg >= 360: deg -= 360

    for i in range(len(pixels)):
        if (i % 3) == 0:
            pic.append(pixels[i])

    for i in range(h):
        row = pic[i * w: ((i + 1) * w)]
        x = getpointConst(row)  # należy wybrać którą funkcję wykorzystać
        # x = getpointAvg(row)
        if x is None:
            continue
        y = i
        data = [x, y, deg]
        points.append(data)

    return points


def getpointConst(row):
    """używa średniej arytmetycznej wszystkich pixeli które spełniają warunek"""
    RED = 128
    REDlist = []
    sequences = []
    global no

    for i in range(len(row)):
        if row[i] >= RED:
            REDlist.append(i)
    i = 0
    while i < len(REDlist):
        if (REDlist[i] + 1) in REDlist:
            condition = 1
        elif (REDlist[i] + 2) in REDlist:
            condition = 1
        else:
            condition = 0
            i += 1
        temp = []
        ii = i
        while condition == 1:
            temp.append(ii)
            ii += 1
            if (REDlist[ii] + 1) in REDlist:
                condition = 1
            elif (REDlist[ii] + 2) in REDlist:
                condition = 1
            else:
                condition = 0
                temp.append(ii)
        sequences.append(temp)
        i = i + len(temp)
    if len(sequences) is 0:
        print('Something is wrong!!!')
        return None
    if sequences[0]:
        longest = max(sequences)
        a = 0
        for j in range(len(longest)):
            a += REDlist[longest[j]]
            x = a / len(longest)
    else:
        x = REDlist[0]
    return x


def getpointAvg(row):
    "używa średniej ważonej pixeli które spełniają warunek"
    "jest niedokończona względem getpointConst"
    RED = 128
    REDlist = []
    sequences = []
    for i in range(len(row)):
        if row[i] >= RED:
            REDlist.append(i)
    i = 0
    while i < len(REDlist):
        if (REDlist[i] + 1) in REDlist:
            condition = 1
        elif (REDlist[i] + 2) in REDlist:
            condition = 1
        else:
            condition = 0
            i += 1
        temp = []
        ii = i
        while condition == 1:
            temp.append(ii)
            ii += 1
            if (REDlist[ii] + 1) in REDlist:
                condition = 1
            elif (REDlist[ii] + 2) in REDlist:
                condition = 1
            else:
                condition = 0
                temp.append(ii)
        sequences.append(temp)
        i = i + len(temp)

    if sequences[0]:
        longest = max(sequences)
        a = 0
        b = 0
        for j in range(len(longest)):
            a += REDlist[longest[j]] * row[REDlist[longest[j]]]
            b += row[REDlist[longest[j]]]
            x = a / b
    else:
        x = REDlist[0]

    return x

    def cartesian(H, r, alpha):
        x = r * math.cos(radians(alpha))
        y = r * math.sin((radians(alpha)))
        return x, y, H
