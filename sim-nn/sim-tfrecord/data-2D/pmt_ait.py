#-*-coding=utf-8-*-
import numpy as np
import math
RadToDeg=180/math.pi
DegToRad=math.pi/180
def ProjectAitoff2xy(l,b):
    b=90-b
    if l>180:
        l=l-360
    alpha2 = (l/2)*DegToRad;
    delta=b*DegToRad
    r2=math.sqrt(2)
    f=2*r2/math.pi
    cdec=math.cos(delta)
    denom  = math.sqrt(1 + cdec*math.cos(alpha2));
    x      = cdec*math.sin(alpha2)*2.*r2/denom;
    y      = math.sin(delta)*r2/denom;
    x     *= RadToDeg/f;
    y     *= RadToDeg/f;
    
    x=int(x)
    y=int(y)
    x=x+180
    y=90-y  
    return x,y
def pmt():
    f = open('pmt_sorted.txt','r')
    num=0
    total=0
    result=[[-1 for col in  range(360)] for row in range(180)]
    for line in f:
        total=total+1
        (pid,z,phi) = line.split()
        z=float(z)
        phi=float(phi)
        z=math.acos(z/19500)    
        x=int(z*180/math.pi)
        y=int(phi*180/math.pi)
        if x==180:
            x=x-1
        
        if y==360:
            y=360-1
            
        y,x=ProjectAitoff2xy(y, x)
        if y==360:
            y=y-1
        result[x][y]=int(pid)
    return result  
    
    
