from datetime import datetime
import math
import time
import tensorflow.python.platform
from tensorflow.python.platform import gfile
import os 
import numpy as np
import tensorflow as tf
import junonn_inputs
import junonn_inference_vgg16
import junonn_train_cd as junonn_train
import junonn_loss
FLAGS = tf.app.flags.FLAGS
tf.app.flags.DEFINE_string('eval_dir', r'G:/python/CNN-net/data/tmp/',"""Directory where to write event logs.""")
tf.app.flags.DEFINE_string('eval_data', r'G:/python/CNN-net/data',"""Either 'test' or 'train_eval'.""")
tf.app.flags.DEFINE_string('ckp_dir', r'G:/python/CNN-net/keep_dict/',"""Directory where to read model checkpoints.""")
tf.app.flags.DEFINE_integer('eval_interval_secs', 60 ,"""How often to run the eval.""")
tf.app.flags.DEFINE_integer('num_examples', 1000,"""Number of examples to run.""")
tf.app.flags.DEFINE_boolean('run_once', False,"""Whether to run eval only once.""")
def eval_once(saver, summary_writer, error_mean,summary_op):

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
            num_iter = int(math.ceil(FLAGS.num_examples / FLAGS.batch_size))
            error_total = 0  
            total_sample_count = num_iter * FLAGS.batch_size
            step = 0

            while step < num_iter and not coord.should_stop():
                predictions = sess.run(error_mean)
                error_total += np.sum(predictions)
                step += 1
            # Compute precision @ 1.

            error_final = error_total / step
            print('%s: error_final @ 1 = %.3f with step:%d' % (datetime.now(), error_final,step))
            summary = tf.Summary()
            summary.ParseFromString(sess.run(summary_op))
            summary.value.add(tag='Precision @ 1', simple_value=error_final)
            summary_writer.add_summary(summary, global_step)

        except Exception as e:  # pylint: disable=broad-except
            coord.request_stop(e)

        coord.request_stop()
        coord.join(threads, stop_grace_period_secs=10)

def evaluate():
    with tf.Graph().as_default():
        eval_data = FLAGS.eval_data
        images, labels = junonn_inputs.inputs(eval_data=eval_data,train_data=False)
        logits = junonn_inference_vgg16.inference(images)

        loss = junonn_loss.loss(logits,labels)
        # Calculate predictions.
        error = logits-labels
        error_mean = tf.sqrt(tf.reduce_mean(tf.multiply(error,error)))    

        variable_averages = tf.train.ExponentialMovingAverage(junonn_train.MOVING_AVERAGE_DECAY)
        variables_to_restore = variable_averages.variables_to_restore()
        saver = tf.train.Saver(variables_to_restore)
        # Build the summary operation based on the TF collection of Summaries.
        summary_op = tf.summary.merge_all()
        graph_def = tf.get_default_graph().as_graph_def()
        summary_writer = tf.summary.FileWriter(FLAGS.eval_dir,graph_def=graph_def)
        while True:
            eval_once(saver, summary_writer, error_mean,summary_op)
            if FLAGS.run_once:
                break
            time.sleep(FLAGS.eval_interval_secs)

def main(argv=None):
    if gfile.Exists(FLAGS.eval_dir):
        gfile.DeleteRecursively(FLAGS.eval_dir)
    gfile.MakeDirs(FLAGS.eval_dir)
    '''
    import os 
    os.environ["CUDA_DEVICE_ORDER"]="PCI_BUS_ID"   # see issue #152
    gpu_id='4,5,6,7'
    os.environ["CUDA_VISIBLE_DEVICES"]=str(gpu_id)
    '''
    evaluate()
if __name__ == '__main__':
    tf.app.run()


