#! /usr/bin/python
# -*- coding: utf-8 -*-
import fileinput as fi
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import math as maths #localisation
import sys
import argparse
from matplotlib.pyplot import cm
from matplotlib.colors import LogNorm

parser = argparse.ArgumentParser(description='Plot data in hitograms');

parser.add_argument('-b',help='number of bins (defaults to 200)',type=int,default=200)
parser.add_argument('-i',help='input file(s) (defaults to stdin)',type=argparse.FileType('r'),default=[sys.stdin],nargs='*')
parser.add_argument('-lx',help='lower x limit',type=float,default=-sys.float_info.max)
parser.add_argument('-ux',help='upper x limit',type=float,default=sys.float_info.max)

parser.add_argument('-ly',help='lower y limit',type=float,default=-sys.float_info.max)
parser.add_argument('-uy',help='upper y limit',type=float,default=sys.float_info.max)
parser.add_argument('-xL','--x-label',help='x axis label',type=str,default='')
parser.add_argument('-yL','--y-label',help='x axis label',type=str,default='')
parser.add_argument('-x',help='Indicates which parameter to use for the x axis (default is 0 (first))',type=int,default=0)
parser.add_argument('-y',help='Indicates which parameter to use for the y axis (default is 1 (second))',type=int,default=1)
args = parser.parse_args()
#print args
matplotlib.rcParams['text.latex.unicode']=True #for greek letters

def convertData(data,xI,yI,xLowerLimit,xUpperLimit,yLowerLimit,yUpperLimit):
    xl,yl = [],[]
    for thing in data:
        try:
            strings = thing.split()
            x = float(strings[xI])
            y = float(strings[yI])
            if maths.isnan(x) and maths.isnan(y):
                raise ValueError
            if (xLowerLimit < x < xUpperLimit) and (yLowerLimit < y < yUpperLimit):
                xl.append(x)
                yl.append(y)
        except ValueError:
            #print "a ValueError exeption has happened FUCKING PANIC!!!!!"
            continue
    return xl, yl

for inFile in args.i:
    convListX,convListY = convertData(inFile.readlines(),args.x,args.y,args.lx,args.ux,args.ly,args.uy) 
    
    freqMap,xEdges,yEdges = np.histogram2d(convListX,convListY,bins=args.b)
    freqMap += 1 #this is massively dodgy but otherwise we do log(0) and fuck shit up

    freqMapS = freqMap.swapaxes(0,1)
    extent = [xEdges[0], xEdges[-1], yEdges[0], yEdges[-1]]
    plt.figure()
    plt.clf()
    plt.imshow(freqMapS,cmap=cm.jet,aspect='auto',norm=LogNorm(),extent=extent,origin='lower',interpolation='nearest')

    plt.xlabel(args.x_label)
    plt.ylabel(args.y_label)
    plt.colorbar()
    plt.title(inFile.name.decode('utf-8'))
    #plt.title("allMC-LkrData-event-Displacement")
plt.show()
