#!/bin/bash

export LANG=en_US.UTF-8
#########################
#########################
# ====== Base ===========
#########################
function Function(){
    local _func=$1
    local _content=$2
    eval "$_func() { $_content }"
    juno-reg $_func
}

#DEBUG=0
function juno-debug() {
    if [ -z "$DEBUG" ]; then
        return;
    fi
    echo 1>&2 ${debug:-===} $*
}

function juno-info() {
    echo 1>&2 ${msg:->>>} $*
}

function comment() {
    echo
}

function juno-list(){
    echo 
}
function juno-reg(){
    local lfunc=$1;
    local msg=" $FUNCNAME >>> "
    local debug=" $FUNCNAME ==="
    juno-debug $lfunc
    if [ -z "$lfunc" ]; then
        juno-info "Please set the function name to juno-reg()"
        return
    fi
    for func in $(juno-list)
    do
        if [ "$func" == "$lfunc" ]
        then
            return 0
        fi
    done
    eval "function juno-list(){
        echo $(juno-list) $lfunc
    }"
}
function juno-gen-help () {
cat << EOF
* User Note:
  + modify the top-dir and setup-script
  + source this script

  + create directory hierarchy
  + generate job
  + submit job

* Developer Note:
  * The function defined here should have the prefix 'juno-gen-'

EOF
}

#########################
#########################
# ====== juno list ======
#########################

# == job top directory ==
juno-reg juno-gen-user-top-dir
function juno-gen-user-top-dir () {
    echo /junofs/users/liuyan/muon-sim/sim-nn/data-cd/jobs-tt-200GeV
}
# == setup script ==
juno-reg juno-gen-user-setup-script
function juno-gen-user-setup-script () {
    echo /junofs/users/liuyan/muon-sim/J16v1r4/trunk.sh
}

# == memory usage ==
juno-reg juno-gen-user-memory-usage
function juno-gen-user-memory-usage() {
    echo /workfs/bes/lint/ihep-code/dotenv/inter.d/jobmom.sh
}

# == detsim related script ==
juno-reg juno-gen-user-detsim-script
function juno-gen-user-detsim-script () {
    echo /junofs/users/liuyan/muon-sim/J16v1r4/offline/Examples/Tutorial/share/tut_detsim.py
}

juno-reg juno-gen-user-input
function juno-gen-user-input () {
    echo /junofs/users/liuyan/muon-sim/sim-nn/data-cd/tt-20190130.txt
}

juno-reg juno-gen-user-detsim-evtmax
function juno-gen-user-detsim-evtmax(){
    echo ${EVTMAX:=1}
}

juno-reg juno-gen-user-detsim-jobnum
function juno-gen-user-detsim-jobnum(){
    echo ${JOBNUM:=1000}
}

juno-reg juno-gen-user-detsim-pmttts
function juno-gen-user-detsim-pmttts(){
    echo ${TTS:=3ns}
}

juno-reg juno-gen-user-detsim-genmom
function juno-gen-user-detsim-genmom(){
    echo ${GENMOM:=200GeV}
}

juno-reg juno-gen-user-detsim-tag
function juno-gen-user-detsim-tag (){
    local date_input=$(juno-gen-user-input)
    echo ${TAG:=${date_input:48:8}}
    #echo ${TAG:=$(date '+%Y%m%d')}
}

juno-reg juno-gen-user-detsim-type
function juno-gen-user-detsim-type () {
    local simtype=$(echo ${SIMTYPE:="full"} |tr 'A-Z' 'a-z')
    if [ "$simtype" == "full" ] \
        || [ "$simtype" == "fullsim" ] \
        || [ "$simtype" == "0" ] ; then
        echo "fullsim"
    elif [ "$simtype" == "toy" ] \
        || [ "$simtype" == "toysim" ] \
        || [ "$simtype" == "toymc" ] \
        || [ "$simtype" == "1" ] ; then
        echo "toysim"
    else
        echo "null"
    fi
}

function juno-gen-user-detsim-toysim(){
    local simtype=$(juno-gen-user-detsim-type)
    if [ "$simtype" == "toysim" ]; then
        echo $*
    fi
}

