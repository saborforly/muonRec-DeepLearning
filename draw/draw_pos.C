{
  /*
  *** /junofs/users/lint/papers/gpu/plot_speedup_multigpus.C
  *** ~/styles/TStyle_CPCLY.C
  */
  gStyle->SetOptStat(00000000);
  
  int n=1;
  TFile* f[1];
  //f[0] = TFile::Open("/junofs/users/liuyan2016/muon-sim/sim-nn/data-draw/draw/pos_cd.root");
  f[0] = TFile::Open("/junofs/users/liuyan2016/muon-sim/sim-nn/data-ana/anashow-tt-display.root");
  //f[0] = TFile::Open("/junofs/users/liuyan2016/muon-sim/sim-nn/data-ana/sim-tt-trainrotrand-40k-fxl2-260k.root");
  for(int i=0; i<n; i++){
    TTree* ana = (TTree*)f[i]->Get("ana");
    TCanvas* proj = new TCanvas("proj", "c", 600, 400);
    
    TH2F* frm_proj = new TH2F("frm_proj","",60,-200,200, 60,-1,-0.2);
    //TH1F* frm_proj = proj->DrawFrame(-0.2, 1, -180, 180);
    frm_proj->GetXaxis()->SetTitle("#phi [deg]");
    frm_proj->GetXaxis()->SetTitleSize(0.07);
    frm_proj->GetXaxis()->SetTitleOffset(0.8);                                 
    frm_proj->GetXaxis()->SetLabelSize(0.05);
    frm_proj->GetXaxis()->SetLabelOffset(0.014);
    frm_proj->GetXaxis()->CenterTitle();

    frm_proj->GetYaxis()->SetTitle("cos(#theta)");
    frm_proj->GetYaxis()->SetTitleSize(0.07);
    frm_proj->GetYaxis()->SetTitleOffset(0.6);
    frm_proj->GetYaxis()->SetLabelSize(0.05);
    frm_proj->GetYaxis()->SetLabelOffset(0.005);
    frm_proj->GetYaxis()->CenterTitle();

    //ana->Draw("TMath::Cos(genstheta/180.0*TMath::Pi()):gensphi>>frm_proj","","COLZ same");
    ana->Draw("TMath::Cos(gendtheta/180.0*TMath::Pi()):gendphi>>frm_proj","","COLZ same");

    //ana->Draw("genstheta:gensphi","","COLZ same");
    //proj->SaveAs("pos_cd.eps");

  }
}
