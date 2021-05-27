#coding=utf-8
#idea:using healpix get pixel and neighbor; pixel and neighbor convolutional  
#使用healpix处理数据，使用对应于healpix的pixel卷积核进行卷积运算
#方法：1. 使用healpix处理球面图片，得到healpix map
#      2. 卷积核大小取一个pixel和它邻居的8个pixel的大小，做为卷积核的大小
import os
import healpy as hp
import matplotlib.pyplot as plt
import numpy as np
import math
nside=64
#convert pmt(id,loc) to hp map
def convert_map():
    f = open('pmt_sorted.txt','r')
    thetas=[]
    phis=[]
    pids=[]
    
    npix = hp.nside2npix(nside)
    null=0
    for line in f:    
        (pid,z,phi) = line.split()
        phi=float(phi)
        z=math.acos(float(z)/19500)    
        thetas.append(z)
        phis.append(phi)
        pids.append(int(pid))
    
    print(np.shape(pids))
    # Go from HEALPix coordinates to indices
    indices = hp.ang2pix(nside, thetas, phis)
    print(np.shape(indices))
    print(npix)
    # Initate the map and fill it with the values
    hpxmap = np.zeros(npix, dtype=np.float)-1
    for i in range(len(pids)):
        if hpxmap[indices[i]]>0:
            null+=1
            continue
        hpxmap[indices[i]] = pids[i]
        #hpxmap[indices[i]] += pids[i]
    
    # Inspect the map
    print(hpxmap)
    #hp.mollview(hpxmap)
    return hpxmap,npix
#rotate hp map
def rot_map():
    t,p = hp.pix2ang(nside, np.arange(hp.nside2npix(nside)))
    
    rot_phi=0
    rot_theta=np.pi
    r=hp.Rotator(deg=False, rot=[rot_phi,rot_theta])
    trot, prot = r(t,p)
    rot_map = hp.get_interp_val(hpxmap, trot, prot)
    hp.mollview(rot_map)    
    return rot_map
#project hp map

def proj_neighbor():
    hpxmap,npix=convert_map()
    res=[]
    for i in range(npix):
        theta,phi=hp.pix2ang(nside,i)
        #print(hp.ang2pix(nside,thetas[i], phis[i]))
        conv_unit=[]
        ori_pix=i
        nb_pix=hp.get_all_neighbours(nside,theta, phi)
        #SW, W, NW, N, NE, E, SE and S
        conv_unit.append(hpxmap[i])
        for pix in nb_pix:
            conv_unit.append(hpxmap[pix])
        res.append(conv_unit)
    
    res=np.reshape(res,-1)
    return res

def proj_1D():
    hpxmap,npix=convert_map()
    res = hp.reorder(hpxmap, r2n=True)
    return res
