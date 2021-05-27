#!/usr/bin/env python

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
        result[x][y]=int(pid)
    for i in range(180):
        j=0
        while j<360:
            gua=[]
            for k in range(10):
                if(result[i][j+k]>0):
                    print(result[i][j+k])
                    gua.append(result[i][j+k])
                    result[i][j+k]=-1        
            nuValue=int((10-len(gua))/2)
            for l,value in enumerate(gua):
                result[i][j+l+nuValue]=value
            j=j+10
    return result  


pmt()
