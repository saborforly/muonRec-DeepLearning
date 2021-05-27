#-*-coding=utf-8-*-
#正弦曲线投影，伪圆柱投影，其中所有纬线和中央子午线是直线。经线是基于正弦函数的曲线
import numpy as np
import math
#import pylab as plt
RadToDeg=180/math.pi
DegToRad=math.pi/180
def ProjectSinusoidal2xy(l,b):
    b=90-b
    if l>180:
        l=l-360    
    Al = l*np.cos(b*DegToRad);
    Ab = b;  
    Al=int(Al)
    Ab=int(Ab)    
    Al=Al+180
    Ab=90-Ab      
    return Al,Ab

def pmt():
    import math
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
            
        y,x=ProjectSinusoidal2xy(y, x)
        if y==360:
            y=y-1
        #print(x,y)
        result[x][y]=int(pid)
    
        
    return result  
    
    
#img=pmt()    
#plt.imshow(img)
#plt.show()      