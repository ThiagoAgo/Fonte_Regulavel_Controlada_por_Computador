#! /usr/bin/env python
#  -*- coding: utf-8 -*-
#
# Support module generated by PAGE version 5.1
#  in conjunction with Tcl version 8.6
#    Apr 27, 2023 02:48:55 PM -03  platform: Windows NT

import sys

try:
    import Tkinter as tk
except ImportError:
    import tkinter as tk

try:
    import ttk
    py3 = False
except ImportError:
    import tkinter.ttk as ttk
    py3 = True

def set_Tk_var():
    global combobox
    combobox = tk.StringVar()
    global combobox1
    combobox1 = tk.StringVar()
    global msg
    msg = tk.StringVar()
    msg.set('')
    global ten
    ten = tk.DoubleVar()

def init(top, gui, *args, **kwargs):
    global w, top_level, root
    w = gui
    top_level = top
    root = top

def destroy_window():
    # Function which closes the window.
    global top_level
    top_level.destroy()
    top_level = None

if __name__ == '__main__':
    import regulador
    regulador.vp_start_gui()




