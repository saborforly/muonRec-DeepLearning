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

void rotrand_test();

void rotrand_test()
{
  TString outdir ="/junofs/users/liuyan/pmt_info/";
  ifstream fin;
  //cout<<fname<<endl;
  
  fin.open("/junofs/users/liuyan/pmt_info/rot_30k_cd.txt");
  string str;

  TString outname =outdir+"rot_30k_cd.root";
  TFile* outf = new TFile(outname,"recreate");
  TTree* anatree = new TTree("ana","ana of rec");
  double t_genDist,t_recDist,t_gencdtrack;
  double t_angle;
  double t_gensTheta,t_gensPhi,t_gendTheta,t_gendPhi;
  double thes,phis,thep,phip;
  double t_genix,t_geniy,t_geniz,t_genit;
  double t_genpx,t_genpy,t_genpz;
  double t_cos_sTheta,t_cos_dTheta;
  anatree->Branch("genDist",&t_genDist);

  anatree->Branch("gencdtrack",&t_gencdtrack);  
 
  anatree->Branch("genstheta",&t_gensTheta);
  anatree->Branch("cos_stheta",&t_cos_sTheta);
  anatree->Branch("gensphi",&t_gensPhi);
  anatree->Branch("gendtheta",&t_gendTheta);
  anatree->Branch("cos_dtheta",&t_cos_dTheta);
  anatree->Branch("gendphi",&t_gendPhi);

  anatree->Branch("genix",&t_genix);
  anatree->Branch("geniy",&t_geniy);
  anatree->Branch("geniz",&t_geniz);
  anatree->Branch("genpx",&t_genpx);
  anatree->Branch("genpy",&t_genpy);
  anatree->Branch("genpz",&t_genpz);
  anatree->Branch("angle",&t_angle);
  cout<<"ready: "<<endl;
  while (!fin.eof()){
    getline(fin, str);
    istringstream strm(str);
    strm>>thes;
    strm>>phis; 
    strm>>thep; 
    strm>>phip;
    //cout<<thes<<endl;
    TVector3 gs(TMath::Sin(thes)*TMath::Cos(phis),TMath::Sin(thes)*TMath::Sin(phis),TMath::Cos(thes));
    //TVector3 gs(t_genix, t_geniy, t_geniz); 
    TVector3 gdir(TMath::Sin(thep)*TMath::Cos(phip),TMath::Sin(thep)*TMath::Sin(phip),TMath::Cos(thep)); 
    gdir = gdir.Unit();
    //TVector3 gtp=gs.Unit();
    t_cos_sTheta=TMath::Cos(thes);
    t_gensTheta = thes/TMath::Pi()*180;
    t_gensPhi   = phis/TMath::Pi()*180;

    t_cos_dTheta=TMath::Cos(thep);
    t_gendTheta = thep/TMath::Pi()*180;
    t_gendPhi   = phip/TMath::Pi()*180;

    t_angle=gs.Angle(gdir);
    //t_gensTheta = gtp.Theta()/TMath::Pi()*180;
    //t_gensPhi = gtp.Phi()/TMath::Pi()*180;
    //t_gendTheta = gdir.Theta()/TMath::Pi()*180;
    //t_gendPhi = gdir.Phi()/TMath::Pi()*180;
    

    //TVector3 gcentrk = gs + (-gs*gdir)*gdir;
    //double dist_cc = (gs.Cross(gdir)).Mag();// dist from track center to LS center
    //TVector3 gr0 = gcentrk - TMath::Sqrt(m_LSRadius*m_LSRadius - dist_cc*dist_cc)*gdir;
    
    //Double_t t_genDist = gdir.Cross(gs).Mag()/gdir.Mag(); 
    //Double_t t_gencdtrack = 2*TMath::Sqrt(17700*17700 - t_genDist*t_genDist);

    //t_dTheta = (rdir.Theta()-gdir.Theta())/TMath::Pi()*180;
    //t_dPhi = (rdir.Phi()-gdir.Phi())/TMath::Pi()*180;
    anatree->Fill();
  }
  outf->Write();
  outf->Close();
   
}
