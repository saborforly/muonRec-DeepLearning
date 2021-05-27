#!/usr/bin/python
# -*- coding:utf-8 -*-
#
# Author: ZHANG Kun - zhangkun@ihep.ac.cn
# Last modified:	2015-05-11 05:20
# Filename:		test.py
# Description: 
def get_parser():

    import argparse

    parser = argparse.ArgumentParser(description='Run Atmospheric Simulation.')
    parser.add_argument("--evtmax", type=int, default=-1, help='events to be processed')
    parser.add_argument("--input", default=["sample_muonsim.root"],nargs='+',  help="input file name list")
    parser.add_argument("--output", default="signals.root", help="output file name")
    parser.add_argument("--loglvl", type=int, default=2, help="log level: 2:debug, 3:info, 4:warn, 5:error, 6:fatal")
    return parser

if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()

    import Sniper
    Sniper.setLogLevel(args.loglvl)
    task = Sniper.Task("task")
    task.asTop()
    task.setLogLevel(args.loglvl)
    
    import BufferMemMgr
    bufMgr = task.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([0, 0]);
    
    import DataRegistritionSvc
    task.property("svcs").append("DataRegistritionSvc")
    Sniper.loadDll("libSimEventV2.so")
    
    import RootIOSvc
    riSvc = task.createSvc("RootInputSvc/InputSvc")
    riSvc.property("InputFile").set(args.input)
    
    import RootWriter
    rw = task.createSvc("RootWriter")
    output = {"DLDataReady":args.output}
    rw.property("Output").set(output)

    import Geometry
    geom = task.createSvc("RecGeomSvc")
    geom.property("GeomFile").set("/afs/ihep.ac.cn/users/z/zhangk/junofs/muon-rec/resources/geom.root")
    geom.property("FastInit").set(True)
    
    Sniper.loadDll("libPmtRec.so")
    task.property("algs").append("PullSimHeaderAlg")
    
    import RecCdMuonAlg
    import DLDataReadyTool
    recalg = RecCdMuonAlg.createAlg(task)
    recalg.setLogLevel(args.loglvl)
    recalg.property("Use3inchPMT").set(True)
    recalg.useRecTool("DLDataReadyTool")
    
    
    task.setEvtMax(args.evtmax)
    task.show()
    task.run()
