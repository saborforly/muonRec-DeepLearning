{
  TFile* f[4];
  int n=2;
  f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
  f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-300k-sphere-4.root");
  //f[2] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-100k-vgg16.root");
  //f[3] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-5k-fxl2.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-ttrotz-cd-newdata.root");
  //f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-cd-newdata.root");
  TProfile* pf_alpha[4];
  TProfile* pf_dD[4];
  TString tname[] = {
    "rot",
    "No-rot",
    "rot-20k",
    "rot-40k"
  };
  TString title[] = {
    "tt",
    "tt-sphere",
    "cd-100k",
    "tt-5k-fxl2"
  };
  //marker style
  int ms[] = {
    20,
    20,
    20,
    20,
  };
  EColor color[] = {
    kBlue,
    kRed,
    kCyan-3,
    kOrange-1,
  };
  for(int i=0; i<n; i++){
    TTree* ana = (TTree*)f[i]->Get("ana");

    TString alpha_name = TString("pf_alpha")+tname[i];
    pf_alpha[i] = new TProfile(alpha_name,title[i],18,0,180);
    TString drawstr = TString("angle:genstheta>>") + alpha_name;
    ana->Draw(drawstr,"genDist<16700&&angle<10","goffprof");

    TString dD_name = TString("pf_dD")+tname[i];
    pf_dD[i] = new TProfile(dD_name,title[i],18,0,180);
    TString drawstr = TString("(recDist-genDist):genstheta>>") + dD_name;
    ana->Draw(drawstr,"angle<10&&genDist<16700","goffprof");

    pf_alpha[i]->GetXaxis()->SetTitle("inject point #theta [deg]");
    pf_alpha[i]->GetYaxis()->SetTitle("Mean #alpha [deg]");
    pf_dD[i]->GetXaxis()->SetTitle("inject point #theta [deg]");
    pf_dD[i]->GetYaxis()->SetTitle("Mean #Delta D [mm]");
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
