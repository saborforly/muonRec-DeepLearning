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
def eval_energy(genlabel):
    thes=genlabel[0]
    phis=genlabel[1]
    thep=genlabel[2]
    phip=genlabel[3]
    energy=genlabel[4]
    ix=sin(thes)*cos(phis)*17700
    iy=sin(thes)*sin(phis)*17700
    iz=cos(thes)*17700
    px=sin(thep)*cos(phip)
    py=sin(thep)*sin(phip)
    pz=cos(thep)

    s=ROOT.TVector3(ix,iy,iz)
    p=ROOT.TVector3(px,py,pz)
    dist_cc = (s.Cross(p)).Mag()
    track_length=np.sqrt(np.square(17700)-np.square(dist_cc))*2
    use_energy=track_length*2/10+10000
    
    if use_energy<energy:
        print(use_energy,energy)
        return True
    else:
        return False
    

def read(file):
    change_file=os.path.basename(file)
    writer = tf.python_io.TFRecordWriter(FLAGS.output_dir+change_file)
    for serialized_example in tf.python_io.tf_record_iterator(file):
        example = tf.train.Example()
        example.ParseFromString(serialized_example)
        image = example.features.feature['image_raw'].float_list.value
        label = example.features.feature['label_raw'].float_list.value
        #if not (eval_energy(genlabel)):
        #    continue
        genlabel=np.array([1])
        print(label,genlabel)
        #save new file
        example = tf.train.Example(features = tf.train.Features(feature={
            'image_raw': _floatlist_feature(image),
            'label_raw': _floatlist_feature(genlabel),
        }))
        
        writer.write(example.SerializeToString())        
    writer.close() 

def change_tfrecords():
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        for allDir in pathDir:
            allDir=FLAGS.input_dir+allDir
            if os.path.isfile(allDir):
                read(allDir)
    if(os.path.isfile(FLAGS.input_dir)):
        start=FLAGS.start
        for i in range(2):
            read(FLAGS.input_dir,start)
            start=start+1
if __name__=="__main__":
    change_tfrecords()
#tensorboard --logdir G:\tarin\neural-1-tt\eval-tt-rot
