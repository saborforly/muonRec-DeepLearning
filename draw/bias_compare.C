{
cout << endl << "Daya Bay Experiment plot defaults." << endl;

  TFile* f[5];
  f[0] = TFile::Open("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtqd_qc100_tc300all-2/good/rec*.rootanatemp.root");

  f[1] = TFile::Open("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtqd_qc100_tc300all-2/good/rec*.rootanatemp.root");
  f[2] = TFile::Open("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtqd_qc100_tc300all-2/good/rec*.rootanatemp.root");
  f[3] = TFile::Open("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtqd_qc100_tc300all-2/good/rec*.rootanatemp.root");
  f[4] = TFile::Open("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtad_qc100_tc300all-2/good/rec*.rootanatemp.root");
  TProfile* pf_alpha[5];
  
  TProfile* pf_dD[5];

  TString tname[] = {
    "1q",
    "1dada",
    "1qdqd",
    "2qd",
    "2ad",
  };

  TString title[] = {
    "corr1d q",
    "corr1d d,a,d,a",
    "corr1d q,d,q,d",
    "corr2d q-d",
    "corr2d a-d",
  };
  //marker style
  int ms[] = {
    20,
    20,
    20,
    24,
    24,
  };

  EColor color[] = {
    kCyan-3,
    kOrange-1,
    kGreen+3,
    kRed,
    kBlue,
  };

  for(int i=0; i<5; i++){
    TTree* ana = (TTree*)f[i]->Get("ana");

    TString alpha_name = TString("pf_alpha")+tname[i];
    pf_alpha[i] = new TProfile(alpha_name,title[i],17,0,17);
    TString drawstr = TString("angle[2]:genDist/1000>>") + alpha_name;
    ana->Draw(drawstr,"chi2<2.5&&angle[2]>0","goffprof");

    TString dD_name = TString("pf_dD")+tname[i];
    pf_dD[i] = new TProfile(dD_name,title[i],17,0,17);
    TString drawstr = TString("(recDist[2]-genDist)/10:genDist/1000>>") + dD_name;
    ana->Draw(drawstr,"chi2<2.5&&recDist[2]==recDist[2]","goffprof");

    pf_alpha[i]->GetXaxis()->SetTitle("D_{mc} [m]");
    pf_alpha[i]->GetYaxis()->SetTitle("Mean #alpha [deg]");
    pf_dD[i]->GetXaxis()->SetTitle("D_{mc} [m]");
    pf_dD[i]->GetYaxis()->SetTitle("Mean #Delta D [cm]");
    pf_alpha[i]->GetXaxis()->CenterTitle();
    pf_alpha[i]->GetYaxis()->CenterTitle();
    pf_dD[i]->GetXaxis()->CenterTitle();
    pf_dD[i]->GetYaxis()->CenterTitle();


    pf_alpha[i]->SetLineColor(color[i]);
    pf_dD[i]->SetLineColor(color[i]);
    pf_alpha[i]->SetMarkerStyle(ms[i]);
    pf_dD[i]->SetMarkerStyle(ms[i]);
    pf_alpha[i]->SetMarkerColor(color[i]);
    pf_dD[i]->SetMarkerColor(color[i]);
  }

  TTree* ana = (TTree*)f[4]->Get("ana");

  TProfile* pf_alpha_nocorr = new TProfile("pf_alpha_nocorr","no correction",17,0,17);
  ana->Draw("angle[0]:genDist/1000>>pf_alpha_nocorr","chi2<3.5&&angle[0]>0","goffprof");
  
  TProfile* pf_dD_nocorr = new TProfile("pf_dD_nocorr","no correction",17,0,17);
  ana->Draw("(recDist[0]-genDist)/10:genDist/1000>>pf_dD_nocorr","chi2<3.5","goffprof");

  pf_alpha_nocorr->GetXaxis()->SetTitle("D_{mc} [m]");
  pf_alpha_nocorr->GetYaxis()->SetTitle("Mean #alpha [deg]");
  pf_alpha_nocorr->GetXaxis()->CenterTitle();
  pf_alpha_nocorr->GetXaxis()->SetTitleOffset(1.2);
  pf_alpha_nocorr->GetYaxis()->CenterTitle();
  //pf_alpha_nocorr->GetYaxis()->SetRangeUser(-0.5,8);
  pf_alpha_nocorr->GetYaxis()->SetRangeUser(-0.5,1);
  pf_alpha_nocorr->SetLineColor(kBlack);
  pf_alpha_nocorr->SetMarkerStyle(20);

  pf_dD_nocorr->GetXaxis()->SetTitle("D_{mc} [m]");
  pf_dD_nocorr->GetYaxis()->SetTitle("Mean #Delta D [cm]");
  pf_dD_nocorr->GetXaxis()->CenterTitle();
  pf_dD_nocorr->GetXaxis()->SetTitleOffset(1.2);
  pf_dD_nocorr->GetYaxis()->CenterTitle();
  //pf_dD_nocorr->GetYaxis()->SetRangeUser(-60,150);
  pf_dD_nocorr->GetYaxis()->SetRangeUser(-10,10);
  pf_dD_nocorr->SetLineColor(kBlack);
  pf_dD_nocorr->SetMarkerStyle(20);


  TCanvas* c_alpha = new TCanvas;
  c_alpha->cd();
  c_alpha->SetGridx();
  c_alpha->SetGridy();
  pf_alpha_nocorr->Draw();
  for (int i = 0; i < 5; ++i) {
    c_alpha->cd();
    pf_alpha[i]->Draw("same");
  }
  c_alpha->BuildLegend(0.18,0.5,0.45,0.90);
  pf_alpha_nocorr->SetTitle("");

  TCanvas* c_dD = new TCanvas;
  c_dD->cd();
  c_dD->SetGridx();
  c_dD->SetGridy();
  pf_dD_nocorr->Draw();
  for (int i = 0; i < 5; ++i) {
    c_dD->cd();
    pf_dD[i]->Draw("same");
  }
  c_dD->BuildLegend(0.18,0.5,0.45,0.90);
  pf_dD_nocorr->SetTitle("");

  c_alpha->SaveAs("alpha-D.pdf");
  c_dD->SaveAs("dD-D.pdf");


}
