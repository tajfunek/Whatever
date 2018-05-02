import calculate as c
import os
import time
import multiprocessing as mp

IMAGES_FOLDER = 'images/'


def init():
    """Saves to array 'files' names of all photos in images subdirectory"""
    files = []
    for file in os.listdir(IMAGES_FOLDER):
        if file.endswith('.png') and not file.startswith('3'):
            files.append(file)
        # break     # Gets only first file, comment if you want more

    return files


def calc_everything(tup):
    """"Designed to be run in other tread. Calculates everything"""
    filename = tup[0]
    q = tup[1]
    filename = filename.rstrip('.png')
    extracted = c.extract(filename, IMAGES_FOLDER)
    points = []
    for point in extracted:
        points.append(c.convert(point[0], point[1], point[2]))

    output = []
    for point in points:
        output.append(c.calculate(point[0], point[1], point[2]))

    q.put(output)

    # print('DONE!!!')


def main():
    """Runs init() and manages threads"""
    _time = time.time()
    files = init()
    files.sort()
    files.reverse()
    m = mp.Manager()
    q = m.Queue()

    tup = list(zip(files, [q] * len(files)))
    print('Starting!!!')
    workers = mp.Pool(processes=8)  # Number of process it creates
    workers.map(calc_everything, tup)

    # Wait for first processes witch finish their jobs
    # time.sleep(3)

    print('Processing queue')
    file = open('OUTPUT.txt', 'w')
    i = 0
    while i < len(files):
        if not q.empty():
            print('Queue.')
            out = q.get()
            for line in out:
                file.write('{} ; {} ; {}\n'.format(line[0], out[1], out[2]))
            i += 1

    print('Closing')
    file.flush()
    file.close()
    workers.close()
    print('Closed')

    print('Done! Time:', time.time() - _time)

    """with open('OUTPUT.txt', 'a') as f:
        for file in os.listdir('Outputs/'):
            with open('Outputs/' + file, 'r') as f2:
                f.write(f2.read())"""


if __name__ == '__main__':
    main()
