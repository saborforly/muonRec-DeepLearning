TString picsroot = "/junofs/users/liuyan/muon-sim/paperStyle/draw/pics/";
//TFile* f = new TFile("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtad_qc100_tc300flists5/rec_total.root");
TFile* f = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16-100k.root");
//TFile* f = new TFile("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr1dtqdqd_tc300all-1/rec*.rootanatemp.root");
TTree* t = (TTree*)f->Get("ana");
void draw_eff(){
  //double rangeDist = 17800-1047.1;
  double rangeDist = 16700;
  const int nbins= 9; 
  double binWidth = rangeDist/8;
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
  //dx mm->cm
  TString valname = TString::Format("(recix-genix)/10>>hx(100,-25,25)");
  TString valtitle = TString::Format("#Delta injectx[cm]");
  draw(valname,valtitle,"hx",selDesc,dx[0],dx[1],dx[2],dx[3]);
  //dy mm->cm
  valname = TString::Format("(reciy-geniy)/10>>hy(100,-25,25)");
  TString valtitle = TString::Format("#Delta injecty[cm] ");
  draw(valname,valtitle,"hy",selDesc,dy[0],dy[1],dy[2],dy[3]);
  //dz
  valname = TString::Format("(reciz-geniz)/10>>hz(100,-25,25)");
  TString valtitle = TString::Format("#Delta injectz[cm]");
  draw(valname,valtitle,"hz",selDesc,dz[0],dz[1],dz[2],dz[3]);
  //dtheta
  valname = TString::Format("(recdtheta-gendtheta)>>hthe(100,-1,1)");
  TString valtitle = TString::Format("#Delta #theta[deg] ");
  draw(valname,valtitle,"hthe",selDesc,dthe[0],dthe[1],dthe[2],dthe[3]);
  //dphi
  valname = TString::Format("(recdphi-gendphi)>>hphi(100,-1,1)");
  TString valtitle = TString::Format("#Delta #phi[deg]");
  draw(valname,valtitle,"hphi",selDesc,dphi[0],dphi[1],dphi[2],dphi[3]);
  //dD 
  valname = TString::Format("(recDist-genDist)/10>>hD(1000,-25,25)");
  TString valtitle = TString::Format("#Delta D[cm] ");
  draw(valname,valtitle,"hD",selDesc,dD[0],dD[1],dD[2],dD[3]);
  //dangle
  valname = TString::Format("angle>>hangle(100,0,10)");
  TString valtitle = TString::Format("#alpha [deg]");
  draw(valname,valtitle,"hangle",selDesc,angle[0],angle[1],angle[2],angle[3]);
  //total events
  TString selDesc = TString::Format("genDist<%f&&genDist>%f",16700,0);
  double nevents = ana->GetEntries(selDesc);

  TH1F *hnsucc=new TH1F("h_n_pass","",170,0.5,17.5);
  TH1F *hall  =new TH1F("h_n_all" ,"",170,0.5,17.5);
  TAxis* xasix = hnsucc->GetXaxis();
  for(int i=0; i<nbins; i++){
    if(i>6){
        double binb = bine;
        double bine = binb+binWidth/2;
    }else{
        double binb = i*binWidth;
        double bine = binb+binWidth;
    }
    binc[i] = (binb+bine)/2;
    binc[i] /= 1000;
    binc1[i] = binc[i] + shift; 
    binc2[i] = binc1[i] + shift; 

    TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb);
    double nevents = ana->GetEntries(selDesc);
    totevts[i] = nevents;
    // sucesss dx,dy,dz : cm -> mm with in 5*sigma
    TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb); 
    selDesc += TString::Format("&&TMath::Abs(recix-genix)<%f&&TMath::Abs(reciy-geniy)<%f&&TMath::Abs(reciz-geniz)<%f&&TMath::Abs(gendtheta-recdtheta)<%f&&TMath::Abs(gendphi-recdphi)<%f",dx[2]*10*5,dy[2]*10*5,dz[2]*10*5,dthe[2]*5,dphi[2]*5);
    double nsucc = ana->GetEntries(selDesc.Data());
    eff[i] = nsucc/nevents;
    int tmp_bin = xasix->FindBin((binb+bine)/2000);
    std::cout<<"val="<<(binb+bine)/2<<",bin="<<tmp_bin<<",nsucc="<<nsucc<<",nevents="<<nevents<<std::endl;
    hnsucc->SetBinContent(tmp_bin, nsucc);
    hall  ->SetBinContent(tmp_bin, nevents);
    cout<<selDesc<<endl;
    cout<<nsucc<<"  "<<nevents<<"  "<<"eff: "<<eff[i]<<endl;
    efferror[i]=0;
    //efferror[i] = TMath::Sqrt(nsucc*(1-nsucc/nevents))/nevents;
   
    //sucess 
    TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb);
    selDesc += TString::Format("&&angle>=0");
    selDesc += TString::Format("&&TMath::Abs(recDist-genDist-(%f))<%f&&TMath::Abs(angle-(%f))<%f",dD[0]*10,dD[2]*10*5,angle[0],angle[2]*5);

    nsucc = ana->GetEntries(selDesc.Data());
    eff1[i] = nsucc/nevents;
    efferror1[i]=0;
    //efferror1[i] = TMath::Sqrt(nsucc*(1-nsucc/nevents))/nevents;
  }

  TCanvas *c_eff = new TCanvas;
  TGraphErrors* g_eff = new TGraphErrors(nbins,binc,eff,errbin,efferror);
  TGraphAsymmErrors* g_eff2 = new TGraphAsymmErrors();
  g_eff2->Divide(hnsucc, hall, "cl=0.683 b(1,1) mode"); 
  g_eff2->SetMarkerStyle(21);
  g_eff->SetMarkerStyle(20);

  g_eff->SetLineWidth(1);
  g_eff->GetYaxis()->CenterTitle();
  //g_eff->GetYaxis()->SetNdivisions(505);
  g_eff->SetTitle("");
  g_eff->GetYaxis()->SetTitle("Tracking Effeciency");
  g_eff->GetYaxis()->SetTitleOffset(0.9);
  g_eff->GetYaxis()->SetTitleSize(0.06);
  g_eff->GetYaxis()->SetLabelOffset(0.005);
  g_eff->GetYaxis()->SetLabelSize(0.06);
  g_eff->GetXaxis()->SetTitle("D_{mc} [m]");
  g_eff->GetXaxis()->SetRangeUser(0,17);
  g_eff->GetXaxis()->SetTitleOffset(0.9);
  g_eff->GetXaxis()->SetTitleSize(0.06);
  g_eff->GetXaxis()->SetLabelOffset(0.005);
  g_eff->GetXaxis()->SetLabelSize(0.06);
  g_eff->GetXaxis()->CenterTitle();

  g_eff->SetTitle("");
  //hnsucc->Draw("hist");
  //hall->Draw("same:pe");
  g_eff->Draw();
  g_eff2->Draw("p");
  c_eff->SetGridx();
  c_eff->SetGridy();
  //g_eff->SaveAs(picsroot+"/eff.pdf");


  /*
  TCanvas *c_eff1 = new TCanvas;
  TGraphErrors* g_eff1 = new TGraphErrors(nbins,binc,eff1,errbin,efferror1);
  g_eff1->GetYaxis()->SetTitle("Tracking Effeciency");
  g_eff1->SetMarkerStyle(20);
  g_eff1->GetXaxis()->SetTitle("D_{mc} [m]");
  g_eff1->SetTitle("a_D");
  g_eff1->Draw();
  c_eff1->SetGridx();
  c_eff1->SetGridy();
  c_eff1->SaveAs(picsroot+"/eff_with_angle_Dist_cut.pdf");
  */
}

void draw(TString drawDesc, TString title, TString hname, TString selDesc, double &mean,double &meanerror,double& sigma,double& sigmaerror){
  //TCanvas *ca = new TCanvas;
  t->Draw(drawDesc,selDesc,"goff");
  h = (TH1*)gDirectory->Get(hname.Data());
  h->Fit("gaus");
  h->SetTitle(title);
  func = h->GetFunction("gaus");
  mean = func->GetParameter(1);
  meanerror = func->GetParError(1);
  sigma = func->GetParameter(2);
  sigmaerror = func->GetParError(2);
  //TString name = picsroot + TString::Format("tmp/%s.pdf",title.Data());
  //ca->SaveAs(name);

  cout<<"mean"<<mean<<"+-"<<meanerror<<endl;
  cout<<"sigma"<<sigma<<"+-"<<sigmaerror<<endl;

}
