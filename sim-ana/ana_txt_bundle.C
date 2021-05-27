#include <TFile.h>
#include <TTree.h>
#include <algorithm>
#include <vector>
#include <TVector3.h>
#include <TSystem.h>
#include <TMath.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
const double m_LSRadius = 17700;

void ana_txt_bundle();

void ana_txt_bundle()
{
  TString outdir ="/junofs/users/liuyan/muon-sim/sim-nn/data-ana/";
  ifstream fin;
  //cout<<fname<<endl;
  
  fin.open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/ana-cd-300k-bundle_two.txt");
  string str;

  TString outname =outdir+"anasim-cd-300k-bundle-2.root";
  TFile* outf = new TFile(outname,"recreate");
  TTree* anatree = new TTree("ana","ana of rec");
  double t_dist,t_true,t_pre;
  //double t_genix,t_geniy,t_geniz,t_genit;
  //double t_genpx,t_genpy,t_genpz;
  anatree->Branch("dist_two",&t_dist);

 
  anatree->Branch("true",&t_true);
  anatree->Branch("pre",&t_pre);

  //anatree->Branch("genix",&t_genix);
  //anatree->Branch("geniy",&t_geniy);
  //anatree->Branch("geniz",&t_geniz);
  //anatree->Branch("genpx",&t_genpx);
  //anatree->Branch("genpy",&t_genpy);
  //anatree->Branch("genpz",&t_genpz);
  cout<<"ready: "<<endl;
  while (!fin.eof()){
    getline(fin, str);
    istringstream strm(str);
    strm>>t_true;
    strm>>t_pre; 
    strm>>t_dist; 
    //cout<<t_angle<<endl;
    /*
    TVector3 gs(t_genix, t_geniy, t_geniz); 
    TVector3 gdir(t_genpx, t_genpy, t_genpz); 
    gdir = gdir.Unit();
    TVector3 gtp=gs.Unit();
    */
    anatree->Fill();
  }
  outf->Write();
  outf->Close();
   
}
