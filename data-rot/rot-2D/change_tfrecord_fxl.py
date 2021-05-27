import os
from math import *
import gc
from six.moves import xrange  # pylint: disable=redefined-builtin
import tensorflow as tf
tf.app.flags.DEFINE_string('input_dir', r'G:/python/CNN-net/data/evt_1000_1.bin.tfrecords',
                           """Path to the CIFAR-10 data directory.""")
tf.app.flags.DEFINE_string('output_dir', r'G:/python/data-change/',
                           """Path to the G:/python/data-change/ data directory.""")
tf.app.flags.DEFINE_integer('start', 1,                   
                           """start count.""")
FLAGS = tf.app.flags.FLAGS
import numpy as np
import ROOT
import math
import random

def _floatlist_feature(value):
    return tf.train.Feature(float_list=tf.train.FloatList(value=value))
'''
def rotate(the,phi,rot_x,rot_y,rot_z):    
    vec=ROOT.TVector3(sin(the)*cos(phi),sin(the)*sin(phi),cos(the))

    rx=ROOT.TRotation().RotateX(rot_x)
    ry=ROOT.TRotation().RotateY(rot_y)
    rz=ROOT.TRotation().RotateZ(rot_z)

    rot_one=ry*vec
    rot_two=rz*rot_one
    rot  =rx*rot_two
    #rot=rz*vec    

    root_the=rot.Theta()
    root_phi=rot.Phi()
    if root_phi<0:
        root_phi=root_phi+2*math.pi
    #print(root_the,root_phi)
    
    return root_the,root_phi
'''
def rotate_fxl(the,phi,vr):
    vec=ROOT.TVector3(sin(the)*cos(phi),sin(the)*sin(phi),cos(the))
    rot=vr*vec
    root_the=rot.Theta()
    root_phi=rot.Phi()
    if root_phi<0:
        root_phi=root_phi+2*math.pi
    return root_the,root_phi


#rotate(4*np.pi/180,100*np.pi/180)
def read(file,start):
    change_file=os.path.basename(file)
    print(file)
    writer = tf.python_io.TFRecordWriter(FLAGS.output_dir+"evt_1000_"+str(start)+".bin.tfrecords")
    
    for serialized_example in tf.python_io.tf_record_iterator(file):
      num=0
      while(num<1):
        #rot_x=random.uniform(-math.pi,math.pi)
        #rot_y=random.uniform(-math.pi,math.pi)
        #rot_z=random.uniform(-math.pi,math.pi)
        rndmcostheta = random.uniform(-1,1);
        rndmphi = random.uniform(0,math.pi*2);
        z = rndmcostheta;
        r = sqrt(1-rndmcostheta*rndmcostheta);
        x = r*cos(rndmphi);
        y = r*sin(rndmphi);
        vst=ROOT.TVector3(x,y,z)
        

        example = tf.train.Example()
        example.ParseFromString(serialized_example)
        image = example.features.feature['image_raw'].float_list.value
        genlabel = example.features.feature['genlabel_raw'].float_list.value
        reclabel = example.features.feature['reclabel_raw'].float_list.value
        image = np.reshape(image,[180,360,2])
 
        vs=ROOT.TVector3(sin(genlabel[0])*cos(genlabel[1]),sin(genlabel[0])*sin(genlabel[1]),cos(genlabel[0]))
        vst=ROOT.TVector3(x,y,z)        
        a = vs.Angle(vst);
        v = vs.Cross(vst);
        vr=ROOT.TRotation().Rotate(a,v)
                

        #print(genlabel)
        #print(reclabel)
        q_num=0
        q_new_num=0
        shape=np.shape(image)
        
        image_new = np.zeros([180,360,2])-1
        #chage genstart
        #gen_thes,gen_phis=rotate(genlabel[0], genlabel[1],rot_x,rot_y,rot_z)
        gen_thes,gen_phis=rotate_fxl(genlabel[0], genlabel[1],vr)
        #change gendirection
        #gen_thep,gen_phip=rotate(genlabel[2], genlabel[3],rot_x,rot_y,rot_z) 
        gen_thep,gen_phip=rotate_fxl(genlabel[2], genlabel[3],vr)
        #chage recstart
        #rec_thes,rec_phis=rotate(reclabel[0], reclabel[1],rot_x,rot_y,rot_z)
        rec_thes,rec_phis=rotate_fxl(reclabel[0], reclabel[1],vr)
        #change recdirection
        #rec_thep,rec_phip=rotate(reclabel[2], reclabel[3],rot_x,rot_y,rot_z)
        rec_thep,rec_phip=rotate_fxl(reclabel[2], reclabel[3],vr)
        #print(thes_new, phis_new, thep_new, phip_new)
        
        #select 
        if cos(rec_thep)>-0.2:
            continue        
        #chage pmt (q,t)
        num=num+1
        for the in range(shape[0]):
            for phi in range(shape[1]):
                if image[the][phi][1]>-0.5:
                    q_num=q_num+1
                    the_rad=the*np.pi/180
                    phi_rad=phi*np.pi/180
                    #the_new,phi_new=rotate(the_rad, phi_rad,rot_x,rot_y,rot_z)
                    the_new,phi_new=rotate_fxl(the_rad, phi_rad,vr)
                    phi_new=int(round(phi_new*180/np.pi))
                    the_new=int(round(the_new*180/np.pi))
                    if the_new==180:
                        the_new=0
                    if phi_new==360:
                        phi_new=0
                    if image_new[the_new][phi_new][0]<-0.5:
                        
                        q_new_num=q_new_num+1
                        image_new[the_new][phi_new][0]=image[the][phi][0]
                        image_new[the_new][phi_new][1]=image[the][phi][1]
                    else:
                        #print(the,phi,the_new,phi_new)
                        image_new[the_new][phi_new][0]=(image_new[the_new][phi_new][0]+image[the][phi][0])/2
                        image_new[the_new][phi_new][1]=(image_new[the_new][phi_new][1]+image[the][phi][1])/2
        #print(q_num,q_new_num)
        #save new file
        image_ch = np.reshape(image_new,[-1])
        label_gen = np.array([gen_thes,gen_phis,gen_thep,gen_phip])
        label_rec = np.array([rec_thes,rec_phis,rec_thep,rec_phip])
        #print(rot_x,rot_y,rot_z)
        #print(reclabel)
        #print(genlabel)
        #print(label_rec)
        #print(label_gen)
        example = tf.train.Example(features = tf.train.Features(feature={
            'image_raw': _floatlist_feature(image_ch),
            'genlabel_raw': _floatlist_feature(label_gen),
            'reclabel_raw': _floatlist_feature(label_rec),
        }))
        
        writer.write(example.SerializeToString())        
        
    writer.close()

def change_tfrecords():
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        filenames = [os.path.join(FLAGS.input_dir, 'evt_1000_%d.bin.tfrecords' % i)
                         for i in xrange(16,21)]
        i=-1
        for allDir in filenames:
            i=i+1
            start=FLAGS.start+i
            print(allDir)
            if os.path.isfile(allDir):
                read(allDir,start)
    if(os.path.isfile(FLAGS.input_dir)):
        start=FLAGS.start
        for i in range(1):
            read(FLAGS.input_dir,start)
            start=start+1

change_tfrecords()
#tensorboard --logdir G:\tarin\neural-1-tt\eval-tt-rot
