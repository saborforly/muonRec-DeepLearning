TString picsroot = "/junofs/users/liuyan/muon-sim/paperStyle/draw/pics/";
//TFile* f = new TFile("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtad_qc100_tc300flists5/rec_total.root");
TFile* f = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16-100k.root");
//TFile* f = new TFile("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr1dtqdqd_tc300all-1/rec*.rootanatemp.root");
TTree* t = (TTree*)f->Get("ana");
void draw_eff_angle_dD(){
  //double rangeDist = 17800-1047.1;
  double rangeDist = 16700;
  const int nbins= 8; 
  double binWidth = rangeDist/nbins;
  double dx[4],dy[4],dz[4];
  double dthe[4],dphi[4];
  double totevts[nbins];
  double sucevts[nbins];
  double eff[nbins],eff1[nbins];
  double efferror[nbins],efferror1[nbins];
  double binc[nbins],binc1[nbins],binc2[nbins];
  double errbin[nbins] = {0};

  double dD[4],angle[4];
  double dD0[4],angle0[4];

  double shift = 0.3;
  TString selDesc = TString::Format("angle>=0");
  //dD 
  valname = TString::Format("(recDist-genDist)/10>>hD(100,-25,25)");
  TString valtitle = TString::Format("#Delta D[cm] ");
  draw_dD(valname,valtitle,"hD",selDesc,dD[0],dD[1],dD[2],dD[3]);
  //dangle
  valname = TString::Format("angle>>hangle(100,0,10)");
  TString valtitle = TString::Format("#alpha [deg]");
  draw_angle(valname,valtitle,"hangle",selDesc,angle[0],angle[1],angle[2],angle[3]);
  //total events
  TString selDesc = TString::Format("genDist<%f&&genDist>%f",16700,0);
  double nevents = ana->GetEntries(selDesc);

  for(int i=0; i<nbins; i++){
    double binb = i*binWidth;
    double bine = binb+binWidth;
    binc[i] = (binb+bine)/2;
    binc[i] /= 1000;
    binc1[i] = binc[i] + shift; 
    binc2[i] = binc1[i] + shift; 

    TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb);
    double nevents = ana->GetEntries(selDesc);
    totevts[i] = nevents;
    // sucesss dx,dy,dz : cm -> mm with in 5*sigma
    TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb); 
    selDesc += TString::Format("&&TMath::Abs((recDist-genDist)/10-%f)<%f && TMath::Abs(angle-%f)<%f",dD[0],dD[2]*5,angle[0],angle[2]*4*5);
    double nsucc = ana->GetEntries(selDesc.Data());
    eff[i] = nsucc/nevents;
    cout<<selDesc<<endl;
    cout<<nsucc<<"  "<<nevents<<"  "<<"eff: "<<eff[i]<<endl;
    efferror[i]=0;
   
  }

  TCanvas *c_eff = new TCanvas;
  TGraphErrors* g_eff = new TGraphErrors(nbins,binc,eff,errbin,efferror);
  g_eff->GetYaxis()->SetTitle("Tracking Effeciency");
  g_eff->SetMarkerStyle(20);
  g_eff->GetXaxis()->SetTitle("D_{mc} [m]");
  g_eff->GetYaxis()->CenterTitle();
  g_eff->GetXaxis()->CenterTitle();
  g_eff->SetTitle("");
  g_eff->Draw();
  c_eff->SetGridx();
  c_eff->SetGridy();
  g_eff->SaveAs(picsroot+"/eff.pdf");
}

void draw_angle(TString drawDesc, TString title, TString hname, TString selDesc, double &mean,double &meanerror,double& sigma,double& sigmaerror){
  TCanvas *ca = new TCanvas;
  t->Draw(drawDesc,selDesc,"goff");
  h = (TH1*)gDirectory->Get(hname.Data());
  h->Fit("landau");
  h->SetTitle(title);
  func = h->GetFunction("landau");
  mean = func->GetParameter(1);
  meanerror = func->GetParError(1);
  sigma = func->GetParameter(2);
  sigmaerror = func->GetParError(2);
  TString name = picsroot + TString::Format("tmp/%s.pdf",title.Data());
  ca->SaveAs(name);

  cout<<"mean"<<mean<<"+-"<<meanerror<<endl;
  cout<<"sigma"<<sigma<<"+-"<<sigmaerror<<endl;

}
void draw_dD(TString drawDesc, TString title, TString hname, TString selDesc, double &mean,double &meanerror,double& sigma,double& sigmaerror){
  TCanvas *ca = new TCanvas;
  t->Draw(drawDesc,selDesc,"goff");
  h = (TH1*)gDirectory->Get(hname.Data());
  h->Fit("gaus");
  h->SetTitle(title);
  func = h->GetFunction("gaus");
  mean = func->GetParameter(1);
  meanerror = func->GetParError(1);
  sigma = func->GetParameter(2);
  sigmaerror = func->GetParError(2);
  TString name = picsroot + TString::Format("tmp/%s.pdf",title.Data());
  ca->SaveAs(name);

  cout<<"mean"<<mean<<"+-"<<meanerror<<endl;
  cout<<"sigma"<<sigma<<"+-"<<sigmaerror<<endl;

}

