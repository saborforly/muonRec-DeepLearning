#import matplotlib
#import numpy as np
#import matplotlib.pyplot as plt
def pmt():
    import math
    f = open('/junofs/users/liuyan/muon-sim/sim-nn/data-tfrecord/pmt_sorted.txt','r')
    num=0
    total=0
    result=[[-1 for col in  range(360)] for row in range(180)]
    for line in f:
        total=total+1
        (pid,z,phi) = line.split()
        z=float(z)
        phi=float(phi)
        z=math.acos(z/19500)    
        x=int(round(z*180/math.pi))
        y=int(round(phi*180/math.pi))
        
        if x==180:
            x=0
        #if result[x][y]!=-1:
        #    num=num+1
        result[x][y]=int(pid)
    '''
    for the in range(180):
            for phi in range(360):            
                if result[the][phi]>-0.5:
                    num=num+1
    print(num)
    '''
    return result  

#pmt()
