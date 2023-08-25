#!/usr/bin/python3


import tkinter as tk
import threading
import time
import math
import sys

from PIL import Image, ImageTk
from PIL import ImageDraw

vr = [0,  36,  73, 109, 146, 182, 219, 255]
vg = [0,  36,  73, 109, 146, 182, 219, 255]
vb = [0,  85, 170, 255]

args = sys.argv

ratio = 2

class Glaphplot01(threading.Thread):

    def __init__(self, num01, canvas01, daemon):
        super(Glaphplot01, self).__init__()
        super(Glaphplot01, self).setDaemon(daemon)
        self.num01 = num01
        self.thread_end_flag = 0
        self.color01 = ['black', 'red', 'blue', 'green', 'cyan', 'magenta', 'yellow', 'white']
        self.canvas01 = canvas01
        self.count01 = 0
        self.image = Image.new('RGB', (256*ratio, 212*ratio), (0, 0, 0))
        self.draw = ImageDraw.Draw(self.image)

    def run(self):
        self.canvas01.executeflag = 1
        self.glaphplot_exec()
        self.canvas01.executeflag = 0
        self.thread_end_flag = 1

    def getThreadEndFlag(self):
        return self.thread_end_flag

    def glaphplot_exec(self):
        while(self.canvas01.stopflag01 == 0):
          while (self.count01 == 0):

            self.count01 += 1
            f = open(args[1], "rb")
            byte01 = f.read()

            xdot =256
            ydot =212


            self.canvas01.fill(0, 0, 0);
            sum_col01 = 0
            j = 0
            index = 0

            while j < ydot: 

                i = 0
                while i < xdot:
                    colr = byte01[index * 3 + 0]
                    colg = byte01[index * 3 + 1]
                    colb = byte01[index * 3 + 2]
                    index += 1
                    self.draw.rectangle((i*ratio,j*ratio,i*ratio+1,j*ratio+1), outline=(colr, colg, colb), fill=(colr, colg, colb))

                    i = i + 1
                j = j + 1
            self.photo_image = ImageTk.PhotoImage(self.image)
            self.canvas01.drawImage(256*ratio / 2, 212*ratio / 2, self.photo_image)


class Frame(tk.Frame):

    def __init__(self, master=None):
        self.width = 256*ratio
        self.height = 232*ratio
        self.bitmap_width = 256*ratio
        self.bitmap_height = 212*ratio
        tk.Frame.__init__(self, master, height=self.height, width=self.width , bg='white')
        self.master.title('draw msx screen 8')
        self.canvas01 = GraphplotCanvas(self, width = self.bitmap_width, height = self.bitmap_height, frame01 = self)
        self.canvas01.pack()

        button_frame01 = tk.Frame(self, width=self.width - 20, height=15, bg='white')
        button_frame01.pack()
        button02 = tk.Button(button_frame01, text='exit', command= lambda : self.on_pushed_exit(button02))
        button02.pack(side=tk.LEFT, padx=(5, 0))

        self.canvas01.start_glaphplot()

    def on_pushed_exit(self, button):
        self.canvas01.stop()
        self.canvas01.delete(all)
        self.destroy()
        self.master.destroy()
        return

class GraphplotCanvas(tk.Canvas):
    def __init__(self, master=None, **kwargs):
        self.width = kwargs.get('width', 256*ratio)
        self.height = kwargs.get('height', 212*ratio)
        self.canvas = tk.Canvas.__init__(self, master, relief=tk.RAISED, bd=4, bg='white', width=self.width, height=self.height)
        self.executeflag = 0
        self.graphplot01 = None
        self.main_thread = threading.currentThread()
        self.frame01 = kwargs.get('frame01', None)
        self.stopflag01 = 0

    def ractangle(self, x0, y0, x1, y1, colr, colg, colb):
        self.create_rectangle(x0, y0, x1, y1, fill='#%02x%02x%02x' % (colr, colg, colb), width=0)

    def line(self, x0, y0, x1, y1, colr, colg, colb):
        self.create_line(x0, y0, x1, y1, fill='#%02x%02x%02x' % (colr, colg, colb))

    def fill(self, colr, colg, colb):
        self.create_rectangle(0, 0, self.width, self.height, fill='#%02x%02x%02x' % (colr, colg, colb), width=0)

    def start_glaphplot(self):
        self.graphplot01 = Glaphplot01(10, self, daemon=True)
        self.graphplot01.start()

    def stop(self):
        if self.stopflag01 != None :
            self.stopflag01 = 1

    def drawImage(self, x0, y0, image01):
        self.create_image(x0, y0, image=image01)

def on_closing():
    f.canvas01.stop()
    if f.canvas01.graphplot01 != None:
        f.canvas01.graphplot01.join()
    f.canvas01.delete(all)
    f.destroy()
    root.destroy()
    return

##----------------
if __name__ == '__main__':
    root = tk.Tk()
    main_thread = threading.currentThread()
    root.protocol('WM_DELETE_WINDOW', on_closing)
    f = Frame(master = root)
    f.pack()
    f.mainloop()