# == seed related ==
function gen-seed () {
    local GEN_JOB_TOP=$(juno-gen-user-top-dir)

    # temporary seed number
    if [ -n "$SEEDSTART" ]; then
        juno-debug using seed $SEEDSTART
        echo $SEEDSTART
        ((SEEDSTART++))
        return
    fi
    
    if [ ! -f $GEN_JOB_TOP/seed ];
    then
        # create initial seed
        echo 0 > $GEN_JOB_TOP/seed
    fi

    export GEN_SEED=$(cat $GEN_JOB_TOP/seed)
    echo $GEN_SEED
    export GEN_SEED=$(($GEN_SEED+$(juno-gen-user-detsim-jobnum)))
    echo $GEN_SEED > $GEN_JOB_TOP/seed
}

juno-reg juno-gen-user-job-dir
function juno-gen-user-job-dir(){
    local _mom=$(juno-gen-user-detsim-genmom)
    local _tag=$(juno-gen-user-detsim-tag)
    local _simtype=$(juno-gen-user-detsim-type)
    local _tts=$(juno-gen-user-detsim-pmttts)
    local _evtmax=$(juno-gen-user-detsim-evtmax)   
    echo ${JOBDIR:=${_simtype}_${_tag}_evtmax${_evtmax}_mom_tts${_tts}}
}

function juno-gen-user-job-mkdir(){
    local _dir=$(juno-gen-user-job-dir)
    local _top=$(juno-gen-user-top-dir)

    if [ ! -d "$_top" ]; then
        juno-info "[$_top] does not exist! Please set correct path!"
        return
    fi
    
    if [ -d "$_top/$_dir" ]; then
        return
    elif [ -e "$_top/$_dir" ]; then
        juno-info "[$_top/$_dir] has existed and is not a directory!"
        return
    else
        juno-info "Creating job directory [$_top/$_dir]."
        mkdir -p "$_top/$_dir"
    fi
    
    if [ ! -d "$_top/$_dir" ]; then
        juno-info "Failed to create job directory [$_top/$_dir]?"
        return
    fi
}


