import sys


if __name__ == '__main__':

    args = sys.argv
    if (len(args) != 3):
        exit(0)

    f1 = open(args[1], "rb")
    data_in = f1.read()
    f1.close()
    data_out = bytearray()

    for i in range(len(data_in)):
        if (i >= 256):
            data_out.append(data_in[i])

    f2 = open(args[2], "wb")
    f2.write(data_out)
    f2.close()

    exit(0)


