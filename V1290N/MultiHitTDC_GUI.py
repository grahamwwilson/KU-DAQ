#!/usr/bin/env python

#$Id: Rates_GUI.py,v 1.19 2008/08/29 22:30:11 graham Exp $

# This example shows how to add functionality to the buttons and also allows
# to set a default button.

# 'setdefault' method is used to set the default button to the button given
# by the argument.this causes the specified button to be displayed with the
# platform specific appearance for a default button.
  
# 'alignbuttons' is used to set the widths of all the buttons to be the same
# as the width of the widest button.  

# 'invoke' method is used to invoke the call back command associated with
# the button specified by the argument.

#
# I have extended this initial example quite a bit with an emphasis on 
# getting started with something that potentially does something useful
# and exploring ways to do useful stuff rather 
# than much concern about aesthetics.
#
# DONE:
#        0) Install python on heplx4, test and incorporate with actual V560 program
#        i) Query environment variables like $HOME and $USER
#
# STILL TO DO
#       ii) Aborting threaded process ?
#      iii) Reading and displaying current rates ?
#       iv) Add text-entry field for run conditions ? (added but not yet used ...)
#
#                           Graham W. Wilson      June 12th 2008
#                               updated           June 19th 2008
#                                hi               June 20th 2008
#
# Now have separate output directories based on actual date working
# Would be nice to add a BLT plot with the rate history with errors.
#  

title = 'V1290N MultiHitTDC GUI'

# Import Pmw from this directory tree.
import sys
import os
#sys.path[:0] = ['../../..']

#import Tkinter
from Tkinter import *
#import tkMessageBox, tkFileDialog, tkColorChooser
import string
import Pmw
import commands
import threading
import time
import datetime
from subprocess import Popen, PIPE        

class Demo:
    def __init__(self, parent):
        self.master = parent

# Here we first set up the parameters

	# Create and pack the ButtonBox.
	self.buttonBox = Pmw.ButtonBox(parent,
		labelpos = 'nw',
                orient = 'vertical',                             
		label_text = 'Controls:',
                frame_borderwidth = 2,
                frame_relief = 'groove')
	self.buttonBox.pack(side= 'right', expand = 1, padx = 30, pady = 10)

# Entry fields for run name, cyclemax, freq, daqfreq, comments
# Currently only the latter 3 affect program execution

        self.case = IntVar()  # tie a variable to an entry
        self.case.set('1')
        self.case_widget = Pmw.EntryField(parent,
                        labelpos='w',
                        label_text='Run Number',
                        entry_width=10,
                        entry_textvariable=self.case,
                        command=self.status_entries)

        self.cyclemax = IntVar(); self.cyclemax.set(1000000)
        self.cyclemax_widget = Pmw.EntryField(parent,
                        labelpos='w',  # n, nw, ne, e, and so on
                        label_text='Number of events to collect (-1 = no limit)',
                        validate={'validator': 'integer', 'min': -1},
                        entry_width=10,
                        entry_textvariable=self.cyclemax,
                        command=self.status_entries)

        self.freq = IntVar(); self.freq.set(0)
        self.freq_widget = Pmw.EntryField(parent,
                        labelpos='w',  # n, nw, ne, e, and so on
                        label_text='Pulser frequency (Hz)',
                        validate={'validator': 'integer', 'min': 0},
                        entry_width=10,
                        entry_textvariable=self.freq,
                        command=self.status_entries)
                        
        self.daqfreq = IntVar(); self.daqfreq.set(20000)
        self.daqfreq_widget = Pmw.EntryField(parent,
                        labelpos='w',  # n, nw, ne, e, and so on
                        label_text='Daq frequency (Hz)',
                        validate={'validator': 'integer', 'min': 1},
                        entry_width=10,
                        entry_textvariable=self.daqfreq,
                        command=self.status_entries)                
        
        self.comments = StringVar(); self.comments.set('"                      "')
        self.comments_widget = Pmw.EntryField(parent,
                        labelpos='w',  # n, nw, ne, e, and so on
                        label_text='Logbook Comments:',
                        validate = None,
                        entry_width=30,
                        entry_textvariable=self.comments,
                        command=self.status_entries)

        widgets = (self.case_widget, self.cyclemax_widget, self.freq_widget, self.daqfreq_widget, self.comments_widget)
        for w in widgets:
            w.pack(side='top',padx=20,pady=12, anchor='w')
        Pmw.alignlabels(widgets)
               

	self.buttonBox.add('START Data-Taking Run', command = self.altstart2)
        self.buttonBox.add('Display Output File', command = self.display_out)
        self.buttonBox.add('Show Elapsed Time', command = self.show_elapsedtime)
	self.buttonBox.add('END Data-Taking Run', command = self.stop)        

	# Make all the buttons the same width.
	self.buttonBox.alignbuttons()
        
    def show_date(self):
        print 'Today is', self.datestring
        print 'The time is', self.timestring        

    def show_elapsedtime(self):
