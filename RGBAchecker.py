import png
filename = "images/1_000.png"
reader = png.Reader(filename)
w, h, pixels, metadata = reader.read_flat()
print(metadata)
