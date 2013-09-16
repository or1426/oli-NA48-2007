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

args = parser.parse_args()
#print args
matplotlib.rcParams['text.latex.unicode']=True #for greek letters
def ovalMod(coord,xMinMax,yMinMax):
    x, y = coord
    xc = maths.pow( (2*x - (xMinMax[0] + xMinMax[1]))/(xMinMax[1] - xMinMax[0]), 2)
    yc = maths.pow( (2*y - (yMinMax[0] + yMinMax[1]))/(yMinMax[1] - yMinMax[0]), 2)
    return maths.sqrt(xc + yc)

def convertData(data,xLowerLimit,xUpperLimit,yLowerLimit,yUpperLimit):
    r1,r2 = [],[]
    for thing in data:
        try:
            s1, s2,s3,s4,s5,s6 = thing.split()
            f1, f2 = float(s5),float(s2)
            coord = f1,f2

            if ~maths.isnan(f1) and ~maths.isnan(f2):
               if (xLowerLimit < f1 < xUpperLimit) and (yLowerLimit < f2 < yUpperLimit):
                   #xMinMax = (-4.89,4.89)
                   #yMinMax = (0.469,0.521)
                   #if ovalMod(coord,xMinMax,yMinMax) > 1 and not(f1 < 15 and f2 > 0.54):
                   r1.append(f1)
                   r2.append(f2)
        except ValueError:
            #print "a ValueError exeption has happened FUCKING PANIC!!!!!"
            continue
    return r1, r2


for inFile in args.i:
    convListX,convListY = convertData(inFile.readlines(),args.lx,args.ux,args.ly,args.uy) 
    
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
