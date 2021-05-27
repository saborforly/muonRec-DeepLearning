#!/bin/bash - 
#===============================================================================
#
#          FILE: jobmom.sh
# 
#         USAGE: jobmom.sh pid
# 
#   DESCRIPTION: Monitor the memory usage of the child process of $pid.
#                The pid can be a value $$ (caller's pid)
#                
#                Example in a job script:
#                  jobmom.sh $$ >& log.mem.usage &
#                  (time python job.py) >& log
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Tao Lin (lintao@ihep.ac.cn), 
#  ORGANIZATION: 
#       CREATED: 08/28/2015 22:41
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

function debug () {
    echo 1>&2 $*
}

function get-child() {                                                          
    debug $FUNCNAME
    local parent=$1                                                             
    local list=                                                                 
    local child=$(ps --ppid $parent -o pid h)                                   
    for c in $child;                                                            
    do                                                                          
        # check the child again                                                 
        if ps --pid $c -o pid h >& /dev/null; then                              
            echo $c                                                             
            get-child $c                                                        
        fi                                                                      
    done                                                                        
}                                                                               
                                                                                
function filter-child() {                                                       
    debug $FUNCNAME
    for c in $*                                                                 
    do                                                                          
        if [ -z "$c" ]; then
            continue
        fi
        if (ps --pid $c -o args h | grep -v bash | grep -v grep) >& /dev/null ;                  
        then                                                                    
            echo $c                                                             
        fi                                                                      
    done                                                                        
}                                                                               
                                                                                
function mom() {                                                                
    debug $FUNCNAME
    if [ "$#" -eq "0" ]; then
        debug please input a pid
        return
    fi
    local parent=$1                                                             
    if [ -z "$parent" ]; then
        debug please input a pid
        return
    fi
    sleep 1                                                                     
    echo parent: $parent                                                        
    local child=$(get-child $parent)                                            
    child=$(filter-child $child)                                                
    echo child: $child                                                          
    if [ -z "$child" ]; then
        debug can not find any child
        return
    fi
                                                                                
    while ps --pid $child -o vsz,rss h;
    do                                                                          
        sleep 5                                                                 
    done                                                                        
}                                                                               

mom $*
