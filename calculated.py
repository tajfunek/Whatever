#!/usr/bin/python
# -*- coding: utf-8 -*-
import calculate as c
import time
import os
import multiprocessing as mp

IMAGES_FOLDER = 'images/'


def init():
    """Saves to array 'files' names of all photos in images subdirectory"""
    files = []
    for file in os.listdir(IMAGES_FOLDER):
        if file.endswith('.png'):
            files.append(file)
        # break     # Gets only first file, comment if you want more

    return files


def calc_everything(tup):
    """"Designed to be run in other tread. Calculates everything"""
    filename = tup[0]
    q = tup[1]
    filename = filename.rstrip('.png')
    _time = time.time()
    extracted = c.extract(filename, IMAGES_FOLDER)
    #print('Extract time: ', time.time()-_time, '  File:  ', filename)

    _time = time.time()
    output = []
    if filename.startswith('3'):
        #pass
        for point in extracted:
            output.append(c.calculateTOP(point[0], point[1], point[2]))
    else:
        for point in extracted:
            output.append(c.calculate(point[0], point[1], point[2]))
    #print('Calculate time: ', time.time()-_time)

    q.put(output)

    # print('DONE!!!')


def main():
    """Runs init() and manages threads"""
    _time = time.time()
    files = init()
    files.sort()
    m = mp.Manager()
    q = m.Queue()

    tup = list(zip(files, [q] * len(files)))
    print('Starting!!!')

    _time = time.time()
    workers = mp.Pool(processes=8)  # Number of process it creates
    workers.map(calc_everything, tup)
    #print('Allocate time: ', time.time()-_time)

    # Wait for first processes witch finish their jobs
    # time.sleep(3)

    file = open('OUTPUT.txt', 'w')
    i = 0
    while i < len(files):
        if not q.empty():
            out = q.get()
            for line in out:
                file.write('{} ; {} ; {}\r\n'.format(line[0], line[1], line[2]))
            i += 1

    file.flush()
    file.close()
    workers.close()

    print('Done! Time:', time.time() - _time)

    """with open('OUTPUT.txt', 'a') as f:
        for file in os.listdir('Outputs/'):
            with open('Outputs/' + file, 'r') as f2:
                f.write(f2.read())"""


if __name__ == '__main__':
    main()
