import tensorflow as tf
import numpy as np

def loss(logits, labels):
    labels = tf.cast(labels, tf.float32)
    error2 = tf.square(logits - labels)
    error_mean = tf.sqrt(tf.reduce_mean(error2),name="mean_standard_error_loss")
    
    tf.add_to_collection('losses', error_mean)
    with tf.name_scope('eval'):
        with tf.name_scope('d_start') as scope:
            sd = tf.sqrt(tf.reduce_sum(error2[:,:3],axis=1))
            mean = tf.reduce_mean(sd)
            tf.summary.scalar('mean',mean)
            stddev = tf.sqrt(tf.reduce_mean(tf.square(sd-mean)))
            tf.summary.scalar('stddev',stddev)
            #tf.summary.histogram(scope,sd)
        
        with tf.name_scope('angle') as scope:
            
            mul = tf.reduce_sum(tf.multiply(logits,labels)[:,3:],axis=1)
            sqrt= tf.sqrt(tf.reduce_sum(tf.square(logits[:,3:]),axis=1))
            angle = tf.acos(mul/sqrt)*180/np.pi
            #angle nan
            mean = tf.reduce_mean(angle)
            tf.summary.scalar('mean',mean)
            stddev = tf.sqrt(tf.reduce_mean(tf.square(angle-mean)))
            tf.summary.scalar('stddev',stddev)
            #tf.summary.histogram(scope,angle_cos)

        with tf.name_scope('logits') as scope:
            with tf.name_scope('logits_start') as scope:
                start_norm2 = tf.sqrt(tf.reduce_sum(tf.square(logits[:,:3]),axis=1))
                mean = tf.reduce_mean(start_norm2)
                #tf.summary.scalar('mean',mean)
                #tf.summary.histogram(scope,start_norm2)
            with tf.name_scope('logits_direction') as scope:
                direction_norm2 = tf.sqrt(tf.reduce_sum(tf.square(logits[:,3:]),axis=1))
                mean = tf.reduce_mean(direction_norm2)
                #tf.summary.scalar('mean',mean)
                #tf.summary.histogram(scope,direction_norm2)
    # The total loss is defined as the cross entropy loss plus all of the weight
    # decay terms (L2 loss).
    return tf.add_n(tf.get_collection('losses'), name='total_loss')



