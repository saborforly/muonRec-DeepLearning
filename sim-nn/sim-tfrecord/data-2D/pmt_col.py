import matplotlib
import numpy as np
import matplotlib.pyplot as plt
def pmt():
    import math
    #f = open('/junofs/users/liuyan/muon-sim/sim-nn/data-tfrecord/pmt_sorted.txt','r')
    f = open('pmt_sorted.txt','r')
    
    num=0
    total=0
    result=[[[-1 for cha in range(2)] for col in  range(360)] for row in range(180)]
    result=np.array(result)
    print(np.shape(result))
    for line in f:
        #total=total+1
        (pid,z,phi) = line.split()
        z=float(z)
        phi=float(phi)
        z=math.acos(z/19500)    
        x=int(round(z*180/math.pi))
        y=int(round(phi*180/math.pi))
        
        if x==180:
            x=0
        
        #LPmt
        if int(pid)<17739:
            result[x][y][0]=int(pid)
        #SPmt
        if int(pid)>=17739:
            total=total+1
            if result[x][y][1]>0:
                num+=1                
            result[x][y][1]=int(pid)
    
    #plt.imshow(result[:,:,1])
    #plt.show()
    print(num,total)
    return result  
if __name__=='__main__':
    shape=pmt()
    height = len(shape)
    width = max([len(line) for line in shape])
    print(height,width)
    num=0
    for i,line in enumerate(shape):
        for j,index in enumerate(line):
            if index[1]>0 and index[0]>-0.1:
                num=num+1
            #print(i,j,index[1])
    print(num)    
