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
def rotate(the,phi,rot_z):    
    vec=ROOT.TVector3(sin(the)*cos(phi),sin(the)*sin(phi),cos(the))

    rz=ROOT.TRotation().RotateZ(rot_z)

    rot=rz*vec
    root_the=rot.Theta()
    root_phi=rot.Phi()
    if root_phi<0:
        root_phi=root_phi+2*math.pi
    #print(root_the,root_phi)
    
    return root_the,root_phi

#rotate(4*np.pi/180,100*np.pi/180)
def read(file,start):
    change_file=os.path.basename(file)
    print(file)
    writer = tf.python_io.TFRecordWriter(FLAGS.output_dir+"evt_1000_"+str(start)+".bin.tfrecords")
    
    for serialized_example in tf.python_io.tf_record_iterator(file):
      num=0
      rot_num=0
      while(num<1):
        rot_z=random.uniform(-math.pi,math.pi)

        example = tf.train.Example()
        example.ParseFromString(serialized_example)
        image = example.features.feature['image_raw'].float_list.value
        label = example.features.feature['label_raw'].float_list.value
        image = np.reshape(image,[180,360,2])
        
        #print(genlabel)
        #print(reclabel)
        q_num=0
        q_new_num=0
        shape=np.shape(image)
        
        image_new = np.zeros([180,360,2])-1
        #chage genstart
        gen_thes,gen_phis=rotate(label[0], label[1],rot_z)
        #change gendirection
        gen_thep,gen_phip=rotate(label[2], label[3],rot_z) 
        ''' 
        #select 
        if cos(gen_thes)<0.5:
            num=num-1
            rot_num=rot_num+1
            if rot_num> (1-cos(gen_thes))*5:
               num=1
        
        '''    
        #chage pmt (q,t)
        num=num+1
        for the in range(shape[0]):
            for phi in range(shape[1]):
                if image[the][phi][1]>-0.5:
                    q_num=q_num+1
                    the_rad=the*np.pi/180
                    phi_rad=phi*np.pi/180
                    the_new,phi_new=rotate(the_rad, phi_rad,rot_z)
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
        print(rot_z)
        print(label)
        print(label_gen)
        example = tf.train.Example(features = tf.train.Features(feature={
            'image_raw': _floatlist_feature(image_ch),
            'label_raw': _floatlist_feature(label_gen),
        }))
        
        writer.write(example.SerializeToString())
    writer.close()

def change_tfrecords():
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        filenames = [os.path.join(FLAGS.input_dir, 'evt_1000_%d.bin.tfrecords' % i)
                         for i in xrange(99,101)]
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
