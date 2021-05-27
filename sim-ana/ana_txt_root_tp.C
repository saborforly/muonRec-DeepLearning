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

void ana_txt_root_tp();

void ana_txt_root_tp()
{
  TString outdir ="/junofs/users/liuyan/muon-sim/sim-nn/data-ana/";
  ifstream fin;
  //cout<<fname<<endl;
  
  fin.open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-rotrand-40k-fxl2-300k-tp.txt");
  string str;

  TString outname =outdir+"anasim-tt-rotrand-40k-fxl2-300k-tp.root";
  TFile* outf = new TFile(outname,"recreate");
  TTree* anatree = new TTree("ana","ana of rec");
  double t_genDist,t_recDist,t_gencdtrack;
  double t_angle;
  double t_gensTheta,t_gensPhi,t_gendTheta,t_gendPhi;
  double t_recsTheta,t_recsPhi,t_recdTheta,t_recdPhi;
  double t_genix,t_geniy,t_geniz,t_genit;
  double t_recix,t_reciy,t_reciz,t_recit;
  double t_genex,t_geney,t_genez;
  double t_recex,t_recey,t_recez;

  double t_gensthe, t_gensphi, t_genethe, t_genephi, t_recsthe, t_recsphi, t_recethe, t_recephi;
  anatree->Branch("genDist",&t_genDist);
  anatree->Branch("recDist",&t_recDist);
  anatree->Branch("angle",&t_angle);

  anatree->Branch("gencdtrack",&t_gencdtrack);  
 
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
  anatree->Branch("genex",&t_genex);
  anatree->Branch("geney",&t_geney);
  anatree->Branch("genez",&t_genez);
  anatree->Branch("recex",&t_recex);
  anatree->Branch("recey",&t_recey);
  anatree->Branch("recez",&t_recez);
  cout<<"ready: "<<endl;
  while (!fin.eof()){
    getline(fin, str);
    istringstream strm(str);
    strm>>t_gensthe;
    strm>>t_gensphi; 
    strm>>t_genethe; 
    strm>>t_genephi; 
    strm>>t_recsthe;             
    strm>>t_recsphi;              
    strm>>t_recethe;
    strm>>t_recephi;               
    //cout<<t_angle<<endl;
    t_genix=sin(t_gensthe)*cos(t_gensphi);
    t_geniy=sin(t_gensthe)*sin(t_gensphi);
    t_geniz=cos(t_gensthe);
    t_genex=sin(t_genethe)*cos(t_genephi);
    t_geney=sin(t_genethe)*sin(t_genephi);
    t_genez=cos(t_genethe);
    
    t_recix=sin(t_recsthe)*cos(t_recsphi);
    t_reciy=sin(t_recsthe)*sin(t_recsphi);
    t_reciz=cos(t_recsthe);
    t_recex=sin(t_recethe)*cos(t_recephi);
    t_recey=sin(t_recethe)*sin(t_recephi);
    t_recez=cos(t_recethe);
 

    TVector3 gs(t_genix, t_geniy, t_geniz); 
    TVector3 gdir(t_genex-t_genix, t_geney-t_geniy, t_genez-t_geniz); 
    gdir = gdir.Unit();
    TVector3 gtp=gs.Unit();
    t_gensTheta = gtp.Theta()/TMath::Pi()*180;
    t_gensPhi = t_gensphi/TMath::Pi()*180;
    t_gendTheta = gdir.Theta()/TMath::Pi()*180;
    t_gendPhi = gdir.Phi()/TMath::Pi()*180;
    
    TVector3 rs(t_recix, t_reciy, t_reciz);
    TVector3 rdir(t_recex-t_recix, t_recey-t_reciy, t_recez-t_reciz);
    rdir = rdir.Unit();
    TVector3 rtp=rs.Unit();
    t_recsTheta = rtp.Theta()/TMath::Pi()*180;
    t_recsPhi = rtp.Phi()/TMath::Pi()*180;
    t_recdTheta = rdir.Theta()/TMath::Pi()*180;
    t_recdPhi = rdir.Phi()/TMath::Pi()*180;

    //TVector3 gcentrk = gs + (-gs*gdir)*gdir;
    //double dist_cc = (gs.Cross(gdir)).Mag();// dist from track center to LS center
    //TVector3 gr0 = gcentrk - TMath::Sqrt(m_LSRadius*m_LSRadius - dist_cc*dist_cc)*gdir;
    t_angle = acos(gdir.Dot(rdir))/TMath::Pi()*180;
    Double_t t_genDist = 17700*gdir.Cross(gs).Mag()/gdir.Mag(); 
    Double_t t_recDist = 17700*rdir.Cross(rs).Mag()/rdir.Mag();
    
    Double_t t_gencdtrack = 2*TMath::Sqrt(17700*17700 - t_genDist*t_genDist);

    //t_dTheta = (rdir.Theta()-gdir.Theta())/TMath::Pi()*180;
    //t_dPhi = (rdir.Phi()-gdir.Phi())/TMath::Pi()*180;
    anatree->Fill();
  }
  outf->Write();
  outf->Close();
   
}
