{
  const int n=1000;
  float x[n]; //{400,1800,3600, 7400,9600, 13000,24000,50000,50010, 100000,100010,149800,170000,190000};
  float y[n]; // {0.1,0.06,0.056,0.04,0.038,0.035,0.033,0.031,0.0081,0.008, 0.0044,0.0040,0.0040,0.0040};
  ifstream fin("/junofs/users/liuyan2016/muon-sim/sim-nn/data-draw/draw/run_learning_rate.csv");
  string line_t;
  getline(fin, line_t);
  int i = 0;
  while (getline(fin, line_t)){
    istringstream iss(line_t);
    string field;
    vector<string> fields;
    while (getline(iss, field, ',')){
      fields.push_back(field);
    }
    //cout<<fields[1]<<endl;
    
    x[i] = atof((fields[1].erase(fields[1].find_last_not_of("\t\r\n")+1)).c_str());
    y[i] = atof((fields[2].erase(fields[2].find_last_not_of("\t\r\n")+1)).c_str());
    if (x[i]==0){
      x[i]=x[i-1];
      y[i]=y[i-1];
    }
    //break;
    i++;
    if(i>=1000){break;}
    
  } 

  TCanvas* c_alpha = new TCanvas("c_alpha", "c", 400, 300);
  TH1F* frm_alpha = c_alpha->DrawFrame(0, 0,200000, 0.2);
  frm_alpha->GetXaxis()->SetTitle("Step"); 
  frm_alpha->GetYaxis()->SetTitle("Loss");
  //frm_alpha->GetXaxis()->SetTitleSize(18);
  frm_alpha->GetXaxis()->SetTitleOffset(0.8);
  frm_alpha->GetXaxis()->SetTitleSize(32);
  frm_alpha->GetXaxis()->SetLabelSize(28);

  frm_alpha->GetYaxis()->SetTitleOffset(0.8);
  frm_alpha->GetYaxis()->SetTitleSize(28);
  frm_alpha->GetYaxis()->SetRangeUser(0.00001,1);
  frm_alpha->GetYaxis()->SetLabelSize(28);
  frm_alpha->GetYaxis()->SetTitleOffset(1.2);

  TGraph *gr1 = new TGraph (n, x, y);
  gr1->SetLineColor(kRed);
  gr1->Draw();
  
  c_alpha->SaveAs("lr_step.png");

}
