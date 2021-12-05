"""
Graph from ESP32 Serial link
Run photoresitor sketch in same time !

Author : G.MENEZ

"""
import sys, serial
import numpy as np
from time import sleep
from collections import deque
from matplotlib import pyplot as plt
import json

#=============================================

class AnalogData:
    
    def __init__(self, maxLen=100):
        """ Ring Buffer de maxLen samples """        
        self.x = []
        self.y = []
        self.maxLen = maxLen

    def add(self, x, y):
        """ Add a sample in the  ring buffer """
        if len(self.x) == self.maxLen:
            self.x.pop(0)
            self.y.pop(0)
                        
        self.x.append(x)
        self.y.append(y)
    
#=============================================

class AnalogPlot:
    """ Plotting Figure """
    def __init__(self, analogData):
        plt.ion()                 # set plot to "animated"
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(111)
        self.axline, = self.ax.plot(analogData.x, analogData.y,
                                    'r.-', label="ttyUSB0")

        plt.xlabel('Json Payload #')
        plt.ylabel('Luminosity')
        #plt.xlim([0, 100])
        #plt.ylim([0, 4000])
        self.decoration("Analog Signal")
        
    def updateplot(self, analogData):
        """  update plot """
        self.axline.set_xdata(analogData.x)
        self.axline.set_ydata(analogData.y)

        self.fig.canvas.draw()
        self.fig.canvas.flush_events()
        #        self.ax.autoscale()
        self.ax.relim()            # reset intern limits of the current axes
        self.ax.autoscale_view()   # reset axes limits 
    
    def decoration(self, title):
        plt.title(title)
        plt.grid(True)
        plt.legend()
        
#----------------  Main function -----------------
def main():
    # Ring buffer of last samples 
    analogData = AnalogData()
    # plotting canvas
    analogPlot = AnalogPlot(analogData)    
    
    # open serial port
    ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )
    
    counter=0
    while True:
        try:
            # Read 
            v = ser.readline()
            #v = str(v)
            v = v.rstrip()
            v = v.decode("utf-8")
            if v != "" :
                print(v)
                v = json.loads(v)
                print ("Valeur : {}".format(v))

                # Plot
                analogData.add(counter, v["luminosite"])
                analogPlot.updateplot(analogData)
                print('plotting new data...')
                
        except KeyboardInterrupt:
            print('exiting')
            # close serial
            #ser.flush()
            ser.close()
            break
        
        counter += 1
    
    # close serial
    ser.flush()
    ser.close()
    
#------------------ call main -----------------------

if __name__ == '__main__':
    main()
