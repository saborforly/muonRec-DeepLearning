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

void pos_txt_root();

void pos_txt_root()
{
  TString outdir ="/junofs/users/liuyan2016/muon-sim/sim-nn/data-draw/draw/";
  ifstream fin;
  //cout<<fname<<endl;
  
  fin.open("/junofs/users/liuyan2016/muon-sim/sim-nn/data-draw/draw/pos_cd.txt");
  string str;

  TString outname =outdir+"pos_cd.root";
  TFile* outf = new TFile(outname,"recreate");
  TTree* anatree = new TTree("ana","ana of rec");
  double t_genDist,t_recDist,t_gencdtrack;
  double t_angle;
  double t_gensTheta,t_gensPhi,t_gendTheta,t_gendPhi;
  double t_recsTheta,t_recsPhi,t_recdTheta,t_recdPhi;
  double t_genix,t_geniy,t_geniz,t_genit;
  double t_recix,t_reciy,t_reciz,t_recit;
  double t_genpx,t_genpy,t_genpz;
  double t_recpx,t_recpy,t_recpz;
  double null;
  anatree->Branch("genDist",&t_genDist);

  anatree->Branch("gencdtrack",&t_gencdtrack);  
  anatree->Branch("genstheta",&t_gensTheta);
  anatree->Branch("gensphi",&t_gensPhi);

  anatree->Branch("gendtheta",&t_gendTheta);
  anatree->Branch("gendphi",&t_gendPhi);

  anatree->Branch("genix",&t_genix);
  anatree->Branch("geniy",&t_geniy);
  anatree->Branch("geniz",&t_geniz);
  anatree->Branch("genpx",&t_genpx);
  anatree->Branch("genpy",&t_genpy);
  anatree->Branch("genpz",&t_genpz);
  while (!fin.eof()){
    getline(fin, str);
    istringstream strm(str);
    strm>>null;
    strm>>null; 
    strm>>null; 
    strm>>null; 
    strm>>t_genpx; 
    strm>>t_genpy;
    strm>>t_genpz;
    strm>>null;   
    strm>>null;             
    strm>>t_genix;              
    strm>>t_geniy;
    strm>>t_geniz;               
    //cout<<t_angle<<endl;
    TVector3 gs(t_genix, t_geniy, t_geniz); 
    TVector3 gdir(t_genpx, t_genpy, t_genpz); 
    gdir = gdir.Unit();

    double m_LSRadius = 17700;
    TVector3 reccentrk1 = gs + (-gs*gdir)*gdir;

    double t_genDist = (gs.Cross(gdir)).Mag();// dist from track center to LS center
    if(t_genDist>m_LSRadius){
        continue;
    }
    std::cout << "########### dist1  ################: " <<t_genDist<< std::endl;
    
    TVector3 rr1 = reccentrk1 - TMath::Sqrt(m_LSRadius*m_LSRadius - t_genDist*t_genDist)*gdir;

    

    t_gendTheta = gdir.Theta()/TMath::Pi()*180;
    t_gendPhi = gdir.Phi()/TMath::Pi()*180;

    t_gensTheta = rr1.Theta()/TMath::Pi()*180;
    t_gensPhi = rr1.Phi()/TMath::Pi()*180;
    t_genix = rr1.X();
    t_geniy = rr1.Y(); 
    t_geniz = rr1.Z();
    
    Double_t t_gencdtrack = 2*TMath::Sqrt(17700*17700 - t_genDist*t_genDist);

    //t_dTheta = (rdir.Theta()-gdir.Theta())/TMath::Pi()*180;
    //t_dPhi = (rdir.Phi()-gdir.Phi())/TMath::Pi()*180;
    anatree->Fill();
  }
  outf->Write();
  outf->Close();
   
}
