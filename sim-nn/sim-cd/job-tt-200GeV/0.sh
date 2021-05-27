#!/bin/bash
source /junofs/users/liuyan2016/muon-sim/J16v1r4/trunk.sh
cd /junofs/users/liuyan2016/muon-sim/sim-nn/data-cd/jobs-tt-200GeV/fullsim_/tt-2019_evtmax1_mom_tts3ns/data_1
ProcID=$1
time (python /junofs/users/liuyan2016/muon-sim/J16v1r4/offline/Examples/Tutorial/share/tut_detsim.py --evtmax 1 --seed $(($ProcID+36690)) --output 0_evt_4033.88_-1563.02_25000_-0.12227_-0.00873444_-0.992458_170915.root --user-output user_0.root --no-gdml --no-anamgr-normal --no-anamgr-deposit --no-anamgr-genevt --no-anamgr-interesting-process --disable-struts-fastens all --pmtsd-v2 --pmt-hit-type 2 --pmtsd-merge-twindow 1.0 gun --particles mu- --positions 4023.62 -1564.46 25000 --directions -0.122465 -0.0126402 -0.992392 --momentums 200000) 
