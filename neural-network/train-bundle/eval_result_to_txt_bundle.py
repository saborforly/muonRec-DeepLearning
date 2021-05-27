from datetime import datetime
import math
import time
import os
import numpy as np
import tensorflow as tf
import junonn_inputs_bundle_eval
import junonn_inference_vgg16_bundle
import junonn_train
from tensorflow.python.framework.errors_impl import InvalidArgumentError
import argparse
import datetime
FLAGS = tf.app.flags.FLAGS

tf.app.flags.DEFINE_string('input', r'G:\python\CNN-net\data',
                           """Path to the CIFAR-10 data directory.""")
tf.app.flags.DEFINE_string('output', r'data.txt',
                           """Directory where to write event logs """
                           """and checkpoint.""")                        
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/temp/keep',
                           """Directory where to load event logs """
                           """and checkpoint.""")
tf.app.flags.DEFINE_integer('evtmax', 2,
                            """Number of batches to run.""")
tf.app.flags.DEFINE_integer('batch_size', 2,
                            """Number of images to process in a batch.""")
def eval_once(saver, evals, output):
    #gpu_num = int(os.environ.get('GPU_NUM', 1))
    #session_conf = tf.ConfigProto( device_count={"GPU": gpu_num},intra_op_parallelism_threads=gpu_num, inter_op_parallelism_threads=gpu_num,log_device_placement=True)
    with tf.Session() as sess:
        ckpt = tf.train.get_checkpoint_state(FLAGS.ckp_dir)
        if ckpt and ckpt.model_checkpoint_path:
            saver.restore(sess, ckpt.model_checkpoint_path)
            global_step = ckpt.model_checkpoint_path.split('/')[-1].split('-')[-1]
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
            starttime = datetime.datetime.now()
            while step < num_iter and not coord.should_stop():
                labels,logits,tag = sess.run(evals)
                #print(np.shape(logits))
                print(logits)
                print(labels)
                result = np.concatenate([labels,logits,tag],1)
                #print (result)
                np.savetxt(output,result,'%s')
                step+=1
            endtime = datetime.datetime.now()
            print ((endtime - starttime).seconds)

        except Exception as e:  # pylint: disable=broad-except
            coord.request_stop(e)

        coord.request_stop()
        coord.join(threads, stop_grace_period_secs=10)

def inputs(filenames,batch_size):
   
    filenames = [os.path.join(FLAGS.input, 'evt_1000_%d.bin.tfrecords' % i)
                 for i in range(101,121)]    
    for f in filenames:
      if not tf.gfile.Exists(f):
        raise ValueError('Failed to find file: ' + f)    
    filename_queue = tf.train.string_input_producer(filenames)

    read_input = junonn_inputs_bundle_eval.read_junonn(filename_queue)

    return junonn_inputs_bundle_eval._generate_image_and_label_batch(read_input.image,read_input.label,read_input.tag,
                                                       batch_size*10, batch_size,
                                                       shuffle=False)

def evaluate():
    """Eval CIFAR-10 for a number of steps."""
    with tf.Graph().as_default() as g:
        # Get images and labels for CIFAR-10.
        images, labels, tag = inputs(FLAGS.input,FLAGS.batch_size)
        

        # inference model.
        logits = junonn_inference_vgg16_bundle.inference(images)
        print(labels)
        logits = tf.nn.softmax(logits)
        #logits = tf.argmax(logits,1)
        #logits = tf.reshape(logits, [FLAGS.batch_size,1])
        evals= [labels,logits,tag]
        print("bundle is pre:  ", logits,labels)

        # Restore the moving average version of the learned variables for eval.
        variable_averages = tf.train.ExponentialMovingAverage(
            junonn_train.MOVING_AVERAGE_DECAY)
        variables_to_restore = variable_averages.variables_to_restore()
        saver = tf.train.Saver(variables_to_restore)

        output = open(FLAGS.output,'w')
        eval_once(saver, evals, output)
        output.close()


def main(argv=None):  # pylint: disable=unused-argument
    evaluate()


if __name__ == '__main__':
    tf.app.run()
