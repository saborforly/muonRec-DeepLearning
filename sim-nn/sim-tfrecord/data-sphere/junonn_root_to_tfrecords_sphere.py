from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf
import os
from math import *
import healpix_sphere as sphere
import re
import ROOT

def sp(fname):
    fname = re.sub(r'(.*)evt_', "", fname)
    fname = re.sub(r'.root', "", fname)
    m = re.split(r'_', fname)
    ix=float(m[0])
    iy=float(m[1])
    iz=float(m[2])
    px=float(m[3])
    py=float(m[4])
    pz=float(m[5])
    #print(ix)
    s=ROOT.TVector3(ix,iy,iz)
    p=ROOT.TVector3(px,py,pz)
    #thes=s.Theta()
    #phis=s.Phi()

    m_LSRadius=17700
    gcentrk = s + (-s).Dot(p)*p;
    dist_cc = (s.Cross(p)).Mag()
    gr0 = gcentrk - float(np.sqrt(m_LSRadius*m_LSRadius - dist_cc*dist_cc))*p
    #print(gr0[0]/17700)
    thes=gr0.Theta()
    phis=gr0.Phi()


    thep=p.Theta()
    phip=p.Phi()


    return thes,phis,thep,phip

def loadEvent(event,fname):
    #print ('Input files:' )
    
    #import ROOT
    #event = ROOT.TChain('evt')
    #event.Add(fname+'/evt')
    print ('Total events: ' + str(event.GetEntries()))
    cur = [-1]
    
    shape = sphere.proj_1D()
    height = len(shape)
    
    test_count=[-1 for i in range(height)]
    test_count=np.array(test_count)
    
    entries = event.GetEntry()
    #thes,phis,thep,phip=sp(fname)
    class Record(object):
        pass
    
    def assign():
        cur[0] += 1
        if cur[0] >= entries:
            raise Exception('Loaded up all the events.')
        thes,phis,thep,phip=fname[cur[0]]
        
        status = event.GetEntry(cur[0])
        if status==0:
            raise Exception('Something wrong with loading Event : ' + str(cur[0]))

        record = Record
        #record.reclabel = np.array([thes, phis, thep, phip])
        record.reclabel = np.array([thes, phis, thep, phip])        
        record.genlabel = np.array([event.thes, event.phis, event.thep, event.phip])
        #record.label = np.array([sin(event.thes)*cos(event.phis),sin(event.thes)*sin(event.phis),cos(event.thes),sin(event.thep)*cos(event.phip),sin(event.thep)*sin(event.phip),cos(event.thep)])
        record.image = np.zeros([height,2])
        
        if cur[0]%100==0:
            print(cur[0],entries)
            print(record.reclabel)
            print(record.genlabel)
            print('=>> Loading event : ' + str(cur[0]))
            #print(shape)
            print(np.shape(shape)) 
        for i,index in enumerate(shape):
            index=int(index)
            record.image[i][0] = -1 if (index==-1) else event.q[index]
            record.image[i][1] = -1 if (index==-1) else event.t[index]

        
        test_image=np.array(record.image[:,0])
        if (test_count==test_image).all() :
            print('input file wrong with loading Event : ' + str(cur[0]))



        return record

    return assign

def _int64_feature(value):
  return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))
def _floatlist_feature(value):
  return tf.train.Feature(float_list=tf.train.FloatList(value=value))

def convert_to(record, writer):
    image = np.reshape(record.image,[-1])
    genlabel = record.genlabel
    reclabel = record.reclabel

    example = tf.train.Example(features = tf.train.Features(feature={
        'genlabel_raw': _floatlist_feature(genlabel),
        'reclabel_raw': _floatlist_feature(reclabel),
        'image_raw': _floatlist_feature(image)}))

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
def readfile(event,fname,evtcounter,writer):
    event_data = loadEvent(event,fname)
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
            convert_to(event_data(),writer)
            evtcounter+=1
    except Exception(e):
        print(e)
    #writer.close()
    return evtcounter,writer

def main(argv):
    evtcounter=0
    event = ROOT.TChain('evt')
    #event.Add(fname+'/evt')
    fname=[]
    writer = tf.python_io.TFRecordWriter('evt.tfrecords')
    if(os.path.isdir(FLAGS.input_dir)):
        pathDir =  os.listdir(FLAGS.input_dir)
        for allDir in pathDir:
            child = os.path.join('%s%s' % (FLAGS.input_dir, allDir))
            event.Add(child+'/evt')
            fname.append(sp(child))
        evtcounter,writer=readfile(event,fname,evtcounter,writer)
        
    if(os.path.isfile(FLAGS.input_dir)):
        event.Add(FLAGS.input_dir+'/evt')
        readfile(event,evtcounter,writer)
    writer.close()
if __name__ == '__main__':
    tf.app.run()

    
    
    
    
    
#sp("signal_-4304.16_-8120.36_25000_0.155189_-0.213673_-0.9645.root")
#print(np.arctan(1/np.sqrt(2)))
