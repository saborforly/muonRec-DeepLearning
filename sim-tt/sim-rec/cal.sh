#!/usr/bin/env bash

source /junofs/users/liuyan/muon-sim/J17v1r1/trunk.sh
cd /junofs/users/liuyan/muon-sim/sim-tt/sim-rec/

num=${1}
listdir=${2}
#listlist=($(for f in ${listdir}/*; do echo $f; done))
#inputlist=${listlist[10#${num}]}
#echo $inputlist
inputlist=$listdir
prefix=$(basename ${inputlist})

outdir=${prefix}
#output=cal/data/${outdir}/cal_${prefix}.root
logfile=cal/log/${prefix}/log_${prefix}.txt
py="/junofs/users/liuyan/muon-sim/J17v1r1/offline/Examples/Tutorial/share/tut_det2calib_onlyTT.py"

if [ -n "${outdir}" ];then
    if [ ! -e "cal/data/${outdir}" ]; then
        mkdir -p cal/data/${outdir}
    fi

    if [ ! -e "cal/log/${outdir}" ];then
        mkdir -p cal/log/${outdir}
    fi
fi
listfile=$(cat ${inputlist})
for f in ${listfile}
do
echo ${f}
se=$RANDOM
out=$(basename ${f})
output=cal/data/${outdir}/cal-${out}
echo ${out}
(time (python $py --evtmax 3 --seed $se  --detoption TT --output ${output} --input ${f})) >& $logfile
done
