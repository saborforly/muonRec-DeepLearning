#!/usr/bin/env bash

source /junofs/users/liuyan/muon-sim/J17v1r1/trunk.sh
cd /junofs/users/liuyan/muon-sim/sim-tt/sim-rec/

num=${1}
listdir=${2}
#listlist=($(for f in ${listdir}/*; do echo $f; done))
#inputlist=${listlist[10#${num}]}
inputlist=$listdir
#echo $inputlist

prefix=$(basename ${inputlist})

outdir=${prefix}
#output=cal/data/${outdir}/cal_${prefix}.root
logfile=rec/log/${prefix}/log_${prefix}.txt
py="/junofs/users/liuyan/muon-sim/J17v1r1/offline/Examples/Tutorial/share/run_TTTracking2.py"

if [ -n "${outdir}" ];then
    if [ ! -e "rec/data/${outdir}" ]; then
        mkdir -p rec/data/${outdir}
    fi

    if [ ! -e "rec/log/${outdir}" ];then
        mkdir -p rec/log/${outdir}
    fi
fi
listfile=$(cat ${inputlist})
for f in ${listfile}
do
echo ${f}
out=$(basename ${f})
output=rec/data/${outdir}/rec-${out}
echo ${out}
(time (python $py --evtmax 3  --detoption TT --output ${output} --input ${f})) >& $logfile
done
