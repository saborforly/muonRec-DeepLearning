{

  const int n=2;
  TFile* f[n];
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-50k-vgg16.root");
  //f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-40k-vgg16-cd.root");
  f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
  //f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-sat2000.root");
  //f[2] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-vgg16-sat2000.root");
  //f[3] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-vgg16-randGeV.root");
  //f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-40k-vgg16-cd.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-40k-vgg16-tt.root");
  //f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotdegree-better.root");
  //f[2] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-20k.root");
  //f[3] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-5k-zyz.root");
  //f[4] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-5k-fxl.root");
  //f[3] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-5k-fxl2.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-sphere-11.root");
  //f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-300k-sphere-4.root");
  //f[2] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-sphere-6.root");
  //f[3] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-sphere-7.root");
  //f[4] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-sphere-8.root");
  f[1] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-vgg16.root");
  TProfile* pf_alpha[3];
  TProfile* pf_dD[3];
  TString tname[] = {
    "Label TT",
    "Label MC",
    "dataset 4",
    "rot-40k",
    "rot-fxl2",
    "rot5"
  };
  //TString title[] = {"no rot","rot deg xyz","rot rand xyz", "rot zyz", "rot fxl1", "rot fxl2", };
  TString title[] = {
    "TT",
    "CD",
    "dataset 4",
    "rot fxl",
    "SphericalCNN 8",
    "CNN",
  };

  //marker style
  int ms[] = {
    24,
    25,
    26,
    27,
    28,
    30,
  };
  EColor color[] = {
    kBlack,
    kRed,
    kBlue,
    kCyan,
    kOrange,
    kBlack,
  };
  // gStyle->SetLabelSize(0.06,"xyz");

  // gStyle->SetTitleOffset(0.8,"xyz");
  // gStyle->SetTitleSize(0.07,"xyz");
  for(int i=0; i<n; i++){
    TTree* ana = (TTree*)f[i]->Get("ana");

    TString alpha_name = TString("pf_alpha")+tname[i];
    pf_alpha[i] = new TProfile(alpha_name,title[i],17,0,180);
    TString drawstr = TString("angle:genstheta>>") + alpha_name;
    ana->Draw(drawstr,"genDist<17700 ","goffprof");

    TString dD_name = TString("pf_dD")+tname[i];
    pf_dD[i] = new TProfile(dD_name,title[i],17,0,180);
    drawstr = TString("(recDist-genDist):genstheta>>") + dD_name;
    ana->Draw(drawstr,"genDist<17700","goffprof");

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
    pf_alpha[i]->SetMarkerStyle(ms[i]);
    pf_dD[i]->SetMarkerStyle(ms[i]);
    pf_alpha[i]->SetMarkerColor(color[i]);
    pf_dD[i]->SetMarkerColor(color[i]);
  }


  TCanvas* c_alpha = new TCanvas("c_alpha", "c", 300, 300);
  TH1F* frm_alpha = c_alpha->DrawFrame(0, 0, 180, 2);
  frm_alpha->GetXaxis()->SetTitle("stheta [deg]");
  frm_alpha->GetYaxis()->SetTitle("Mean #alpha [deg]");
  c_alpha->cd();
  c_alpha->SetGridx();
  c_alpha->SetGridy();
  //c_alpha->GetYaxis()->SetRangeUser(0,2.5);
  for (int i = 0; i < n; ++i) {
    c_alpha->cd();
    pf_alpha[i]->Draw("same");
  }
  //c_alpha->BuildLegend(0.5,0.5,0.75,0.90);
  TLegend* leg_alpha = new TLegend(0.4,0.5,0.6,0.7);
  leg_alpha->AddEntry(pf_alpha[0],"Label TT","P");
  leg_alpha->AddEntry(pf_alpha[1],"Label MC","P");
  //leg_alpha->AddEntry(pf_alpha[2],"Dataset 4","P");
  leg_alpha->SetX1NDC(0.01);
  leg_alpha->SetX2NDC(0.9);
  leg_alpha->Draw();


  TCanvas* c_dD = new TCanvas("c_dD", "c", 300, 300);
  TH1F* frm_dD = c_dD->DrawFrame(0, -100, 180, 100);
  frm_dD->GetXaxis()->SetTitle("stheta [deg]");
  frm_dD->GetYaxis()->SetTitle("Mean #Delta D [mm]");
  c_dD->cd();
  c_dD->SetGridx();
  c_dD->SetGridy();
  //c_dD->GetYaxis()->SetRangeUser(-5,5);
  for (int i = 0; i < n; ++i) {
    c_dD->cd();
    pf_dD[i]->Draw("same");
  }
  // c_dD->BuildLegend(0.18,0.5,0.45,0.90);
  TLegend* leg = new TLegend(0.4,0.6,0.6,0.8);
  leg->AddEntry(pf_dD[0],"Label TT","P");
  leg->AddEntry(pf_dD[1],"Label MC","P");
  //leg->AddEntry(pf_dD[2],"Dataset 4","P");
  leg->SetX1NDC(0.01);
  leg->SetX2NDC(0.9);
  leg->Draw();

  //c_alpha->SaveAs("alpha-D.eps");
  //c_dD->SaveAs("dD-D.eps");
  
}
