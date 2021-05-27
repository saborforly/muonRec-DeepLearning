#!/usr/bin/env python
import numpy as np
def pmt():
    f = open('pmt_sorted.txt','r')
    result = []
    line_cur = []
    phi_cur=-1
    z_cur=20000
    for line in f:
        (pid,z,phi) = line.split()
        if z!=z_cur:
            result.append(line_cur)
            line_cur = []
            z_cur = z
            phi_cur = -1
        line_cur.append(int(pid))
        phi=float(phi)
        if phi>phi_cur:
            phi_cur = phi
        else:
            print ('Current (%s,%s,%s) with phi %s not bigger than old one %s'%(pid,z,phi,phi,phi_cur))
    result.append(line_cur)
    result = result[1:]
    shape = [len(line) for line in result]
    maxl = max(shape)
    #print()
    for i,line in enumerate(result):
        ll = len(line)
        result[i] = [-1]*int(((maxl-ll)/2))+line
    print(np.shape(result[63]))
    return result

pmt()
    
