# -*- coding: utf-8 -*-
"""
Created on Sun Apr 29 07:33:24 2018

@author: DN
"""

import numpy as np
from skimage.io import imsave

resX = 640
resY = 480

ramka_1 = np.zeros((resX, resY, 3), dtype=np.uint8)
ramka_2 = np.zeros((resX, resY, 3), dtype=np.uint8)
ramka_3 = np.zeros((resX, resY, 3), dtype=np.uint8)

kolor_lini = 255
kanal_R = int(0)
kanal_G = int(1)
kanal_B = int(2)

kamera_1 = 1
kamera_2 = 2
kamera_3 = 3

for kat in range(182):
    ramka_1[:, int(ramka_1.shape[1] / 2 - 3):int(ramka_1.shape[1] / 2 + 3), kanal_R] = kolor_lini
    ramka_1[:, int(ramka_1.shape[1] / 2), kanal_G] = 0
    ramka_1[:, int(ramka_1.shape[1] / 2), kanal_B] = 0

    ramka_2[:, int(ramka_1.shape[1] / 2 - 3):int(ramka_1.shape[1] / 2 + 3), kanal_R] = kolor_lini
    ramka_2[:, int(ramka_1.shape[1] / 2), kanal_G] = 0
    ramka_2[:, int(ramka_1.shape[1] / 2), kanal_B] = 0

    ramka_3[:, int(ramka_1.shape[1] / 2 - 3):int(ramka_1.shape[1] / 2 + 3), kanal_R] = kolor_lini
    ramka_3[:, int(ramka_1.shape[1] / 2), kanal_G] = 0
    ramka_3[:, int(ramka_1.shape[1] / 2), kanal_B] = 0

    nazwa_pliku_1 = "%1i_%03i.png" % (kamera_1, kat)
    nazwa_pliku_2 = "%1i_%03i.png" % (kamera_2, kat)
    nazwa_pliku_3 = "%1i_%03i.png" % (kamera_3, kat)

    imsave(nazwa_pliku_1, ramka_1)
    imsave(nazwa_pliku_2, ramka_2)
    imsave(nazwa_pliku_3, ramka_3)