function juno-gen-user-detsim-cmd(){
    local gen_x=$1;shift
    local gen_y=$1;shift
    local gen_z=$1;shift
    local gen_dirx=$1;shift
    local gen_diry=$1;shift
    local gen_dirz=$1;shift
    local gen_ene=$1;shift
    local output=$1;shift
    local useroutput=$1;shift
    local seed=$1;shift
    local evtmax=${1:-$(juno-gen-user-detsim-evtmax)};shift
    local script=$(juno-gen-user-detsim-script)
    local mom=$(juno-gen-user-detsim-genmom)
    mom=${mom/T/*1000G}
    mom=${mom/G/*1000M}
    mom=${mom/MeV/}
    mom=$(echo $mom | bc)
 
    local _tts=$(juno-gen-user-detsim-pmttts)
    _tts=${_tts/s/*10.0^9}
    _tts=${_tts/m/*1000.0u}
    _tts=${_tts/u/*1000.0n}
    _tts=${_tts/n/*10.0^(-9)}
    _tts=$(echo $_tts | bc -l)


    echo python $script --evtmax $evtmax \
                        --seed $seed    \
                        --output $output \
                        --user-output $useroutput \
                        --no-gdml       \
                        --no-anamgr-normal \
                        --no-anamgr-deposit \
                        --no-anamgr-genevt \
                        --no-anamgr-interesting-process \
                        $(juno-gen-user-detsim-toysim --no-optical) \
                        $(juno-gen-user-detsim-toysim --anamgr-list MuonToySim) \
                        --disable-struts-fastens all \
                        --pmtsd-v2  \
                        --pmt-hit-type 2 \
                        --pmtsd-merge-twindow 1.0 \
                        gun --particles mu- \
                            --positions $gen_x  $gen_y  $gen_z \
                            --directions $gen_dirx $gen_diry $gen_dirz\
                            --momentums $mom
}

function juno-gen-detsim-outsh() {
    local seed=$1; shift
    local evtmax=${1:-$(juno-gen-detsim-${XXX}-evtmax)}; shift
    local prefix=${1:-evt}; shift
    local suffix=${1:-root}; shift

    echo ${prefix}-${seed}-${evtmax}.${suffix}
}

 # == job template ==
function juno-gen-user-job-script () {
    local msg=" $FUNCNAME >>> "
    local debug=" $FUNCNAME === " 
    local _dir=$(juno-gen-user-job-dir)
    local _top=$(juno-gen-user-top-dir)
    if [ ! -d "$_top/$_dir" ]; then
        juno-gen-user-job-mkdir
    fi

    for i in  $(seq 12)
    do
      if [ ! -d "$_top/$_dir/script_$i" ]; then                                         
          juno-info "Creating job directory [$_top/$_dir/script_$i]."                   
          mkdir -p "$_top/$_dir/script_$i"                                              
      fi 

 
      if [ ! -d "$_top/$_dir/data_$i" ]; then
          juno-info "Creating job directory [$_top/$_dir/data_$i]."
          mkdir -p "$_top/$_dir/data_$i"
      fi
      if [ -e "$_top/$_dir/data_$i" ]; then
          juno-info "[$_top/$_dir/data_$i] has existed and is not a directory!"
      fi
    done
    
    local seedbase=$(gen-seed)
    local seed="\$((\$ProcID+${seedbase}))"
    local evtmax=${1:-$(juno-gen-user-detsim-evtmax)}; shift
    local prefix=${1:-}; shift
    juno-debug  SEEDBASE: $seedbase


    local num=-1
    local list=$(juno-gen-user-input)
    cat ${list} |while read line; 
    do
      #for f in ${line2}; 
      #do echo $f;   
      #done; break;
      num=$[num+1] 
      if (($num>12000));then
         break   
      fi
      da_num=`expr $num / 1000`
      da_num=$[da_num+1]
      s_num=`expr $num % 1000 `
 
      gen=($(for f in ${line}; do echo ${f}; done)) 
      echo ${gen[@]}
      local output=${s_num}_evt_${gen[7]}_${gen[8]}_${gen[9]}_${gen[10]}_${gen[11]}_${gen[12]}_${gen[6]}.root
      local useroutput=user_${s_num}.root

      local cmd=$(juno-gen-user-detsim-cmd ${gen[0]} ${gen[1]} ${gen[2]} ${gen[3]} ${gen[4]} ${gen[5]} ${gen[6]} ${output} ${useroutput} $seed $evtmax)
      juno-debug cmd: $cmd
      juno-debug script: $script
      
      pushd "$_top/$_dir/script_${da_num}" >& /dev/null
      local script=${s_num}.sh
      local log=${s_num}_log.txt
      juno-debug log: $log
      #$(juno-gen-user-memory-usage) \$\$ >& ${log}.mem.usage &(time $cmd) >& $log )
      #hostname >> $log
      cat << JOBEOF > $script
#!/bin/bash
source $(juno-gen-user-setup-script)
cd $_top/$_dir/data_${da_num}
ProcID=\$1
time ($cmd) 
JOBEOF
      popd >& /dev/null 
    done
    
    local jobnum=${1:-$(juno-gen-user-detsim-jobnum)}
    local condor_script=condor-${jobnum}.desc
    #juno-debug condor_script: $condor_script

    
    for i in $(seq 12)
    do
      echo ${condor_script}
      pushd "$_top/$_dir/script_$i" >& /dev/null
      cat << CONDOREOF > ${condor_script}
Universe = vanilla
Executable = $_top/$_dir/script_$i/\$(Process).sh
Requirements = Target.OpSysAndVer =?= "SL6" && !regexp("bws0441", Name)
Accounting_Group = juno
Args=\$(Process)
Queue $jobnum
CONDOREOF
      popd >& /dev/null
    done  
    popd >& /dev/null
}

function juno-print(){
    for func in $(juno-list);do
        printf "%-30s    ---    %s\n" $func "$($func)"
    done |sort 
}

function gen(){
    juno-print
    juno-gen-user-job-script
}

function sub(){
    local _dir=$(juno-gen-user-job-dir)
    local _top=$(juno-gen-user-top-dir)
    if [ ! -d "$_top/$_dir" ]; then
        juno-info "[$_top/$_dir] does not exist!"
        return
    fi
    pushd "$_top/$_dir" >& /dev/null
    echo "$_top/$_dir"/condor*.desc
    echo "which job to submit? (Enter 0 to end)"
    select job in "$_top/$_dir"/condor*.desc; do
    if [ "$job" ];then
        condor_submit $job
    else
        break;
    fi
    done
    popd >& /dev/null
}
gen
#sub
$*
