import calculate as c
import os
import time
import multiprocessing as mp


def init():
    """Saves to array 'files' names of all photos in images subdirectory"""
    files = []
    for file in os.listdir('images/'):
        if file.endswith('.png') and not file.startswith('3'):
            files.append(file)
        # break     # Gets only first file, comment if you want more

    return files


def calc_everything(filename):
    """"Designed to be run in other tread. Calculates everything and saves as previous filename with .txt
        extension in output folder"""
    filename = filename.rstrip('.png')
    extracted = c.extract(filename)
    points = []
    for point in extracted:
        points.append(c.convert(point[0], point[1], point[2]))

    output = []
    for point in points:
        output.append(c.calculate(point[0], point[1], point[2]))

    with open('Outputs/' + filename + '.txt', 'w') as f:
        for point in output:
            f.write('{} ; {} ; {} \n'.format(point[0], point[1], point[2]))
    print('DONE!!!')


def main():
    """Runs inti() and manages threads"""
    _time = time.time()
    files = init()
    files.sort()
    files.reverse()

    print('Starting!!!')
    workers = mp.Pool(processes=8)  # Number of process it creates
    workers.map(calc_everything, files)

    print('Done! Time:', time.time()-_time)
    with open('OUTPUT.txt', 'a') as f:
        for file in os.listdir('Outputs/'):
            with open('Outputs/' + file, 'r') as f2:
                f.write(f2.read())


if __name__ == '__main__':
    main()
