#! /usr/bin/python
# -*- coding: utf-8 -*-
import fileinput as fi
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import math as maths #localisation
import sys
import argparse


parser = argparse.ArgumentParser(description='Cut data in two dimensions');

parser.add_argument('-i',help='input file(s) (defaults to stdin)',type=argparse.FileType('r'),default=[sys.stdin],nargs='*')

args = parser.parse_args()

def ovalMod(coord,xMinMax,yMinMax):
    x, y = coord
    xc = maths.pow( (2*x - (xMinMax[0] + xMinMax[1]))/(xMinMax[1] - xMinMax[0]), 2)
    yc = maths.pow( (2*y - (yMinMax[0] + yMinMax[1]))/(yMinMax[1] - yMinMax[0]), 2)
    return maths.sqrt(xc + yc) 

def inTriangle(coords):
    v = [(0,0.49235),(0.2204,0.5056),(0.2047,0.4857)]
    grads = [(v[1][1] - v[0][1])/(v[1][0] - v[0][0]),
             (v[2][1] - v[0][1])/(v[2][0] - v[0][0]),
             (v[1][1] - v[2][1])/(v[1][0] - v[2][0])]
    x,y = coords
    if y < v[0][1]+(x-v[0][0])*grads[0]:
        if y > v[0][1]+(x-v[0][0])*grads[1]:
            if y > v[2][1] + (x-v[2][0])*grads[2]:
                return True
    return False


def cut(pi0Mass,kMass,missingMom,EPRatio,missingMass2,muonFlag,pi0TransMom):
    if (missingMass2 < 43) and (0.8 < EPRatio) :
        return 'e'
    elif (inTriangle( (pi0TransMom,kMass) ) and not muonFlag) or (0.02 < EPRatio < 0.9):
        return 'p'
    else:
        return 'm'
    

def convertData(data):
    d = {'e':0,'m':0,'p':0}
    for thing in data:
        try:
            strings = thing.split()
            pi0Mass = float(strings[0])
            kMass = float(strings[1])
            missingMom = float(strings[2])
            EPRatio = float(strings[3])
            missingMass2 = float(strings[4])
            muonFlag =  (strings[5][0] == '1')

            pi0TransMom = float(strings[6])
            floats = [pi0Mass,kMass,missingMom,EPRatio,missingMass2,pi0TransMom]
            for f in floats:
                if maths.isnan(f):
                    raise ValueError
            d[cut(pi0Mass,kMass,missingMom,EPRatio,missingMass2,muonFlag,pi0TransMom)] += 1
        except ValueError:
            continue

    total = d['e'] + d['m'] + d['p']
    return d, total


for inFile in args.i:
    d, total = convertData(inFile.readlines()) 
    
    print "{0:20}\te:{1:10}\t{2:10}\tm:{3:10}\t{4:10}\tp:{5:10}\t{6:10}\tT:{7:10}".format(
inFile.name,
d['e'],
d['e']/float(total),
d['m'],
d['m']/float(total),
d['p'],
d['p']/float(total),
total)
