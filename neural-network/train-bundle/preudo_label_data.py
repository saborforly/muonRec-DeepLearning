import math
import time
import os
import numpy as np
import tensorflow as tf
import junonn_inference_vgg
from tensorflow.python.framework.errors_impl import InvalidArgumentError

tf.app.flags.DEFINE_string('input_dir', '/python/data-change/',  """Directory where to read model input.""")
tf.app.flags.DEFINE_string("output_dir", "g:/python/data-root-tf/train-low/", "Output file name")
tf.app.flags.DEFINE_string("ckp_dir", "g:/python/data-root-tf/train-low/", "Directory where to read model checkpoints.")
tf.app.flags.DEFINE_integer("evtmax", 1000, 'Events to be processed')
tf.app.flags.DEFINE_integer("batch_size",1, 'Number of events to process in a batch')
FLAGS = tf.app.flags.FLAGS
MOVING_AVERAGE_DECAY = 0.999
def _floatlist_feature(value):
    return tf.train.Feature(float_list=tf.train.FloatList(value=value))
def read_junonn(filename_queue):

    class JunoEvent(object):
        pass
    result = JunoEvent()

    result.labellength = 4
    result.height = 180
    result.width = 360
    result.depth = 2

    reader = tf.TFRecordReader()
    result.key,value = reader.read(filename_queue)
    features = tf.parse_single_example(value,
                                       features={
                                           'image_raw':tf.FixedLenFeature([result.height*result.width*result.depth],tf.float32),
                                           'label_raw':tf.FixedLenFeature([result.labellength],tf.float32),
                                           #'genlabel_raw':tf.FixedLenFeature([result.labellength],tf.float32)
                                  })


    #result.image = tf.reshape(features['image_raw'],[result.height,result.width,result.depth])
    result.init_image=features['image_raw']
    image = tf.reshape(features['image_raw'],[result.height,result.width,result.depth])
    imageq = image[:,:,0]
    imaget = image[:,:,1]
    meanq = tf.reduce_mean(imageq)
    meant = tf.reduce_mean(imaget)
    diffq=imageq-meanq
    difft=imaget-meant

    stdq=tf.sqrt(tf.reduce_mean(tf.square(diffq)))
    stdt=tf.sqrt(tf.reduce_mean(tf.square(difft)))

    resultimageq=diffq/stdq
    resultimaget=difft/stdt

    result.image =  tf.stack([resultimageq,resultimaget],axis=2)

    thes_true=features['label_raw'][0]
    phis_true=features['label_raw'][1]
    thep_true=features['label_raw'][2]
    phip_true=features['label_raw'][3]

    #result.label=tf.pack([tf.sin(thes_true)*tf.cos(phis_true),tf.sin(thes_true)*tf.sin(phis_true),tf.cos(thes_true),tf.sin(thep_true)*tf.cos(phip_true),tf.sin(thep_true)*tf.sin(phip_true),tf.cos(thep_true)])
    result.label=tf.stack([tf.sin(thes_true)*tf.cos(phis_true),tf.sin(thes_true)*tf.sin(phis_true),tf.cos(thes_true),tf.sin(thep_true)*tf.cos(phip_true),tf.sin(thep_true)*tf.sin(phip_true),tf.cos(thep_true),thes_true,phis_true,thep_true,phip_true])
    return result


def eval_once(saver, evals, writer):
    with tf.Session() as sess:
        ckpt = tf.train.get_checkpoint_state(FLAGS.ckp_dir)
        if ckpt and ckpt.model_checkpoint_path:
            saver.restore(sess, ckpt.model_checkpoint_path)
            global_step = ckpt.model_checkpoint_path.split('/')[-1].split('-')[-1]
            print(global_step)
        else:
            print('No checkpoint file found')
            return
        coord = tf.train.Coordinator()
        try:
            threads = []
            for qr in tf.get_collection(tf.GraphKeys.QUEUE_RUNNERS):
                threads.extend(qr.create_threads(sess, coord=coord, daemon=True,start=True))
            num_iter = int(math.ceil(FLAGS.evtmax / FLAGS.batch_size))
            print ('num_iter:%d for once,evtmax:%d,batch_size:%d' %(num_iter,FLAGS.evtmax,FLAGS.batch_size))
            total_sample_count = num_iter * FLAGS.batch_size
            print ("total_sample_count:%d for once" %(total_sample_count))
            step = 0
            
            while step < num_iter and not coord.should_stop():
                init_image,logits,labels = sess.run(evals)
                init_image = np.reshape(init_image,[-1])
                logits=np.reshape(logits,[-1])
                labels=np.reshape(labels,[-1])
                #print(init_image)
                print(logits)
                print(labels)
                example = tf.train.Example(features = tf.train.Features(feature={
                    'image_raw': _floatlist_feature(init_image),
                    'genlabel_raw': _floatlist_feature(labels),
                    'reclabel_raw': _floatlist_feature(logits),
                }))
                step+=1
                writer.write(example.SerializeToString())


        except Exception as e:  # pylint: disable=broad-except
            coord.request_stop(e)

        coord.request_stop()
        coord.join(threads, stop_grace_period_secs=10)

def inputs(filenames,batch_size):
   
    filenames = [os.path.join(FLAGS.input_dir, 'evt_1000_%d.bin.tfrecords' % i)
                 for i in range(35,36)]    
    for f in filenames:
        if not tf.gfile.Exists(f):
            raise ValueError('Failed to find file: ' + f)    
    filename_queue = tf.train.string_input_producer(filenames)

    read_input = read_junonn(filename_queue)
    num_preprocess_threads = 16
    init_image,image,label_batch = tf.train.batch(
            [read_input.init_image,read_input.image, read_input.label],
        batch_size=batch_size,
        num_threads=num_preprocess_threads,
        capacity=3 + 3 * batch_size)    
    return init_image,image,tf.reshape(label_batch, [batch_size,read_input.label.get_shape()[0].value])

def evaluate():
    """Eval CIFAR-10 for a number of steps."""
    with tf.Graph().as_default() as g:
        # Get images and labels for CIFAR-10.
        print(FLAGS.input_dir)
        init_image,image, labels_full = inputs(FLAGS.input_dir,FLAGS.batch_size)
        
        labels = labels_full[:,:6]
         

        # inference model.
        logits = junonn_inference_vgg.inference(image)
        
        logits = logits[:,:6]
        
        #print(logits,labels)
        evals= [init_image,logits,labels]

        # Restore the moving average version of the learned variables for eval.
        variable_averages = tf.train.ExponentialMovingAverage(
            MOVING_AVERAGE_DECAY)
        variables_to_restore = variable_averages.variables_to_restore()
        saver = tf.train.Saver(variables_to_restore)
        
        writer = tf.python_io.TFRecordWriter(FLAGS.output_dir+"evt_1000_4.bin.tfrecords")
        eval_once(saver, evals, writer)
        writer.close()


def main(argv=None):  # pylint: disable=unused-argument
    evaluate()


if __name__ == '__main__':
    tf.app.run()
