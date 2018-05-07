import png
for i in range(10000):
    reader = png.Reader("try.png")
    w, h, pixels, metadata = reader.read_flat()

print('PyPNG')
#print(pixels)
