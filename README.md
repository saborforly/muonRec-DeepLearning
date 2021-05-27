# 基于深度学习的缪子重建
### 系统环境：
Python：2.7  
ROOT： 5.12  
TensorFlow：1.3.0  
Offline：J16v1r4、J17v1r1  
## 前言
江门中微子实验具有丰富的物理研究内容，其主要物理目标为测量中微子的质量等级和精确测量中微子的振荡参数。
江门中微子实验探测器如图所示：  

![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/detector.png)

宇宙线缪子作为主要的本底之一，其次级产物Li9/He8与实验测量的IBD过程的行为非常相似，无法区分，因此缪子径迹的重建非常关键。相比于已有的重建方法，基于深度学习的方法不需要构建复杂的光学模型和进行额外的修正，可作为现有重建方法的补充。主要包括一下几个部分：

## 1. 数据集篇 —— 三军未动，粮草先行 
深度学习是由数据驱动的，数据样本将直接和重建的性能相关。
数据的处理流程如图所示：  

![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/data-aug.png)

1.1 顶部径迹探测器的重建。
   根据宇宙线缪子在JUNO测器的分布，使用产生子产生经过顶部径迹探测器和中心探测器的GenEvent数据。利用产生子数据GenEvent和顶部径迹探测器的缪子模拟算法产生经过顶部径迹探测器的缪子模拟事例SimEvent。然后对模拟数据进行刻度和重建得到重建数据。  
  示例：
*    source sim-tt/sim-gen/Mugen.sh  #批量产生不同随机数的模拟脚本
*    source run-*.sh    #运行模拟脚本
*    source sim-tt/sim-gen/sim.sh  #将产生的模拟数据的所有路径存放在一个文件中，如20190216_muonexe
*    source sim-tt/sim-rec/cal.sh  #将模拟数据转化为刻度数据
*    source sim-tt/sim-rec/rec.sh  #将刻度数据转化为重建数据

1.2 重建事例的挑选。
分析顶部径迹探测器的重建性能，使用挑选方法对顶部探测器重建的数据进行挑选，挑选完成之后保存径迹的信息TrackInfo，包括真实径迹的信息Truth和重建的径迹信息Rec。示例：
* root -l sim-tt/sim-ana/anahit.C  #提取重建数据的信息，包括TrackInfo和挑选信息。文件名称20190216anatemp.root
* root -l sim-nn/sim-cd/root_txt.C  #根据挑选条件保存rec和Truth。文件名称tt-20190216.txt

1.3 中心探测器的模拟。
使用挑选后事例的真实径迹信息Truth,能量200GeV，产生中心探测器的模拟数据。
* source sim-nn/sim-cd/gen_tt200GeV.sh #根据tt-20190216.txt中保存的rec和Truth产生缪子模拟脚本*.sh
* source sim-nn/sim-cd/job-tt-200GeV/0.sh #根据Truth，运行脚本产生模拟数据0_evt_-175.322_-13827.3_25000_0.0415556_0.510892_-0.85864_183785.root

1.4 事例的建模。
经过中心探测器之后，得到中心探测器的模拟数据。将模拟数据进行数据提取和建模之后形成类似图片形式的数据。建模方式有很多种，可以根据需要选择，但需要保证尽量减少PMT的位置信息、电荷数和最快光时间信息的丢失。本人主要使用了等距圆柱投影和球面数据形式。
* source sim-nn/sim-data/cnn.sh   #提取模拟数据中所有PMT的击中信息 输入文件路径list 输出signal-0_evt_-175.322_-13827.3_25000_0.0415556_0.510892_-0.85864_183785.root
* source sim-nn/sim-tfrecod/junonn_tt_root__tfrecords.py #输入为signal-*.root所在的目录，产生evt_1000_1.bin.tfrecords文件
* 2D投影PMT位置图片示例：  
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/2D-convg.png)  

1.5 数据增强-旋转。
将标签为顶部径迹探测器的重建信息Rec的数据集使用缪子事例的增强方法产生旋转后的事例，旋转后的事例覆盖了TT探测器没有覆盖的事例，如图所示：  
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/TT_rot_1.png)  

旋转后的事例入射点和方向的分布基本上覆盖了宇宙线缪子的在中心探测器的入射点和方向的分布。
* python change_tfrecord_fxl2.py  #输入为原始数据，输出为旋转后的数据
* 旋转的方式有很多，但是需要考虑旋转之后需要保持数据的均匀性，采用的旋转方法如图所示：  
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/TT_rot_2.png)  

* 旋转方法的思想是取得良好重建性能的关键，详细的内容可以参考本人的论文：JUNO实验基于深度学习的缪子重建和关联数据分析软件研究

## 2 神经网络篇 —— 谋定而后动，一发制敌
用于分类的卷积神经网络有很多，例如AlexNet、VGGNet、ResNet等，这里主要使用的是VGGNet-I和SCNN-I。如图所示：  
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/TT_rot_2.png) 

2.1 输入层  
输入数据是电荷和时间的双通道的图像。深度学习的输入数据，在进入神经网络进行数据训练之前，一般需要对输
入数据做一些预处理，加快神经网络的收敛。主要采用的预处理方式是将图像中的每一个通道的像素数组进行标准化处理:  
              <center>x'=(x-μ)/σ</center>
其中，X是原始数据，X'是标准化后的数据，μ是X的平均值，σ是X的标准偏差。
* 参考代码：neural-network/train-tt-2D/junonn_inputs_tt.py

