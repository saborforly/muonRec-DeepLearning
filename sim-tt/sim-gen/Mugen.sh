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
    echo /junofs/users/liuyan/muon-sim/sim-tt/sim-gen/full-sim-muongen/jobs
}
# == setup script ==
juno-reg juno-gen-user-setup-script
function juno-gen-user-setup-script () {
    echo /junofs/users/liuyan/muon-sim/J17v1r1/trunk.sh
}

# == memory usage ==
juno-reg juno-gen-user-memory-usage
function juno-gen-user-memory-usage() {
    echo /workfs/bes/lint/ihep-code/dotenv/inter.d/jobmom.sh
}

# == detsim related script ==
juno-reg juno-gen-user-detsim-script
function juno-gen-user-detsim-script () {
    echo /junofs/users/liuyan/muon-sim/J17v1r1/offline/Examples/Tutorial/share/run_detsim_tt.py
}


juno-reg juno-gen-user-detsim-evtmax
function juno-gen-user-detsim-evtmax(){
    echo ${EVTMAX:=3}
}

juno-reg juno-gen-user-detsim-jobnum
function juno-gen-user-detsim-jobnum(){
    echo ${JOBNUM:=10000}
}

#juno-reg juno-gen-user-detsim-pmttts
#function juno-gen-user-detsim-pmttts(){
#    echo ${TTS:=3ns}
#}
juno-reg juno-gen-user-detsim-genmom
function juno-gen-user-detsim-genmom(){
    echo ${GENMOM:=200GeV}
}

juno-reg juno-gen-user-detsim-tag
function juno-gen-user-detsim-tag (){
    echo ${TAG:=$(date '+%Y%m%d')}
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
        echo 10 > $GEN_JOB_TOP/seed
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
#    local _tts=$(juno-gen-user-detsim-pmttts)
    echo ${JOBDIR:=${_simtype}_${_tag}__muonexe}
}
juno-reg juno-gen-user-job-userdir
function juno-gen-user-job-userdir(){
    local _mom=$(juno-gen-user-detsim-genmom)
    local _tag=$(juno-gen-user-detsim-tag)
    local _simtype=$(juno-gen-user-detsim-type)
#    local _tts=$(juno-gen-user-detsim-pmttts)
    echo ${USERDIR:=${_simtype}_${_tag}__useroutput}
}


function juno-gen-user-job-mkdir(){
    local _dir=$(juno-gen-user-job-dir)
    local _top=$(juno-gen-user-top-dir)
    local _userdir=$(juno-gen-user-job-userdir)
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

    if [ -d "$_top/$_userdir" ]; then
        return
    elif [ -e "$_top/$_userdir" ]; then
        juno-info "[$_top/$_userdir] has existed and is not a directory!"
        return
    else
        juno-info "Creating job directory [$_top/$_userdir]."
        mkdir -p "$_top/$_userdir"
    fi

}

function juno-gen-detsim-output() {
    local seed=$1; shift
    local evtmax=${1:-$(juno-gen-detsim-${XXX}-evtmax)}; shift
    local prefix=${1:-evt}; shift
    local suffix=${1:-root}; shift

    echo ${prefix}-${seed}-${evtmax}.${suffix}
}

function juno-gen-detsim-useroutput() {
    local seed=$1; shift
    local evtmax=${1:-$(juno-gen-detsim-${XXX}-evtmax)}; shift
    local prefix=${1:-user}; shift
    local preevt=${1:-evt}; shift
    local suffix=${1:-root}; shift

    echo ${prefix}-${preevt}-${seed}-${evtmax}.${suffix}
}

function juno-gen-user-detsim-cmd(){
    local seed=$1;shift
    local evtmax=${1:-$(juno-gen-user-detsim-evtmax)};shift
    local script=$(juno-gen-user-detsim-script)
    local output=$(juno-gen-detsim-output $seed $evtmax evt root)
    local mom=$(juno-gen-user-detsim-genmom)
    local useroutput=$(juno-gen-detsim-useroutput $seed $evtmax user evt root)
    local _top=$(juno-gen-user-top-dir)
    local _userdir=$(juno-gen-user-job-userdir)
#    local _tts=$(juno-gen-user-detsim-pmttts)
#    _tts=${_tts/s/*10.0^9}
#    _tts=${_tts/m/*1000.0u}
#    _tts=${_tts/u/*1000.0n}
#    _tts=${_tts/n/*10.0^(-9)}
#    _tts=$(echo $_tts | bc -l)


#    --pmt-hit-tts $_tts \
    echo python $script --evtmax $evtmax \
                        --seed $seed    \
                        --output $output \
                        --user-output $_top/$_userdir/$useroutput \
                        --detoption TT \
                        hepevt --exe Muon 
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
    pushd "$_top/$_dir" >& /dev/null

    local seedbase=$(gen-seed)
    local seed="\$((\$ProcID+${seedbase}))"
    local evtmax=${1:-$(juno-gen-user-detsim-evtmax)}; shift
    local prefix=${1:-}; shift
    juno-debug  SEEDBASE: $seedbase
    local cmd=$(juno-gen-user-detsim-cmd $seed $evtmax)
    local script=$(juno-gen-detsim-output $seedbase $evtmax ${prefix}run sh)
    local log=$(juno-gen-detsim-output $seed $evtmax ${prefix}log txt)
    juno-debug cmd: $cmd
    juno-debug script: $script
    juno-debug log: $log

    if [ -e "$script" ]; then
        juno-info "[$script] has existed!"
        return
    fi
    cat << JOBEOF > $script
#!/bin/bash
source $(juno-gen-user-setup-script)
cd `pwd`
ProcID=\$1
$(juno-gen-user-memory-usage) \$\$ &
(time $cmd) 
JOBEOF

    local jobnum=${1:-$(juno-gen-user-detsim-jobnum)}
    local condor_script=$(juno-gen-detsim-output $seedbase $evtmax ${prefix}condor desc)
    juno-debug condor_script: $condor_script

    if [ -e "$condor_script" ]; then
        juno-info "[$condor_script] has existed!"
        return
    fi
    cat << CONDOREOF > ${condor_script}
Universe = vanilla
Executable = $_top/$_dir/${script}
Requirements = Target.OpSysAndVer =?= "SL6"
Accounting_Group = juno
Args=\$(Process)
Queue $jobnum
CONDOREOF
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
$*
