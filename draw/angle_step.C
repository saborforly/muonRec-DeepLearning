{
  const int n=1000;
  float xe[n]; //{400,1800,3600, 7400,9600, 13000,24000,50000,50010, 100000,100010,149800,170000,190000};
  float ye[n]; // {0.1,0.06,0.056,0.04,0.038,0.035,0.033,0.031,0.0081,0.008, 0.0044,0.0040,0.0040,0.0040};
  ifstream fe("/junofs/users/liuyan2016/muon-sim/sim-nn/data-draw/draw/eval_angle_mean.csv");
  //ifstream fe("/junofs/users/liuyan2016/muon-sim/sim-nn/data-draw/draw/eval_angle_stddev.csv");
  string line_e;
  getline(fe, line_e);
  int ie = 0;
  while (getline(fe, line_e)){
    istringstream ise(line_e);
    string field;
    vector<string> fields;
    while (getline(ise, field, ',')){
      fields.push_back(field);
    }
    
    xe[ie] = atof((fields[1].erase(fields[1].find_last_not_of("\t\r\n")+1)).c_str());
    ye[ie] = atof((fields[2].erase(fields[2].find_last_not_of("\t\r\n")+1)).c_str());
    cout<<ye[ie]<<endl;
    ie++;
    if(ie>=1000){break;}

  }



  TCanvas* c_alpha = new TCanvas("c_alpha", "c", 400, 300);
  TH1F* frm_alpha = c_alpha->DrawFrame(0, 0,200000, 5);
  frm_alpha->GetXaxis()->SetTitle("step"); 
  frm_alpha->GetYaxis()->SetTitle("loss");
  //frm_alpha->GetXaxis()->SetTitleSize(18);
  frm_alpha->GetXaxis()->SetTitleOffset(0.8);
  frm_alpha->GetXaxis()->SetTitleSize(14);
  frm_alpha->GetXaxis()->SetLabelSize(12);

  frm_alpha->GetYaxis()->SetTitleOffset(1);
  frm_alpha->GetYaxis()->SetTitleSize(14);
  frm_alpha->GetYaxis()->SetLabelSize(12);

  
  TGraph *gr2 = new TGraph (n, xe, ye);
  gr2->SetLineColor(kBlack);
  gr2->Draw("same");

  TLegend* leg = new TLegend(0.4,0.6,0.6,0.8);
  leg->Draw();

}