# Suspect this only has about 0.01s precision so should format it appropriately        
        self.elapsedtime = time.time() - self.t0
        print 'Elapsed time (s) = ', self.elapsedtime

    def status_entries(self):
        """Read values from entry widgets or variables tied to them."""
        s = "entry fields: '" + self.case.get() + \
            "', " + str(self.cyclemax.get()) + \
            ", " + str(self.freq.get())
        self.status_line.configure(text=s)        

    def _processReturnKey(self, event):
	self.buttonBox.invoke()

    def start(self):
#	print 'You clicked on Start'
# For now use the idiom of HPL p54
#
        cmd = './test1.exe'
        failure, output = commands.getstatusoutput(cmd)
        if failure:
            print 'Failed to run test1.exe program\n%s\n%s' % \
                  (cmd, output); sys.exit(1)
        else:
            print 'Program started successfully'

    def stop(self):
	print 'Stop'
        self.then = datetime.datetime.now()
        print 'Today is',self.then.strftime("%d-%B-%Y")
        print 'The time is', self.then.strftime("%T")
        print 'Will kill the V1290N-Read process'
        print ' - but first check elapsed time and observed rates '
        self.show_elapsedtime()        
        self.status_log()
        cmd = './V1290N-Read_kill.sh '
        print 'Command to be executed is: ', cmd
        print 'Killing your V1290N-Read process now'
# Use thread to run system command in parallel with the script (p77)        
        tkill = threading.Thread(target=commands.getstatusoutput, args=(cmd,))
        tkill.start()
        print ' '
        print ' You may start a new run with a NEW run number from within the GUI now if you like'
        print ' '

    def altstart2(self):
	print 'Start'
        self.now = datetime.datetime.now()
#        print 'Today is', self.today
        self.datestring = self.now.strftime("%d-%B-%Y")
        self.timestring = self.now.strftime("%T")
        print 'Today is', self.datestring
        print 'The time is', self.timestring
        
        self.home = os.environ.get('HOME')
        self.mypath = str(self.home) + "/My_Data_Taking/MultiHitTDC_DataTaking/" + str(self.datestring) + "/Run" + str(self.case.get())
        print 'mypath =', self.mypath 
        
# playing with methods in HPL p 77

        cmd = './V1290N-Read.sh '
        opts = " " + str(self.case.get()) + " " + str(self.cyclemax.get()) + " " + str(self.freq.get()) + " " + str(self.daqfreq.get())
        cmd += opts                                # Append the command line options
# Note that the shell script is the place where the
# parameters which are seen by the executable are seen.
        cmd += " " + str(self.datestring)
        cmd += " " + str(self.timestring)
        cmd += " " + (self.comments.get())

        print 'Command to be executed is: ', cmd

        self.t0 = time.time()

        print 'Program starting at time: ', self.t0
# Use thread to run system command in parallel with the script (p77)        
        t = threading.Thread(target=commands.getstatusoutput, args=(cmd,))
        t.start()

    def show_user(self):
        print 'Show User Name'
        self.user = os.environ.get('USER')
        print 'Hello', self.user

    def show_origdir(self):
        print 'Show Original Directory'
        self.origdir = os.getcwd()
        print 'Original Directory', self.origdir       
        
    def show_stdout(self):
