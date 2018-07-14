#!/usr/bin/python
# -*- coding: utf-8 -*-
"""Funkcje do obliczeń na danych punktach - pewnego rodzaju biblioteka."""
"""Wymagają modułu math!"""

import math
#import PNG_read
#from skimage.io import imread
import png
import time
import numpy as np

#wspólne stałe parametry kamery i skanera oraz zdjęcia:
w = 1024        # szerokość zdjęcia
h = 1280        # wysokość zdjęcia
f = 1110        # f to ogniskowa w pixelach (odległość "matryca-obiektyw")
laserDEG = 28.5 # kąt nachylenia lasera (należący do trójkąta z punktem skanowanym)
laserDIS = 100   # odległość kamera-laser w mm
k = 194         # k - odległość kamera-środek "tacki" w mm
camH = 73       # wysokość na której znajduje się kamera w mm
#odległość kamera-tacka oraaz camH jak na razie nie muszą być podane osobno


def convert(x, y, deg):
    """przygotowywuje punkty do obliczeń"""
    resX = w
    resY = h
    x -= (resX / 2)
    y = (resY / 2) - y
    return x, y, deg


def cartesian(H, r, alpha):
    x = r * math.cos(alpha)
    y = r * math.sin(alpha)
    return x, y, H


def calculate(x=0, y=0, deg=0):
    """używa układy współrzędnych o punkcie (0,0) na środku zdjęcia"""
    """do obliczeń na pomiarach kamer na bokach"""
    """teraz wywołuje też cartesian() na koniec"""
    # stałe parametry kamery:
    #k = 231.75 # k - odległość kamera-środek "tacki" w mm
    #k = 205
    #camH = 75  # wysokość na której znajduje się kamera w mm

    # obliczenia:
    r = laserDIS / (math.tan(math.radians(laserDEG)) + (x / f))
    a = x * r / f
    H = y * r / f + camH
    R = math.sqrt((k - r) ** 2 + a ** 2)
    if r == k:
        if x != 0:
            beta = 90
        elif x == 0:
            beta = 0
    else:
        beta = math.atan(a / (k - r))

    if x >= 0:
        Alpha = deg + beta
    elif x < 0:
        Alpha = deg - beta

    X, Y, Z = cartesian(H, R, Alpha)
    point = [X, Y, Z]
    return point


def calculateTOP(x=0, y=0, deg=0):
    """Do obliczeń na pomiarach z kamery na górze"""
    """zwraca o drazu punkt w układzie kartezjańskim!!!"""
    """w przeciwieństwie do calculateTOP - działa"""
    # stałe parametry kamery:
    k = 315
    # obliczenia:
    h1 = laserDIS / (math.tan(math.radians(laserDEG)) - (x / f))
    Z = k - h1
    a = x * h1 / f
    b = y * h1 / f
    sin = math.sin(deg)
    cos = math.cos(deg)
    X = a * cos + b * sin
    Y = -1 * a * sin + b * cos
    #kartezjański układ współrzędnych!!!
    point = [X, Y, Z]
    return X, Y, Z


def extract(filename, folder, stepDEGR=1):
    _time = time.time()
    pic = []
    points = []

    try:
        #PyPNG:
        reader = png.Reader(folder + filename + '.png')
        w, h, pixels, metadata = reader.read_flat()
        #np.array(pixels)
        # pixels = np.array(PNG_read.read(folder + filename + '.png'))
        #print('Reading time:', time.time()-_time)
        _time = time.time()
        filename = filename.rstrip(".png")
        cam_no = int(filename[0])
        deg = int(filename[2:]) * stepDEGR
        #deg = int(deg)
        if cam_no == 2:
            deg += 180
            if deg >= 360: deg -= 360
        deg = math.radians(deg)

        for i in range(len(pixels)):
            if (i % 3) == 0:
                pic.append(pixels[i+1]) #zmienić na i+1 !!!!! -

        for i in range(h):
            row = pic[i * w: ((i + 1) * w)]
            x = getpointConst(row)  # należy wybrać którą funkcję wykorzystać
            # x = getpointAvg(row)
            if x is None:
                continue
            y = i
            x, y, deg = convert(x, y, deg)
            data = [x, y, deg]
            points.append(data)
        #print('getpoint time:', time.time()-_time)

        return points
    except:
        return None


def getpointConst(row):
    """używa średniej arytmetycznej wszystkich pixeli które spełniają warunek"""
    RED = 140
    REDlist = []
    sequences = []

    for i in range(len(row)):
        if row[i] >= RED:
            REDlist.append(i)
    i = 0
    while i < len(REDlist):
        if (REDlist[i] + 1 or REDlist[i] + 2) in REDlist:
            condition = 1
            temp = []
            ii = i
            while condition == 1:
                temp.append(ii)
                ii += 1
                if (REDlist[ii] + 1 or REDlist[ii] + 2) in REDlist:
                    condition = 1
                else:
                    condition = 0
                    temp.append(ii)
            sequences.append(temp)
            i = i + len(temp)
        else:
            i += 1

    try:
        if sequences[0]:
            longest = max(sequences)
            a = 0
            for j in range(len(longest)):
                a += REDlist[longest[j]]
                x = a / len(longest)
        else:
            x = REDlist[0]
        return x
    except:
        if len(REDlist) is 0:
            #print('Something is wrong!!!')
            return None
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


def getpointConst2(row):
    """Ta nowa wolniejsza. W ramach optymalizacji zwiększono czas wykonywania o ok 3 sekundy"""
    """Nie przesestowana do końca - brak pewności, że działa"""
    """używa średniej arytmetycznej wszystkich pixeli które spełniają warunek"""
    RED = 128
    #REDlist = []
    sequences = []
    i = 0
    while i < len(row):
        if row[i] >= RED:
            red = True
            x = row[i]
            if (row[i] + 1 or row[i] + 2) >= RED:
                condition = 1
                temp = []
                ii = i
                while condition == 1:
                    temp.append(ii)
                    ii += 1
                    if (row[ii] + 1 or row[ii] + 2) >= RED:
                        condition = 1
                    else:
                        condition = 0
                        temp.append(ii)
                        sequences.append(temp)
                        i = i + len(temp)
            else:
                i += 1
        else: i += 1

    if red == False:
        #print('Something is wrong!!!')
        return None

    if sequences[0]:
        longest = max(sequences)
        a = 0
        for j in range(len(longest)):
            a += row[longest[j]]
            x = a / len(longest)

    return x


def calculateTOP_OLD(x=0, y=0, deg=0):
    """stara wersja, nie działa poprawnie. zostawiona just in case"""
    """Do obliczeń na pomiarach z kamery na górze"""
    # stałe parametry kamery:
    f = 577
    k = 250
    laserDEG = 30
    laserDIS = 30

    x, y, deg = convert(x, y, deg)
    # obliczenia:
    h1 = laserDIS / (math.tan(math.radians(laserDEG)) - (x / f))
    H = k - h1
    a = x * h1 / f
    r = y * h1 / f
    R = math.sqrt(a ** 2 + r ** 2)
    if r != 0:
        beta = math.degrees(math.atan(a / r))

    # ustalenie kąta (dużo przypadków)
    if y == 0:
        if x > 0:
            alpha = deg + 90
        if x < 0:
            alpha = deg - 90
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

    point = [H, R, alpha]
    return point
