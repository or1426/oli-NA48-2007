#! /usr/bin/python
# -*- coding: utf-8 -*-
import fileinput as fi
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import math as maths #localisation
import sys
import argparse


parser = argparse.ArgumentParser(description='Cut data in one dimension');

parser.add_argument('-i',help='input file(s) (defaults to stdin)',type=argparse.FileType('r'),default=[sys.stdin],nargs='*')

args = parser.parse_args()


def cutData(data,cut):
    greaterThan = 0
    lessThan = 0
    total = len(data)
    for number in data:
        if number > cut:
            greaterThan += 1
        else:
            lessThan += 1
    return lessThan,greaterThan, total

def pionCut(data):
    x, y = data[2], data[1]
    xMinMax = (-3.35,3.35)
    yMinMax = (0.485,0.503)
    xc = maths.pow( (2*x - (xMinMax[0] + xMinMax[1]))/(xMinMax[1] - xMinMax[0]), 2)
    yc = maths.pow( (2*y - (yMinMax[0] + yMinMax[1]))/(yMinMax[1] - yMinMax[0]), 2)
    s = maths.sqrt(xc + yc)
    if s < 1 or (s < 15 and s > 0.54):
        return False
    else:
        return True

def convertData(data):
    r = []
    for thing in data:
        try:
            strings = thing.split()
            floats = [float(thing) for thing in strings if ~maths.isnan(float(thing))]
            if len(floats) == 6: #everything turned out to be ~Nan
                r.append(floats[3])
        except ValueError:
            continue
    return r

dl = []
for inFile in args.i:
    dl.append( (inFile.name,convertData(inFile.readlines())) )
    
while True:
    cuts = []
    f = raw_input('\t> ') 
    while f != 'go':
        try:
            cuts.append(float(f))
            print '\t< ',f
        except ValueError:
            print '\t< I cant let you do that Dave'
        f = raw_input('\t> ') 
    
    for cut in cuts:
        print '\t< cut position: {0}'.format(cut)
        for section in dl:
            nLess,nGreater, nTotal = cutData(section[1],cut) 
            print '\t\t{0:30}\tL:{1:10}\t{2:10}\tG:{3:10}\t{4:10}\tT:{5:10}'.format(section[0], nLess,nLess/float(nTotal), nGreater,nGreater/float(nTotal), nTotal)
    
