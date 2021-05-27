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
def read(file,start):
    change_file=os.path.basename(file)
    print(file)
    writer = tf.python_io.TFRecordWriter(FLAGS.output_dir+"evt_1000_"+str(start)+".bin.tfrecords")
    
    for serialized_example in tf.python_io.tf_record_iterator(file):
        example = tf.train.Example()
        example.ParseFromString(serialized_example)
        image = example.features.feature['image_raw'].float_list.value
        genlabel = example.features.feature['label_raw'].float_list.value
        reclabel = example.features.feature['reclabel_raw'].float_list.value
        image = np.reshape(image,[180,360,2])
        shape=np.shape(image)
        
        sat=2000
        q_max = 0
        num = 0
        for the in range(shape[0]):
            for phi in range(shape[0]):
                if image[the][phi][0]>sat:
                   #print(image[the][phi][0])
                   if q_max<image[the][phi][0]:
                       q_max=image[the][phi][0]
                       num+=1
                   image[the][phi][0]=np.sqrt(((np.sqrt(1+8*np.square(image[the][phi][0]/sat)))-1)/(4*np.square(image[the][phi][0]/sat)))*image[the][phi][0]*np.sqrt(2)
                   #print(image[the][phi][0])
        #print(q_num,q_new_num)
        #print(reclabel)
        print(num,q_max)
        image_ch = np.reshape(image,[-1]) 
        example = tf.train.Example(features = tf.train.Features(feature={
            'image_raw': _floatlist_feature(image_ch),
            'genlabel_raw': _floatlist_feature(genlabel),
            'reclabel_raw': _floatlist_feature(reclabel),
        }))
        
        writer.write(example.SerializeToString())        
        
    writer.close()

def change_tfrecords():
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        filenames = [os.path.join(FLAGS.input_dir, 'evt_1000_%d.bin.tfrecords' % i)
                         for i in xrange(101,121)]
        i=-1
        for allDir in filenames:
            i=i+1
            start=FLAGS.start+i
            #print(allDir)
            if os.path.isfile(allDir):
                read(allDir,start)
    if(os.path.isfile(FLAGS.input_dir)):
        start=FLAGS.start
        for i in range(2):
            read(FLAGS.input_dir,start)
            start=start+1

change_tfrecords()
#tensorboard --logdir G:\tarin\neural-1-tt\eval-tt-rot
