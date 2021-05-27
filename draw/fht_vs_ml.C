{
  int n=2;
  TFile* f[n];
  f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-vgg16.root");
  f[1] = TFile::Open("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtad_qc100_tc300all-2/good/rec*.rootanatemp.root");
  TProfile* pf_alpha[n];
  TProfile* pf_dD[n];
  TString tname[] = {
    "fht",
    "ml",
    "rot-20k",
    "rot-40k",
    "rot-fxl2",
    "rot5"
  };
  TString title[] = {
    "CNNs",
    "first light model",
    "rot rand xyz",
    "rot zyz",
    "rot fxl1",
    "rot fxl2",
  };
  //marker style
  int ms[] = {
    20,
    20,
    20,
    20,
    20,
    20,
  };
  EColor color[] = {
    kBlue,
    kGreen,
    kRed,
    kCyan-3,
    kOrange-1,
    kBlack,
  };
  TTree* ana = (TTree*)f[0]->Get("ana");                
  TString alpha_name = TString("pf_alpha")+tname[0];                      
  pf_alpha[0] = new TProfile(alpha_name,title[0],17,0,16.7);              
  TString drawstr = TString("angle:genDist/1000>>") + alpha_name;         
  ana->Draw(drawstr,"genDist<16700&&angle<10","goffprof");                   
  TString dD_name = TString("pf_dD")+tname[0];
  pf_dD[0] = new TProfile(dD_name,title[0],17,0,16.7);                    
  TString drawstr = TString("(recDist-genDist):genDist/1000>>") + dD_name;
  ana->Draw(drawstr,"genDist<16700&&angle<10","goffprof");

  TTree* ana = (TTree*)f[1]->Get("ana");
  TString alpha_name = TString("pf_alpha")+tname[1];                  
  pf_alpha[1] = new TProfile(alpha_name,title[1],17,0,16.7);                
  TString drawstr = TString("angle[2]:genDist/1000>>") + alpha_name;      
  ana->Draw(drawstr,"chi2<2.5&&angle[2]>0","goffprof");
  TString dD_name = TString("pf_dD")+tname[1];
  pf_dD[1] = new TProfile(dD_name,title[1],17,0,17);
  TString drawstr = TString("(recDist[2]-genDist):genDist/1000>>") + dD_name;
  ana->Draw(drawstr,"chi2<2.5&&recDist[2]==recDist[2]","goffprof");       

  for(int i=0; i<n; i++){

    pf_alpha[i]->GetXaxis()->SetTitle("D_{mc} [m]");
    pf_alpha[i]->GetYaxis()->SetTitle("Mean #alpha [deg]");
    pf_dD[i]->GetXaxis()->SetTitle("D_{mc} [m]");
    pf_dD[i]->GetYaxis()->SetTitle("Mean #Delta D [mm]");
    pf_alpha[i]->GetXaxis()->CenterTitle();
    pf_alpha[i]->GetYaxis()->CenterTitle();
    pf_dD[i]->GetXaxis()->CenterTitle();
    pf_dD[i]->GetYaxis()->CenterTitle();


    pf_alpha[i]->SetLineColor(color[i]);
    pf_dD[i]->SetLineColor(color[i]);
    pf_alpha[i]->SetMarkerStyle(ms[i]); pf_dD[i]->SetMarkerStyle(ms[i]);
    pf_dD[i]->SetMarkerStyle(ms[i]);
    pf_alpha[i]->SetMarkerColor(color[i]);
    pf_dD[i]->SetMarkerColor(color[i]);
  }


  TCanvas* c_alpha = new TCanvas;
  c_alpha->cd();
  c_alpha->SetGridx();
  c_alpha->SetGridy();
  //c_alpha->GetYaxis()->SetRangeUser(0,2.5);
  for (int i = 0; i < n; ++i) {
    c_alpha->cd();
    pf_alpha[i]->Draw("same");
  }
  c_alpha->BuildLegend(0.18,0.5,0.45,0.90);

  TCanvas* c_dD = new TCanvas;
  c_dD->cd();
  c_dD->SetGridx();
  c_dD->SetGridy();
  //c_dD->GetYaxis()->SetRangeUser(-5,5);
  for (int i = 0; i < n; ++i) {
    c_dD->cd();
    pf_dD[i]->Draw("same");
  }
  c_dD->BuildLegend(0.18,0.5,0.45,0.90);

  c_alpha->SaveAs("alpha-D.pdf");
  c_dD->SaveAs("dD-D.pdf");
  
}
