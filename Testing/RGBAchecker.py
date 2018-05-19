import png
filename = "C:\Python\programs and projects\CD\Whatever\images_new/1_0.png"
reader = png.Reader(filename)
w, h, pixels, metadata = reader.read_flat()
print(metadata)
