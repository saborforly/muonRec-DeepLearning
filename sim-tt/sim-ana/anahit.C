#include <TFile.h>
#include <TTree.h>
#include <algorithm>
#include <vector>
#include <TVector3.h>
#include <TChain.h>
#include <TSystem.h>
#include <TMath.h>
#include <string>
#include <Event/TTCalibEvent.h>
#include <Event/SimHeader.h>
#include <Event/RecHeader.h>
#include <fstream>
#include <iostream>
using namespace std;
const double m_LSRadius = 17700;
ostream &operator<<(ostream& s, TVector3& tv)
{
  s << "(" << tv.X() << ", " << tv.Y() << ", " << tv.Z() << ")" ; 
  return s; 
}


void anahit(TString ftxt){
  TString outdir ="/junofs/users/liuyan/k8s/20190216";
  //TString outdir ="/junofs/users/liuyan/muon-sim/sim-tt/sim-ana/20190216";
  TString outname =outdir+"anatemp.root";
  TFile* outf = new TFile(outname,"recreate");
  TTree* anatree = new TTree("ana","ana of rec");
  vector<Double_t> theTrk, phiTrk, deltatheta; 
  vector<Double_t> recd, gend, deltad, deltainj; 
  vector<Double_t> vchi2;
  vector<Double_t> vangle;
  vector<Double_t> vdist;

  double t_totalpe[10];
  double t_genDist,t_energy;
  double t_angle[10];
  double t_recDist[10];
  double t_chi2[10],t_cdinjd[10],t_wpd[10],t_rcdz[10],t_rzaix[10];
  int t_npoints[10],t_nch;
  int t_channel[14],t_zflag;
  double t_dist[10];
  double t_genTheta;
  double t_genPhi;
  double t_dTheta[10];
  double t_dPhi[10];
  double t_genix,t_geniy,t_geniz,t_genit;
  double t_recix[10],t_reciy[10],t_reciz[10],t_recit[10];
  double t_gentl,t_rectl[10];
  double t_genpx,t_genpy,t_genpz;
  double t_startx,t_starty,t_startz;
  double t_cdx,t_cdy,t_cdz;
  double t_recpx[10],t_recpy[10],t_recpz[10],t_dist_bottom[10];
  int t_ntrk;
  anatree->Branch("ntrk",&t_ntrk);
  anatree->Branch("genDist",&t_genDist);
  anatree->Branch("recDist",t_recDist,"recDist[ntrk]/D");
  anatree->Branch("angle",t_angle,"angle[ntrk]/D");
  anatree->Branch("npoints",t_npoints,"npoints[ntrk]/I");
  anatree->Branch("chi2",t_chi2,"chi2[ntrk]/D");
  anatree->Branch("gentheta",&t_genTheta);
  anatree->Branch("genphi",&t_genPhi);
  anatree->Branch("dtheta",t_dTheta,"dtheta[ntrk]/D");
  anatree->Branch("dphi",t_dPhi,"dphi[ntrk]/D");

  anatree->Branch("startx",&t_startx);
  anatree->Branch("starty",&t_starty);
  anatree->Branch("startz",&t_startz);
  anatree->Branch("cdx",&t_cdx);
  anatree->Branch("cdy",&t_cdy);
  anatree->Branch("cdz",&t_cdz);


  anatree->Branch("genix",&t_genix);
  anatree->Branch("geniy",&t_geniy);
  anatree->Branch("geniz",&t_geniz);
  anatree->Branch("genit",&t_genit);
  anatree->Branch("recix",t_recix,"recix[ntrk]/D");
  anatree->Branch("reciy",t_reciy,"reciy[ntrk]/D");
  anatree->Branch("reciz",t_reciz,"reciz[ntrk]/D");
  anatree->Branch("genpx",&t_genpx);
  anatree->Branch("genpy",&t_genpy);
  anatree->Branch("genpz",&t_genpz);
  anatree->Branch("gentl",&t_gentl);
  anatree->Branch("recpx",t_recpx,"recpx[ntrk]/D");
  anatree->Branch("recpy",t_recpy,"recpy[ntrk]/D");
  anatree->Branch("recpz",t_recpz,"recpz[ntrk]/D");
  anatree->Branch("dist_bottom",t_dist_bottom,"dist_bottom[ntrk]/D");
  anatree->Branch("cdinjd",t_cdinjd,"cdinjd[ntrk]/D");
  anatree->Branch("nchannel",&t_nch);
  anatree->Branch("wpd",t_wpd,"wpd[ntrk]/D");
  anatree->Branch("channel",t_channel,"channel[8]/I");
  anatree->Branch("rcdz",t_rcdz,"rcdz[ntrk]/D");
  anatree->Branch("rzaix",t_rzaix,"rzaix[ntrk]/D");
  anatree->Branch("energy",&t_energy);
  string ftxtnew=string(ftxt);
  ifstream in(ftxt);
  string users,recs,line,filen;
  TString fusers,frecs;
  int loc,len;
  int NDeposits,dep_nwall[100];
  Float_t InitX;
  while(getline(in,line)){
    //loc=line.rfind("cal-");
    loc=line.rfind("cal-");
    len=line.length();

    filen=line.substr(loc+4,len);
    users=filen.insert(0,"/junofs/users/liuyan/muon-sim/sim-tt/sim-rec/cal/data/20190216_muonexe/cal-");

    fusers=TString(users);
    frecs=TString(line);
    cout<<"user file "<<fusers<<endl;
    //fusers="/junofs/users/liuyan/muon-sim/testuser-3.root";    
    //frecs="/junofs/users/liuyan/muon-sim/sim-tt/sim-rec/rec/data/20180813_muoncal/rec-cal-evt-11472-3.root";
    TFile *f = new TFile(fusers);
    /*
    TTree *t = (TTree*)f->Get("TT");
    TTree *tgen = (TTree*)f->Get("geninfo"); 

    TBranch *xg = tgen->GetBranch("InitX");
    xg->SetAddress(&InitX);
    TBranch *nd = t->GetBranch("NDeposits");
    TBranch *nw = t->GetBranch("dep_nwall");
    nd->SetAddress(&NDeposits);
    nw->SetAddress(&dep_nwall);
    */
    TTree *t = (TTree*)f->Get("/Event/Calib/TTCalibEvent");

    TFile *fre=new TFile(frecs);
    TTree* retree = (TTree*)fre->Get("/Event/Rec/TTRecEvent");
    TTree* rhtree = (TTree*)fre->Get("/Event/Rec/RecHeader");
    TTree* simtree= (TTree*)fre->Get("/Event/Sim/SimEvent");
     
    
    //TChain* simtree = new TChain();
    //TChain* rhtree = new TChain();
    //TChain* retree = new TChain(); 
    //retree->Add(frecs+"/Event/Rec/TTRecEvent");
    //rhtree->Add(frecs+"/Event/Rec/RecHeader");  
    //simtree->Add(frecs+"/Event/Sim/SimEvent");
    JM::TTRecEvent * recevt = new JM::TTRecEvent; 
    retree->SetBranchAddress("TTRecEvent", &recevt); 

    JM::RecHeader * header = new JM::RecHeader; 
    rhtree->SetBranchAddress("RecHeader", &header); 

    JM::SimEvent* simEvent = new JM::SimEvent; 
    simtree->SetBranchAddress("SimEvent", &simEvent); 

    JM::TTCalibEvent* cal=new JM::TTCalibEvent;
    t->SetBranchAddress("TTCalibEvent", &cal);
    
    Int_t nentries = simtree->GetEntries(); 
    if (nentries!=retree->GetEntries() || nentries!=t->GetEntries()){
      cout<<"missmatch entriese:"<<nentries<<" "<<retree->GetEntries()<<" "<<t->GetEntries()<<endl;
      break;
    }
    //cout<<"nentries:"<<nentries<<endl;
    for (int entry = 0; entry < nentries ; ++entry){
      /*
      nd->GetEntry(entry);
      nw->GetEntry(entry);
      xg->GetEntry(entry);
      t_NDeposits=NDeposits;
      //t_dep_nwall=dep_nwall;
      int count_nwall=0;
      int n_num=0;

      for (int i = 0; i < NDeposits; i++) {
        for (int j = 0; j < NDeposits - i; j++) {
            if (dep_nwall[j] > dep_nwall[j + 1]) {
                int temp = dep_nwall[j];
                dep_nwall[j] = dep_nwall[j + 1];
                dep_nwall[j + 1] = temp;
            }
        }
      }
      for(int j=0;j< 6;++j){      
        t_dep_nwall[j]=0;
      }
      int wall=dep_nwall[0];
      for(int n=0;n< NDeposits;++n){
        if(dep_nwall[n]!=wall){
          wall=dep_nwall[n];
          t_dep_nwall[n_num]=count_nwall;   
          n_num=n_num+1;
          count_nwall=0;
        }
        count_nwall=count_nwall+1;
        if(n==NDeposits-1){
          t_dep_nwall[n_num]=count_nwall;
        }      
      }
      
      cout<<" NDeposits is "<<t_NDeposits<<endl;
      //cout<<" dep_nwall is "<<t_dep_nwall[0]<<" "<<t_dep_nwall[1]<<" "<<t_dep_nwall[2]<<" "<<t_dep_nwall[3]<<endl;
      */

      retree->GetEntry(entry); 
      simtree->GetEntry(entry); 
      JM::SimEvent* se = simEvent;
      JM::SimTrack* sitrk = se->findTrackByTrkID(1); 
      Float_t genx = sitrk->getInitX(); 
      Float_t geny = sitrk->getInitY(); 
      Float_t genz = sitrk->getInitZ(); 
      Float_t gent = sitrk->getInitT();
      Float_t genpx = sitrk->getInitPx(); 
      Float_t genpy = sitrk->getInitPy(); 
      Float_t genpz = sitrk->getInitPz(); 
      TVector3 gs(genx, geny, genz); 
      TVector3 gdir(genpx, genpy, genpz); 
      gdir = gdir.Unit();
      t_genTheta = gdir.Theta()/TMath::Pi()*180;
      t_genPhi = gdir.Phi()/TMath::Pi()*180;
      //cout<<" energy : "<<genpx/gdir.x()<<endl;
      t_energy=genpx/gdir.x();
      //if(InitX==genx){
      //  cout<<"right"<<endl;
      //}
            


      double frac1 = (25000-gs.z())/gdir.Unit().z();
      TVector3 gr0 = gs+frac1*gdir;
    
      Double_t gendist = gdir.Cross(gs).Mag()/gdir.Mag(); 
      t_genDist = gendist;
      
      TVector3 gencentrk = gr0 + (-gr0*gdir)*gdir;
      TVector3 gcd = gencentrk - TMath::Sqrt(m_LSRadius*m_LSRadius - gendist*gendist)*gdir;

      t_startx = genx;
      t_starty = geny;
      t_startz = genz; 
           
      t_cdx = gcd[0];
      t_cdy = gcd[1];
      t_cdz = gcd[2];


      t_genix = gr0[0];
      t_geniy = gr0[1];
      t_geniz = gr0[2];
    
      t_genpx = gdir[0];
      t_genpy = gdir[1];
      t_genpz = gdir[2];
      
      t->GetEntry(entry);
      int nch=0,channel,n_num=0;
      int count_nwall=0;
      int all_ch=0;
      int wallID[50]={0};
      for(int i=0;i< 14;++i){ 
        t_channel[i]=0;
      } 
      const std::list<JM::CalibTTChannel*>& tmp_calibttcol = cal->calibTTCol();
      Float_t wallZ[100]={0};
      
      for (std::list<JM::CalibTTChannel*>::const_iterator ithit = tmp_calibttcol.begin();
      ithit != tmp_calibttcol.end(); ++ithit){
        JM::CalibTTChannel* tthit = *ithit;
        long channelID=tthit->pmtId();
        channel=int(channelID/32768);
        wallZ[all_ch]=tthit->z();
        cout<<channelID<<"  "<<channel<<"  Z  "<<tthit->z()<<endl;
        wallID[all_ch]=channel;
        all_ch=all_ch+1;
      }
      for (int i = 0; i < all_ch; i++) {
        for (int j = 0; j < all_ch-1 - i; j++) {
            if (wallID[j] > wallID[j + 1]) {
                int temp = wallID[j];
                wallID[j] = wallID[j + 1];
                wallID[j + 1] = temp;
            }
        }
      }
 
      int wall=wallID[0];
      for (int k = 0; k < all_ch; k++){
        if(wallID[k]==wall){
          count_nwall=count_nwall+1;
          if(k == all_ch-1 ){
            t_channel[n_num]=count_nwall;
          }
          continue;
        }
         
        wall=wallID[k];
        //cout<<"test for loop: "<<wall<<" "<<k<<" "<<wallID[k]<<" "<<endl;

        if(k == all_ch-1 ){
          t_channel[n_num]=count_nwall;
        }
        t_channel[n_num]=count_nwall;
        count_nwall=1;
        n_num=n_num+1;        

      }
      
      
      float wallzs=0;
      wallzs=wallZ[0];
      int z_num=0;

      float walltest[5]={0};
      walltest[0]=wallzs;
      for (int k = 0; k < all_ch; k++){
        //cout<<wallZ[k]<<endl;
        if(wallzs-wallZ[k]>45 || wallzs-wallZ[k]<-45){
          wallzs=wallZ[k];
          z_num=z_num+1;

          walltest[z_num]=wallzs;

        }

      }
      int wallzflag=0;
      for(int i=0;i<z_num;++i){
        if(walltest[z_num]==walltest[i]){
          wallzflag=1;
        }
      }
      t_zflag=wallzflag;    
      //cout<<"wallzflag: "<<wallzflag<<endl;

      t_nch=all_ch;


      const std::vector<double>& sx = recevt->Coeff0();
      const std::vector<double>& sy = recevt->Coeff1();
      const std::vector<double>& sz = recevt->Coeff2();
      const std::vector<double>& px = recevt->Coeff3();
      const std::vector<double>& py = recevt->Coeff4();
      const std::vector<double>& pz = recevt->Coeff5();
      const std::vector<double>& Ch = recevt->Chi2();
      const std::vector<int>& npoints = recevt->nPoints();
      Int_t ntrk=sx.size();
      t_ntrk = ntrk;
      if(ntrk>1){
        continue;
      }
      
      int trk=-1;
      for (int itrk = 0; itrk < ntrk; ++itrk)
      { 
      
        double i_x = sx[itrk];
        double i_y = sy[itrk];
        double i_z = sz[itrk]+24250;
        double p_x = px[itrk];
        double p_y = py[itrk];
        double p_z = pz[itrk];
      
        TVector3 dir(p_x, p_y, p_z); 
        if(p_z>0){
          dir=-dir;
        } 
        dir = dir.Unit();
        double chi2 = Ch[itrk];      
        TVector3 s(i_x, i_y, i_z);
       
     
        double frac2 = (25000-s.z())/dir.Unit().z();
        TVector3 rr0 = s+frac2*dir;

        t_dTheta[itrk] = (dir.Theta()-gdir.Theta())/TMath::Pi()*180;
        t_dPhi[itrk] = (dir.Phi()-gdir.Phi())/TMath::Pi()*180;


        Double_t recdist = dir.Cross(rr0).Mag()/dir.Mag(); 
        t_recDist[itrk] = recdist;
        if(recdist>m_LSRadius ){
           continue;
        }
 
        trk=trk+1;

        TVector3 reccentrk = rr0 + (-rr0*dir)*dir;
        TVector3 rcd = reccentrk - TMath::Sqrt(m_LSRadius*m_LSRadius - recdist*recdist)*dir;        
        t_cdinjd[itrk]=(gcd-rcd).Mag();
        TVector3 ztv(0,0,1);
        t_wpd[itrk]=(gcd-rcd).Cross(ztv).Mag();
        t_rcdz[itrk]=rcd.z();        

        t_rzaix[itrk]=TMath::Sqrt(rcd.Mag()*rcd.Mag()-rcd.z()*rcd.z());
        t_angle[itrk] = dir.Angle(gdir)/TMath::Pi()*180;
        if(t_recix != t_recix) t_angle[itrk] = -1;
     
        //numTotTrk++; 
        vchi2.push_back(chi2);
        
        t_npoints[itrk]=npoints[itrk];
        t_chi2[itrk] = chi2;
        theTrk.push_back(dir.Theta()/3.1415926*180.); 
        phiTrk.push_back(dir.Phi()/3.1415926*180.); 
        deltatheta.push_back((dir.Theta()-gdir.Theta())/3.1415926*180.); 
        recd.push_back(recdist); 
        gend.push_back(gendist); 
        deltad.push_back(recdist-gendist); 
        deltainj.push_back((gr0-rr0).Mag()); 
      
        double frac3 = (-21750-rr0.z())/dir.Unit().z(); 
        TVector3 rep = rr0+frac3*dir;
        double frac4 = (-21750-gr0.z())/gdir.Unit().z(); 
        TVector3 gep = gr0+frac4*gdir;   
        t_dist_bottom[itrk]=(rep-gep).Mag();
                 



        TVector3 vecgen2rec=gs-rr0;
        TVector3 vecVet = gdir.Cross(dir);
        vangle.push_back(dir.Angle(gdir)/3.1415926*180);
        vdist.push_back(vecgen2rec*vecVet.Unit());
        t_dist[itrk] = vecgen2rec*vecVet.Unit();
        t_recix[itrk] = rr0[0] ;
        t_reciy[itrk] = rr0[1];
        t_reciz[itrk] = rr0[2];
        t_recpx[itrk] = dir[0];
        t_recpy[itrk] = dir[1];
        t_recpz[itrk] = dir[2];
        t_angle[itrk] = dir.Angle(gdir)/TMath::Pi()*180;
         
        if(1){
          
          cout << "-------------"<< entry << endl;          
          cout<<" NDeposits is "<<t_nch<<endl; 
          cout<<" dep_nwall is "<<t_channel[0]<<" "<<t_channel[1]<<" "<<t_channel[2]<<" "<<t_channel[3]<<" "<<t_channel[4]<<" "<<t_channel[5]<<endl;
          cout << "rec start :" << rr0 << endl; 
          cout << "gen start :" << gr0 << endl;
          cout << "gen direction :" << gdir << endl;
          cout << "rec direction :" << dir << endl; 
          cout << "delta start:"<<((gr0-rr0).Mag())<<endl;

          cout << "gendist:" << gendist << "mm" << endl; 
          cout << "recdist:" << recdist << "mm" << endl; 
          cout << "chi2 of rec: "<< chi2<<endl;
          cout << "npoints of rec: "<<npoints[trk] <<endl;
          cout<<"dist: "<<vecgen2rec*vecVet.Unit()<<"mm"<<endl;
          cout<<"angle:"<<dir.Angle(gdir)/3.1415926*180<<endl;
          cout<<"dist_bottom :"<<t_dist_bottom[0]<<endl;
          cout<<"t_rcdz : "<<t_rcdz[0]<<endl;
          cout<<"t_rzaix : "<<t_rzaix[0]<<endl;
        }
        
      }
      
      if(trk>-1 && t_recpz[0]<0){
        anatree->Fill();
      }
    }
    fre->Close();
    f->Close(); 
  }
  in.close();
  outf->Write();
  outf->Close();

}
