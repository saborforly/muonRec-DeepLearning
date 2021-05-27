from datetime import datetime
import math
import time
import os
import numpy as np
import tensorflow as tf
import junonn_inputs_bundle
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
                labels,logits,dist = sess.run(evals)
                #print(np.shape(logits))
                print(logits)
                print(labels)
                result = np.concatenate([labels,logits,dist],1)
                #print (result)
                np.savetxt(output,result,'%s')
                step+=1
            endtime = datetime.datetime.now()
            print ((endtime - starttime).seconds)

        except Exception as e:  # pylint: disable=broad-except
            coord.request_stop(e)

        coord.request_stop()
        coord.join(threads, stop_grace_period_secs=10)

def read_junonn(filename_queue):

    class JunoEvent(object):
        pass
    result = JunoEvent()

    result.labellength = 1
    result.height = 180
    result.width = 360
    result.depth = 2

    reader = tf.TFRecordReader()
    result.key,value = reader.read(filename_queue)
    features = tf.parse_single_example(value,
                                     features={
                                      'image_raw':tf.FixedLenFeature([result.height*result.width*result.depth],tf.float32),'label_raw':tf.FixedLenFeature([result.labellength],tf.float32),'dist_raw':tf.FixedLenFeature([result.labellength],tf.float32)})


    #result.image = tf.reshape(features['image_raw'],[result.height,result.width,result.depth])
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
    #result.image =  tf.stack([resultimaget],axis=2)
    label_true=features['label_raw'][0]
    dist_true=features['dist_raw'][0]
    
    result.label=tf.stack([label_true])
    result.dist=tf.stack([dist_true])
    #result.label=tf.pack([tf.sin(thes_true)*tf.cos(phis_true),tf.sin(thes_true)*tf.sin(phis_true),tf.cos(thes_true),tf.sin(thep_true)*tf.cos(phip_true),tf.sin(thep_true)*tf.sin(phip_true),tf.cos(thep_true)])
    return result
def _generate_image_and_label_batch(image, label,dist, min_queue_examples,
                                    batch_size, shuffle):
    # Create a queue that shuffles the examples, and then
    # read 'batch_size' images + labels from the example queue.
    num_preprocess_threads = 16
    if shuffle:
        images, label_batch, dist_batch = tf.train.shuffle_batch(
        [image, label, dist],
        batch_size=batch_size,
        num_threads=num_preprocess_threads,
        capacity=min_queue_examples + 3 * batch_size,
        min_after_dequeue=min_queue_examples)
    else:
        images, label_batch, dist_batch = tf.train.batch(
        [image, label, dist],
        batch_size=batch_size,
        num_threads=num_preprocess_threads,
        capacity=min_queue_examples + 3 * batch_size)

    # Display the training images in the visualizer.
    #tf.summary.image('images_q', images[:,:,:,:1])
    #tf.summary.image('images_t', images[:,:,:,1:])

    return images, tf.reshape(label_batch, [batch_size,label.get_shape()[0].value]), tf.reshape(dist_batch, [batch_size,dist.get_shape()[0].value])



def inputs(filenames,batch_size):
   
    filenames = [os.path.join(FLAGS.input, 'evt_1000_%d.bin.tfrecords' % i)
                 for i in range(0,8)]    
    for f in filenames:
      if not tf.gfile.Exists(f):
        raise ValueError('Failed to find file: ' + f)    
    filename_queue = tf.train.string_input_producer(filenames)

    read_input = read_junonn(filename_queue)

    return _generate_image_and_label_batch(read_input.image,read_input.label,read_input.dist, batch_size*10, batch_size,shuffle=False)

def evaluate():
    """Eval CIFAR-10 for a number of steps."""
    with tf.Graph().as_default() as g:
        # Get images and labels for CIFAR-10.
        images, labels ,dist= inputs(FLAGS.input,FLAGS.batch_size)
        

        # inference model.
        logits = junonn_inference_vgg16_bundle.inference(images)
        print(labels)
        logits = tf.nn.softmax(logits)
        logits = tf.argmax(logits,1)
        logits = tf.reshape(logits, [FLAGS.batch_size,1])
        evals= [labels,logits,dist]
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
