#include <TFile.h>
#include <TTree.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <TMath.h>

#include <TVector3.h>
using namespace std;
const double m_LSRadius = 17700;


void root_txt(){
  TFile* f = new TFile("/junofs/users/liuyan/muon-sim/sim-tt/sim-ana/20190216anatemp.root");
  TTree* ana = (TTree*)f->Get("ana");
  int nentries = ana->GetEntries();
  double genix,geniy,geniz,genpx,genpy,genpz,energy;
  double recix[10],reciy[10],reciz[10],recpx[10],recpy[10],recpz[10],chi2[10];
  int npoints[10],channel[10];
  ana->SetBranchAddress("genix",&genix);
  ana->SetBranchAddress("geniy",&geniy);
  ana->SetBranchAddress("geniz",&geniz);
  ana->SetBranchAddress("genpx",&genpx);
  ana->SetBranchAddress("genpy",&genpy);
  ana->SetBranchAddress("genpz",&genpz);
  ana->SetBranchAddress("recix",&recix);
  ana->SetBranchAddress("reciy",&reciy);
  ana->SetBranchAddress("reciz",&reciz);
  ana->SetBranchAddress("recpx",&recpx);
  ana->SetBranchAddress("recpy",&recpy);
  ana->SetBranchAddress("recpz",&recpz);
  ana->SetBranchAddress("energy",&energy);
  ana->SetBranchAddress("chi2",&chi2);  
  ana->SetBranchAddress("npoints",&npoints);
  ana->SetBranchAddress("channel",&channel);

 
  ofstream mycout("/junofs/users/liuyan/muon-sim/sim-nn/data-cd/tt-20190216.txt",ios::out);
  int num=0;
  for (int entry = 0; entry < nentries ; ++entry){
    ana->GetEntry(entry);
    if(npoints[0]>2 && chi2[0]<2.00 && channel[0]<7 && channel[1]<7 && channel[2]<7){
      /*
      TVector3 gs(genix, geniy, geniz);
      TVector3 gdir(genpx, genpy, genpz); 
      gdir = gdir.Unit();
      TVector3 gcentrk = gs + (-gs*gdir)*gdir;  
      double dist_cc = (gs.Cross(gdir)).Mag();   
      TVector3 gr0 = gcentrk - TMath::Sqrt(m_LSRadius*m_LSRadius - dist_cc*dist_cc)*gdir; 

      genix = gr0[0];
      geniy = gr0[1];
      geniz = gr0[2];
      */
      mycout<<genix<<" "<<geniy<<" "<<geniz<<" "<<genpx<<" "<<genpy<<" "<<genpz<<" "<<energy<<" ";
      mycout<<recix[0]<<" "<<reciy[0]<<" "<<reciz[0]<<" "<<recpx[0]<<" "<<recpy[0]<<" "<<recpz[0]<<endl;
    }  
  }
  mycout.close();
  
}
