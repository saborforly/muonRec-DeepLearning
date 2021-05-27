#coding=utf-8
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import re
import numpy as np
import tensorflow as tf
import os
from math import *
def loadEvent(event):
    #print ('Input files:' )
    
    #import ROOT
    #event = ROOT.TChain('evt')
    #event.Add(fname+'/evt')
    print ('Total events: ' + str(event.GetEntries()))
    cur = [-1]
    test_count=[[-1 for i in range(360)] for j in range(180)]
    test_count=np.array(test_count)
    import pmt_col as pmt
    shape = pmt.pmt()
    height = len(shape)
    width = max([len(line) for line in shape])
    entries = event.GetEntry()
    #thes,phis,thep,phip=sp(fname)
    class Record(object):
        pass
    
    def assign():
        cur[0] += 1
        if cur[0] >= entries:
            raise Exception('Loaded up all the events.')
            
        
        status = event.GetEntry(cur[0])
        if status==0:
            
            raise Exception('Something wrong with loading Event : ' + str(cur[0]))

        record = Record
        #record.reclabel = np.array([thes, phis, thep, phip])
        #record.label = np.array([event.thes, event.phis, event.thep, event.phip])
        #record.dist = np.array([event.thes[0], event.phis[0], event.thep[0], event.phip[0],event.thes[1], event.phis[1], event.thep[1], event.phip[1]])
        record.label = np.array([0])
        record.image = np.zeros([height,width,2])
        record.test=True

        #if cur[0]%100==0:
            #print( '=>> Loading event : ' + str(cur[0]))
        for i,line in enumerate(shape):
            for j,index in enumerate(line):
                record.image[i][j][0] = -1 if (index==-1) else event.q[index]
                record.image[i][j][1] = -1 if (index==-1) else event.t[index]
        if (test_count==record.image[:,:,0]).all() :
            print('input file wrong with loading Event : ' + str(cur[0]))
            record.test=False
        
        if np.isnan(event.thep) :
            record.test=False
        else:
            print( '=>> Loading event : ' + str(cur[0])) 
            print(record.label)
            
        return record

    return assign

def _int64_feature(value):
  return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))
def _floatlist_feature(value):
  return tf.train.Feature(float_list=tf.train.FloatList(value=value))

def convert_to(record, writer):
    image = np.reshape(record.image,[-1])
    label = record.label
    #dist = record.dist
    example = tf.train.Example(features = tf.train.Features(feature={
        'label_raw': _floatlist_feature(label),
        'image_raw': _floatlist_feature(image)}))
    #    'dist_raw': _floatlist_feature(dist)}))
    writer.write(example.SerializeToString())

tf.app.flags.DEFINE_string('input_dir', 'evt.root',
                           'Name of root file with muon events stored in tree')
tf.app.flags.DEFINE_string('output_dir', 'evt',
                           'Name of root file with muon events stored in tree')
tf.app.flags.DEFINE_integer('evtpf', 1000,
                            'Number of events stored per file.')
tf.app.flags.DEFINE_integer('start', 0,
                            'Counter of file names, e.g. evt_0.tf, evt_1.tf, evt_2.tf ...')
FLAGS = tf.app.flags.FLAGS
def readfile(event,evtcounter,writer):
    event_data = loadEvent(event)
    #opname = 'p' + opname.replace('*','').replace('.','')
    opname=FLAGS.output_dir+'evt'
    #evtcounter = 0
    print(opname)
    #writer = tf.python_io.TFRecordWriter(opname+'.tfrecords')
    try:
        while True:
            if FLAGS.evtpf != 0 and evtcounter%FLAGS.evtpf==0 :
                writer.close()
                print(opname+'_'+str(FLAGS.evtpf)+'_'+str(int(evtcounter/FLAGS.evtpf+FLAGS.start))+'.bin'+'.tfrecords')
                writer = tf.python_io.TFRecordWriter(opname+'_'+str(FLAGS.evtpf)+'_'+str(int(evtcounter/FLAGS.evtpf+FLAGS.start))+'.bin'+'.tfrecords')
            record=event_data()
            if record.test==False:
                print("find bad event")
                continue
            convert_to(record,writer)
            evtcounter+=1
    except Exception(e):
        print(e)
    #writer.close()
    return evtcounter,writer

def main(argv):
    evtcounter=0
    import re
    import ROOT
    event = ROOT.TChain('evt')
    #event.Add(fname+'/evt')

    writer = tf.python_io.TFRecordWriter('evt.tfrecords')
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        for allDir in pathDir:
            child = os.path.join('%s%s' % (FLAGS.input_dir, allDir))
            event.Add(child+'/evt')
        evtcounter,writer=readfile(event,evtcounter,writer)
        
    if(os.path.isfile(FLAGS.input_dir)):
        event.Add(FLAGS.input_dir+'/evt')
        readfile(event,evtcounter,writer)
    writer.close()
if __name__ == '__main__':
    tf.app.run()

    
    
    
    
    
#sp("signal_-4304.16_-8120.36_25000_0.155189_-0.213673_-0.9645.root")
#print(np.arctan(1/np.sqrt(2)))
