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

void ana_txt_root();

void ana_txt_root_energy()
{
  TString outdir ="/junofs/users/liuyan/muon-sim/sim-nn/data-ana/";
  ifstream fin;
  cout<<outdir<<endl;
  
  fin.open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-rotrand-40k-fxl2-300k-vgg16-randGeV.txt");
  string str;

  TString outname =outdir+"anasim-tt-rotrand-40k-fxl2-300k-vgg16-randGeV-new.root";
  TFile* outf = new TFile(outname,"recreate");
  TTree* anatree = new TTree("ana","ana of rec");
  double t_genDist,t_recDist,t_gencdtrack;
  double t_angle;
  double t_dsangle;

  double t_gensTheta,t_gensPhi,t_gendTheta,t_gendPhi;
  double t_recsTheta,t_recsPhi,t_recdTheta,t_recdPhi;
  double t_genix,t_geniy,t_geniz,t_genit;
  double t_recix,t_reciy,t_reciz,t_recit;
  double t_genpx,t_genpy,t_genpz;
  double t_recpx,t_recpy,t_recpz;
  double t_energy,t_depenergy;
  anatree->Branch("genDist",&t_genDist);
  anatree->Branch("recDist",&t_recDist);
  anatree->Branch("angle",&t_angle);
  anatree->Branch("energy",&t_energy);
  anatree->Branch("dsangle",&t_dsangle);
 
  anatree->Branch("gencdtrack",&t_gencdtrack);  
  anatree->Branch("depenergy",&t_depenergy);
  anatree->Branch("genstheta",&t_gensTheta);
  anatree->Branch("gensphi",&t_gensPhi);
  anatree->Branch("gendtheta",&t_gendTheta);
  anatree->Branch("gendphi",&t_gendPhi);

  anatree->Branch("recstheta",&t_recsTheta);
  anatree->Branch("recsphi",&t_recsPhi);
  anatree->Branch("recdtheta",&t_recdTheta);
  anatree->Branch("recdphi",&t_recdPhi);

  anatree->Branch("genix",&t_genix);
  anatree->Branch("geniy",&t_geniy);
  anatree->Branch("geniz",&t_geniz);
  anatree->Branch("recix",&t_recix);
  anatree->Branch("reciy",&t_reciy);
  anatree->Branch("reciz",&t_reciz);
  anatree->Branch("genpx",&t_genpx);
  anatree->Branch("genpy",&t_genpy);
  anatree->Branch("genpz",&t_genpz);
  anatree->Branch("recpx",&t_recpx);
  anatree->Branch("recpy",&t_recpy);
  anatree->Branch("recpz",&t_recpz);
  cout<<"ready: "<<endl;
  while (!fin.eof()){
    getline(fin, str);
    istringstream strm(str);
    strm>>t_genix;
    strm>>t_geniy; 
    strm>>t_geniz; 
    strm>>t_genpx; 
    strm>>t_genpy; 
    strm>>t_genpz;
    strm>>t_recix;             
    strm>>t_reciy;              
    strm>>t_reciz;
    strm>>t_recpx;               
    strm>>t_recpy;               
    strm>>t_recpz; 
    strm>>t_angle;
    strm>>t_energy;
    //cout<<t_angle<<endl;
    TVector3 gs(t_genix, t_geniy, t_geniz); 
    TVector3 gdir(t_genpx, t_genpy, t_genpz); 
    gdir = gdir.Unit();

    t_dsangle = (-gs).Angle(gdir);

    TVector3 gtp=gs.Unit();
    t_gensTheta = gtp.Theta()/TMath::Pi()*180;
    t_gensPhi = gtp.Phi()/TMath::Pi()*180;
    t_gendTheta = gdir.Theta()/TMath::Pi()*180;
    t_gendPhi = gdir.Phi()/TMath::Pi()*180;
    
    TVector3 rs(t_recix, t_reciy, t_reciz);
    TVector3 rdir(t_recpx, t_recpy, t_recpz);
    rdir = rdir.Unit();
    TVector3 rtp=rs.Unit();
    t_recsTheta = rtp.Theta()/TMath::Pi()*180;
    t_recsPhi = rtp.Phi()/TMath::Pi()*180;
    t_recdTheta = rdir.Theta()/TMath::Pi()*180;
    t_recdPhi = rdir.Phi()/TMath::Pi()*180;

    //TVector3 gcentrk = gs + (-gs*gdir)*gdir;
    //double dist_cc = (gs.Cross(gdir)).Mag();// dist from track center to LS center
    //TVector3 gr0 = gcentrk - TMath::Sqrt(m_LSRadius*m_LSRadius - dist_cc*dist_cc)*gdir;
    
    Double_t t_genDist = gdir.Cross(gs).Mag()/gdir.Mag();
    cout<<t_genDist<<endl; 
    Double_t t_recDist = rdir.Cross(rs).Mag()/rdir.Mag();
    
    Double_t t_gencdtrack = 2*TMath::Sqrt(17700*17700 - t_genDist*t_genDist);
    
    Double_t t_depenergy = 3*t_gencdtrack/10;
    
    //t_dTheta = (rdir.Theta()-gdir.Theta())/TMath::Pi()*180;
    //t_dPhi = (rdir.Phi()-gdir.Phi())/TMath::Pi()*180;
    anatree->Fill();
  }
  outf->Write();
  outf->Close();
   
}
