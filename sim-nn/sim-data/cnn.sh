#!/usr/bin/env bash

source /junofs/users/liuyan/muon-sim/J16v1r4/trunk.sh
cd /junofs/users/liuyan/muon-sim/sim-nn/data-nn/data-cd-randGeV/
#cd /junofs/users/yiph/liuyan/
num=${1}
listdir=${2}
#listdir="/junofs/users/liuyan/muon-sim/sim-nn/data-nn/data-cd-200GeV/flist/ellip20190328"
#listlist=($(for f in ${listdir}/*; do echo $f; done))
#inputlist=${listlist[10#${num}]}
inputlist=${listdir}
echo $listdir

prefix=$(basename ${inputlist})
outdir=${prefix}
datadir=$prefix
logdir=log
#output=cal/data/${outdir}/cal_${prefix}.root
#logfile=log/log_1/log_${prefix}.txt
py="/junofs/users/liuyan/muon-sim/sim-nn/data-nn/dummy.py"

if [ -n "${outdir}" ];then
    if [ ! -e "$datadir" ]; then
        mkdir -p $datadir
    fi

    if [ ! -e "$logdir" ];then
        mkdir -p $logdir
    fi
fi
listfile=$(cat ${inputlist})

for f in ${listfile}
do
echo ${f}
se=$RANDOM
out=$(basename ${f})
output=${datadir}/signal-${out}
logfile=${datadir}/log.txt
echo ${out}
time (python $py  --output ${output} --input ${f})
done