2.2 平面卷积神经网络  
VGGNet神经网络是目前最常用的用来提取图像特征的神经网络，多层卷积和池化之后，卷积神经网络逐渐将原始数据转化为局部的特征信号，底层的特征信号转换为抽象的特征信号，抽象特征信号最后转换为预测任务的输出。网络中全连接层(浅层网络节点：1024-512-256-6)的参数数量减少了75％，并且获得了更好的重建性能。最后，使用具有6个节点的全连接的输出层output以获取网络的预测。在输出层之后是损失函数，用于计算预测值和真实值之间的误差。  
* 参考代码：neural-network/train-tt-2D/junonn_inference_vgg16.py  

2.3 输出层径迹参数  
<center>track = (x,y,z,px,py,pz) </center>  
x,y,z是入射点的位置，归一化的单位向量；px,py,pz是径迹的方向，归一化为单位向量。避免单位的影响和训练过程中的偏好  

2.4 损失函数  
损失函数是度量卷积神经网络模型的准确性以及优化的一个重要的目标，通常被用来评估卷积神经网模型的输出值与实际值相接近的程度。在损失函数的构造中除衡量模型预测性能的部分外，一般还会添加一定的正则化惩罚项，以对抗复杂模型导致的过拟合问题。    
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/loss.png)   

* 参考代码：neural-network/train-tt-2D/junonn_loss.py


## 3.神经网络训练 —— 厚积薄发，直捣黄龙
训练过程中需要关注两个超参数，一个是学习率，另一个是批样本大小。其中，学习率的设定非常重要将直接影响到网络的收敛。  
参考代码: neural-network/train-tt-2D/junonn_train.py  
根据实践经验得出的初始学习率为0.1，每50k步学习率降低1/10。批样本大小batch_size为256。训练的曲线为：  
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/loss_vs_step.png)   

训练集的loss和验证集的loss很接近，并且验证集的loss基本没有太大的变化，这表明模型被训练的刚刚好，模型具有良好的泛化能力和学习能力。  
* 网络训练：python neural-network/train-tt-2D/main.py --train_data neural-network/train-tt-2D/ --train_dir neural-network/train-tt-2D/train/ --ckp_dir neural-network/train-tt-2D/train/ --batch_size 128
* 网络验证：python neural-network/train-tt-2D/junonn_eval_reg.py --eval_data neural-network/test-300k/ --eval_dir neural-network/train-tt-2D/eval/ --ckp_dir neural-network/train-tt-2D/train --eval_interval_secs 60 --num_examples 1000 --batch_size 64
* 训练过程可以使用：tensorboard --logdir neural-network/train-tt-2D/train

## 4 性能验证 —— 举一隅而以三隅反
训练好神经网络之后，根据网络模型，重建测试集的缪子事例。  
* 模型的重建能力由验证集的数据展示，通过以下命令获取重建的输出：python neural-network/train-tt-2D/eval_result_to_txt.py --input neural-network/train-tt-2D/test-300k/ --output neural-network/train-tt-2D/test-vgg16.txt  --ckp_dir neural-network/train-tt-2D/train --batch_size 2 --evtmax 20000  

* 重建性能分析，根据重建的径迹和真实的径迹性能之间的差异，评估重建的性能。将txt转化为root，使用root工具分析：root -l sim-ana/ana_txt_root.C  

性能绘图，参考draw/ 目录下的各种绘图函数，例如：  
* root -l angle_mean_sigma.C #角度的均值和分辨率随径迹到球心距离之间的关系  
* root -l dD_mean_sigma.C #真实径迹和重建径迹距离球心的残差的均值和分辨率随径迹到球心距离之间的关系  
* 性能展示如图所示：  
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/angle_rot_MC.png)   
![image](https://github.com/ihep-sft-group/muonRec-DeepLearning/blob/main/images/dD_rot_MC.png)   

## 5 总结 —— 实践是检验真理的唯一标准
不仅要读万卷书，还要行万里路。基于深度学习的缪子重建，影响重建性能的因素有很多，需要我们不断的探索，从理论中来到实践中去。以下有几点建议，尽可斟酌而行：  
* 深度学习是由数据驱动的，数据的重要意义要远高于深度学习的网络本身，所以数据的处理需要谨慎而行。
* 神经网络的结构太多太多，尤其是用于分类的网络，不要迷失在寻找网络的路途中。需要知道的是，别人的网络或许只适用于它自身的数据集。本人给出的建议是选择网络不要太复杂（省时），网络的主要目的是提取特征，不要舍本逐末。
* 本人首次提出了应用于JUNO实验的球面卷积神经网络，这种神经网络和JUNO实验的球形探测器更加契合，理论上可以取得更好的性能。诸君尽可尝试，但希望能够指明出处，毕竟原创不易。球面卷积的所有代码已经公开，诸君可以在目录中查找，代码执行方法和平面卷积类似。
* 神经网络的训练，也就是网络调参方面有这么一个说法-调参即是炼丹，其实本人并不是十分认同。需要了解的是调参的目的是使得网络模型找到更好的局部极小值，根据已有的经验没有必要把大量的时间浪费在调参上，比如，我增加了一批数据优化了多少，我换了一个网络优化了多少。如果确实对调参有兴趣可以试试自动调参工具，只要你有足够多的时间和计算资源。
* 神经网络，换网络固然可能使得性能有所提升，但是没有必要盲目尝试。网络结构千千万万，莫要迷失，需要关注的是网络解决的问题以及数据的形式，它和你的问题和数据是否有类似之处。

诸君且行且珍惜，祝诸君在深度学习的路上一帆风顺，马到功成。