# only defined once program is started        
	print 'Display stdout'
#        print self.stdout
#        print V560N-Rates.out
#        self.display_file(str(self.mypath)+"/V560N-Rates.out",self.master)

    def show_stderr(self):
# only defined once program is started        
	print 'Show stderr'        
        print self.stderr

    def display_pars(self):
        print 'Display Parameters File'
        self.display_file(str(self.mypath)+"/test.pars",self.master)

    def display_log(self):
#        print 'Display Log File'
        self.display_file(str(self.mypath)+"/test.log",self.master)
        
    def display_out(self):
        print 'Display Output File'
        self.display_file(str(self.mypath)+"/V1290N-Read.out",self.master)
        
    def status_log(self):
# Display the last 11 lines of the log file corresponding to the most recent cycle        
        self.display_tailfile(str(self.mypath)+"/test.log",11,self.master)

    def show_parameters(self):
	print 'Show Parameters'
        ifile = open(str(self.mypath)+"/test.pars",'r')
        for line in ifile:
            print line
        ifile.close()

    def display_file(self, filename, parent):
        """Read file into a text widget in a _separate_ window."""
        filewindow = Toplevel(parent) # new window

        f = open(filename, 'r');  filestr = f.read();  f.close()
        # determine the number of lines and the max linewidth:
        lines = filestr.split('\n')
        nlines = len(lines)
        maxwidth = max(map(lambda line: len(line), lines))

        filetext = Pmw.ScrolledText(filewindow,
             borderframe=20, # a bit space around the text
             vscrollmode='dynamic', hscrollmode='dynamic',
             labelpos='n', label_text='Contents of file '+filename,
             text_width=min(80,maxwidth+10),
             text_height=min(50,nlines),
             text_wrap='none',  # do not break lines
             )
        filetext.pack(expand=True, fill='both')

        filetext.insert('end', filestr)

        # add a quit button:
        Button(filewindow, text='Quit',
               command=filewindow.destroy).pack(pady=10)

        # force the new window to be in focus:
        filewindow.focus_set()

    def display_tailfile(self, filename, ntodisplay, parent):
        """Read file into a text widget in a _separate_ window and display
        only the last ntodisplay lines"""
        filewindow = Toplevel(parent) # new window

        f = open(filename, 'r'); filestr = f.read(); f.seek(0); filestrlist = f.readlines(); f.close()
        # determine the number of lines and the max linewidth:
        lines = filestr.split('\n')
        nlines = len(lines)
        maxwidth = max(map(lambda line: len(line), lines))

        filetext = Pmw.ScrolledText(filewindow,
             borderframe=20, # a bit space around the text
             vscrollmode='dynamic', hscrollmode='dynamic',
             labelpos='n', label_text='Tail of file '+filename,
             text_width=min(90,maxwidth+10),
             text_height=min(50,ntodisplay),
             text_wrap='none',  # do not break lines
             )
        filetext.pack(expand=True, fill='both')

        for i in range(nlines-ntodisplay, nlines-1, 1):
            filetext.insert('end', filestrlist[i])

        # add a quit button:
        Button(filewindow, text='Quit',
               command=filewindow.destroy).pack(pady=10)

        # force the new window to be in focus:
        filewindow.focus_set()


    def textentry(self, parent, variable, label):
        """Make a textentry field tied to variable."""
        # pack a label and entry horizontally in a frame:
        l = Label(parent, text=label)
        l.grid(column=0, row=self.row_counter, sticky='w')
        mywidget = Entry(parent, textvariable=variable, width=8)
        mywidget.grid(column=1, row=self.row_counter)
        self.row_counter += 1
        return mywidget        

# Create demo in root window for testing.
if __name__ == '__main__':
    root = Tk()
    Pmw.initialise(root)
    root.title(title)

    exitButton = Button(root, text = 'Exit', command =root.destroy)
    exitButton.pack(side = 'bottom')
    widget = Demo(root)
    root.mainloop()
