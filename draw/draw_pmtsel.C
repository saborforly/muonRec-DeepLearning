TString picsroot = "/junofs/users/liuyan/muon-sim/sim-nn/data-draw/";
//TFile* f = new TFile("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtad_qc100_tc300flists5/rec_total.root");
//TFile* f = new TFile("/junofs/users/zhangk/muon-rec/work-0602/rec/data/v2_corr2dtad_qc100_tc300_pmtsel2_all3_10times/rec*.rootanatemp.root");
TFile* f = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
TTree* t = (TTree*)f->Get("ana");
void draw_pmtsel(){
  //double rangeDist = 17800-1047.1;
  int usewhichrec = 2;
  double rangeDist = 16700;
  const int nbins= 8; 
  double binWidth = rangeDist/nbins;
  double dx[4][nbins],dy[4][nbins],dz[4][nbins];
  double dthe[4][nbins],dphi[4][nbins];
  double totevts[nbins];
  double sucevts[nbins];
  double eff[nbins],eff1[nbins];
  double efferror[nbins],efferror1[nbins];
  double binc[nbins],binc1[nbins],binc2[nbins];
  double errbin[nbins] = {0};

  double dD[4][nbins],angle[4][nbins];
  double dD0[4][nbins],angle0[4][nbins];

  double shift = 0.3;
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
    selDesc += TString::Format("&&angle>=0",usewhichrec);

    //dx mm->cm
    TString valname = TString::Format("(recix-genix)/10>>hx(100,-25,25)",usewhichrec);
    //TString valname = TString::Format("(recix[%d]-genix)/10>>hx(1000,-150,150)",usewhichrec);
    TString valtitle = TString::Format("#Delta injectx[cm] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hx",selDesc,dx[0][i],dx[1][i],dx[2][i],dx[3][i]);

    //dy mm->cm
    valname = TString::Format("(reciy-geniy)/10>>hy(100,-25,25)",usewhichrec);
    //valname = TString::Format("(reciy[%d]-geniy)/10>>hy(1000,-150,150)",usewhichrec);
    TString valtitle = TString::Format("#Delta injecty[cm] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hy",selDesc,dy[0][i],dy[1][i],dy[2][i],dy[3][i]);

    //dz mm->cm
    //valname = TString::Format("(reciz[%d]-geniz)/10>>hz(1000,-150,150)",usewhichrec);
    valname = TString::Format("(reciz-geniz)/10>>hz(100,-25,25)",usewhichrec);
    TString valtitle = TString::Format("#Delta injectz[cm] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hz",selDesc,dz[0][i],dz[1][i],dz[2][i],dz[3][i]);

    //dD mm->cm
    valname = TString::Format("(recDist-genDist)/10>>hD(1000,-25,25)",usewhichrec);
    TString valtitle = TString::Format("#Delta D[cm] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hD",selDesc,dD[0][i],dD[1][i],dD[2][i],dD[3][i]);
    
    //dtheta deg
    valname = TString::Format("(gendtheta-recdtheta)>>hthe(100,-1,1)",usewhichrec);
    TString valtitle = TString::Format("#Delta #theta[deg] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hthe",selDesc,dthe[0][i],dthe[1][i],dthe[2][i],dthe[3][i]);

    //dphi deg
    valname = TString::Format("(gendphi-recdphi)>>hphi(100,-1,1)",usewhichrec);
    TString valtitle = TString::Format("#Delta #phi[deg] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hphi",selDesc,dphi[0][i],dphi[1][i],dphi[2][i],dphi[3][i]);

    //angle deg
    valname = TString::Format("angle>>hangle(100,0,10)",usewhichrec);
    TString valtitle = TString::Format("#alpha [deg] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hangle",selDesc,angle[0][i],angle[1][i],angle[2][i],angle[3][i]);
    //

    TString selDesc0 = TString::Format("genDist<%f&&genDist>%f&&angle>=0",bine,binb);
    //dD0 mm->cm
    valname = TString::Format("(recDist-genDist)/10>>hD0(1000,-150,150)",usewhichrec);
    TString valtitle = TString::Format("#Delta D0[cm] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hD0",selDesc0,dD0[0][i],dD0[1][i],dD0[2][i],dD0[3][i]);
    //angle0 deg
    valname = TString::Format("angle>>hangle0(100,0,10)",usewhichrec);
    TString valtitle = TString::Format("#alpha0 [deg] (%.2fm ~ %.2fm)",binb/1000,bine/1000);
    draw(valname,valtitle,"hangle0",selDesc,angle0[0][i],angle0[1][i],angle0[2][i],angle0[3][i]);

    // sucesss dx,dy,dz : cm -> mm with in 5*sigma
    //selDesc += TString::Format("&&TMath::Abs(recix[%d]-genix)<%f&&TMath::Abs(reciy[%d]-geniy)<%f&&TMath::Abs(reciz[%d]-geniz)<%f&&TMath::Abs(dtheta[%d])<%f&&TMath::Abs(dphi[%d]<%f)",usewhichrec,dx[2][i]*10*5,usewhichrec,dy[2][i]*10*5,usewhichrec,dz[2][i]*10*5,usewhichrec,dthe[2][i]*5,usewhichrec,dphi[2][i]*5);
    selDesc += TString::Format("&&TMath::Abs(recix-genix)<%f&&TMath::Abs(reciy-geniy)<%f&&TMath::Abs(reciz-geniz)<%f&&TMath::Abs(gendtheta)<%f&&TMath::Abs(gendphi<%f)",usewhichrec,100.,usewhichrec,100.,usewhichrec,100.,usewhichrec,1.,usewhichrec,1.);
    cout<<selDesc.Data()<<endl;

    double nsucc = ana->GetEntries(selDesc.Data());
    eff[i] = nsucc/nevents;
    efferror[i] = TMath::Sqrt(nsucc*(1-nsucc/nevents))/nevents;

    selDesc += TString::Format("&&TMath::Abs(recix-genix)<%f&&TMath::Abs(reciy-geniy)<%f&&TMath::Abs(reciz-geniz)<%f&&TMath::Abs(gendtheta)<%f&&TMath::Abs(gendphi<%f)",usewhichrec,dx[2][i]*10*5,usewhichrec,dy[2][i]*10*5,usewhichrec,dz[2][i]*10*5,usewhichrec,dthe[2][i]*5,usewhichrec,dphi[2][i]*5);
    cout<<selDesc.Data()<<endl;
    //sucess 
    TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb);
    selDesc += TString::Format("&&angle>=0",usewhichrec);
    selDesc += TString::Format("&&TMath::Abs(recDist-genDist-(%f))<%f&&TMath::Abs(angle-(%f))<%f",usewhichrec,dD[0][i]*10,dD[2][i]*10*5,usewhichrec,angle[0][i],angle[2][i]*5);

    nsucc = ana->GetEntries(selDesc.Data());
    eff1[i] = nsucc/nevents;
    efferror1[i] = TMath::Sqrt(nsucc*(1-nsucc/nevents))/nevents;
  }

  TCanvas *c_dD = new TCanvas;
  TGraphErrors* g_dD = new TGraphErrors(nbins,binc,dD[0],errbin,dD[1]);
  TGraphErrors* g_dD0 = new TGraphErrors(nbins,binc,dD0[0],errbin,dD0[1]);
  g_dD0->GetYaxis()->SetTitle("#DeltaD{rec-mc} [cm]");
  g_dD->GetYaxis()->SetTitle("#DeltaD{rec-mc} [cm]");
  g_dD->GetXaxis()->SetTitle("D_{mc} [m]");
  g_dD0->GetXaxis()->SetTitle("D_{mc} [m]");
  g_dD->SetTitle("corr 2d-ad");
  g_dD0->SetTitle("no corr");
  g_dD0->GetYaxis()->SetRangeUser(-20,50);
  g_dD->GetYaxis()->SetRangeUser(-20,50);
  g_dD0->SetLineColor(kBlue);
  g_dD->SetLineColor(kRed);
  g_dD0->SetMarkerColor(kBlue);
  g_dD->SetMarkerColor(kRed);
  g_dD->SetFillColor(kWhite);
  g_dD0->SetFillColor(kWhite);
  g_dD->SetMarkerStyle(20);
  g_dD0->SetMarkerStyle(21);
  c_dD->SetGridx();
  c_dD->SetGridy();
  g_dD0->Draw();
  g_dD->Draw("samePL");
  c_dD->BuildLegend(0.5,0.50,0.80,0.70);
  g_dD0->SetTitle("");
  g_dD->SetTitle("");
  c_dD->SaveAs(picsroot+"/dD-D-auto.pdf");

  TCanvas *c_angle = new TCanvas;
  TGraphErrors* g_angle = new TGraphErrors(nbins,binc,angle[0],errbin,angle[1]);
  TGraphErrors* g_angle0 = new TGraphErrors(nbins,binc,angle0[0],errbin,angle0[1]);
  g_angle0->GetYaxis()->SetTitle("#alpha [deg]");
  g_angle->GetYaxis()->SetTitle("#alpha [deg]");
  g_angle->GetXaxis()->SetTitle("D_{mc} [m]");
  g_angle0->GetXaxis()->SetTitle("D_{mc} [m]");
  g_angle->SetTitle("corr 2d-ad");
  g_angle0->SetTitle("no corr");
  g_angle0->GetYaxis()->SetRangeUser(-0.50,3.5);
  g_angle->GetYaxis()->SetRangeUser(-0.50,3.5);
  g_angle->SetMarkerStyle(20);
  g_angle0->SetMarkerStyle(21);
  g_angle0->SetLineColor(kBlue);
  g_angle->SetLineColor(kRed);
  g_angle0->SetMarkerColor(kBlue);
  g_angle->SetMarkerColor(kRed);
  g_angle->SetFillColor(kWhite);
  g_angle0->SetFillColor(kWhite);
  c_angle->SetGridx();
  c_angle->SetGridy();
  g_angle0->Draw();
  g_angle->Draw("samePL");
  c_angle->BuildLegend(0.4,0.40,0.70,0.60);
  g_angle0->SetTitle("");
  g_angle->SetTitle("");
  c_angle->SaveAs(picsroot+"/angle-D-auto.pdf");

  TCanvas *c_angle_res = new TCanvas;
  TGraphErrors* g_angle_res = new TGraphErrors(nbins,binc,angle[2],errbin,angle[3]);
  TGraphErrors* g_angle0_res = new TGraphErrors(nbins,binc,angle0[2],errbin,angle0[3]);
  g_angle0_res->GetYaxis()->SetTitle("#alpha [deg]");
  g_angle_res->GetYaxis()->SetTitle("#alpha [deg]");
  g_angle_res->GetXaxis()->SetTitle("D_{mc} [m]");
  g_angle0_res->GetXaxis()->SetTitle("D_{mc} [m]");
  g_angle_res->SetTitle("corr 2d-ad");
  g_angle0_res->SetTitle("no corr");
  g_angle0_res->GetYaxis()->SetRangeUser(0,1);
  g_angle_res->GetYaxis()->SetRangeUser(0,1);
  g_angle_res->SetMarkerStyle(20);
  g_angle0_res->SetMarkerStyle(21);
  g_angle0_res->SetLineColor(kBlue);
  g_angle_res->SetLineColor(kRed);
  g_angle0_res->SetMarkerColor(kBlue);
  g_angle_res->SetMarkerColor(kRed);
  g_angle_res->SetFillColor(kWhite);
  g_angle0_res->SetFillColor(kWhite);
  c_angle_res->SetGridx();
  c_angle_res->SetGridy();
  g_angle0_res->Draw();
  g_angle_res->Draw("samePL");
  c_angle_res->BuildLegend(0.4,0.60,0.70,0.80);
  g_angle0_res->SetTitle("");
  g_angle_res->SetTitle("");
  c_angle_res->SaveAs(picsroot+"/angle_res-D-auto.pdf");

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
  c_eff->SaveAs(picsroot+"/eff.pdf");

  TCanvas *c_eff1 = new TCanvas;
  TGraphErrors* g_eff1 = new TGraphErrors(nbins,binc,eff1,errbin,efferror1);
  g_eff1->GetYaxis()->SetTitle("Tracking Effeciency");
  g_eff1->SetMarkerStyle(20);
  g_eff1->GetXaxis()->SetTitle("D_{mc} [m]");
  g_eff1->SetTitle("");
  g_eff1->Draw();
  c_eff1->SetGridx();
  c_eff1->SetGridy();
  c_eff1->SaveAs(picsroot+"/eff_with_angle_Dist_cut.pdf");
  
  TCanvas* c_dxdydz =  new TCanvas;
  TGraphErrors* g_dx = new TGraphErrors(nbins,binc,dx[2],errbin,dx[3]);
  TGraphErrors* g_dy = new TGraphErrors(nbins,binc1,dy[2],errbin,dy[3]);
  TGraphErrors* g_dz = new TGraphErrors(nbins,binc2,dz[2],errbin,dz[3]);
  g_dx->SetTitle("#Delta injection x");
  g_dx->GetXaxis()->SetTitle("D_{mc} [m]");
  g_dx->GetYaxis()->SetTitle("Spatial Resolution [cm]");
  g_dy->SetTitle("#Delta injection y");
  g_dz->SetTitle("#Delta injection z");
  g_dx->SetMarkerStyle(20);
  g_dy->SetMarkerStyle(20);
  g_dz->SetMarkerStyle(20);
  g_dx->SetLineStyle(2);
  g_dy->SetLineStyle(2);
  g_dz->SetLineStyle(2);
  g_dx->SetMarkerSize(1.5);
  g_dy->SetMarkerSize(1.5);
  g_dz->SetMarkerSize(1.5);
  //TF1* fx  = new TF1("fx","[0]+[1]*x+[2]*x*x",0,17000);
  //TF1* fy  = new TF1("fy","[0]+[1]*x+[2]*x*x",0,17000);
  //TF1* fz  = new TF1("fz","[0]+[1]*x+[2]*x*x",0,17000);
  //TF1* fx  = new TF1("fx","[0]+[1]*x",0,17000);
  //TF1* fy  = new TF1("fy","[0]+[1]*x",0,17000);
  //TF1* fz  = new TF1("fz","[0]+[1]*x",0,17000);
  //g_dx->Fit(fx);
  //g_dy->Fit(fy);
  //g_dz->Fit(fz);

  EColor color[] = {kBlue,kRed,kGreen+2}
  g_dx->SetLineColor(color[0]);
  g_dy->SetLineColor(color[1]);
  g_dz->SetLineColor(color[2]);
  g_dx->SetMarkerColor(color[0]);
  g_dy->SetMarkerColor(color[1]);
  g_dz->SetMarkerColor(color[2]);
  g_dx->SetFillColor(kWhite);
  g_dy->SetFillColor(kWhite);
  g_dz->SetFillColor(kWhite);

  g_dx->GetYaxis()->CenterTitle();
  g_dx->Draw();
  g_dy->Draw("samePL");
  g_dz->Draw("samePL");

  c_dxdydz->BuildLegend(0.2,0.67,0.58,0.88);
  g_dx->SetTitle("");
  c_dxdydz->SetGridx();
  c_dxdydz->SetGridy();
  c_dxdydz->SaveAs(picsroot+"/res-dxdydz.pdf");

  TCanvas* c_dir =  new TCanvas;
  TGraphErrors* g_dthe = new TGraphErrors(nbins,binc,dthe[2],errbin,dthe[3]);
  TGraphErrors* g_dphi = new TGraphErrors(nbins,binc,dphi[2],errbin,dphi[3]);
  g_dthe->SetTitle("#Delta#theta");
  g_dphi->SetTitle("#Delta#phi");
  g_dthe->SetLineColor(kRed);
  g_dthe->SetMarkerColor(kRed);
  g_dphi->SetLineColor(kBlue);
  g_dphi->SetMarkerColor(kBlue);
  g_dphi->SetMarkerSize(1.5);
  g_dthe->SetMarkerSize(1.5);
  g_dphi->SetMarkerStyle(21);
  g_dthe->SetMarkerStyle(21);
  g_dphi->SetFillColor(kWhite);
  g_dthe->SetFillColor(kWhite);
  //g_dphi->GetXaxis()->SetTitle("D_{mc} [m]");
  //g_dphi->GetYaxis()->SetTitle("Angular Resolution [deg]");
  //g_dphi->GetYaxis()->CenterTitle();
  g_dphi->Draw();
  g_dthe->Draw("samePL");
  c_dir->BuildLegend(0.2,0.67,0.50,0.88);
  g_dphi->SetTitle("");
  c_dir->SetGridx();
  c_dir->SetGridy();
  c_dir->SaveAs(picsroot + "/res-dir.pdf");

  TCanvas* c_res =  new TCanvas;
  double small = 1e-5;
  c_res->Divide(1,2,small,small);
  c_res->cd(1);
  c_res->cd(1)->SetBottomMargin(small);
  gPad->SetPad(0.,0.5,1.,1.);
  h2 =new TH1F("h2","",17,0,17);
  for(int i=0;i<17;i++){
    h2->SetBinContent(i+1,0);
  }
  h2->Draw();
  h2->SetLineWidth(1);
  h2->GetYaxis()->CenterTitle();
  h2->GetYaxis()->SetNdivisions(505);
  h2->SetTitle("");
  h2->GetYaxis()->CenterTitle();
  h2->GetYaxis()->SetTitle("Spatial [cm]");
  h2->GetYaxis()->SetRangeUser(1.01,6);
  h2->GetXaxis()->SetRangeUser(0,17);
  h2->GetYaxis()->SetTitleOffset(0.5);
  h2->GetYaxis()->SetTitleSize(0.12);
  h2->GetYaxis()->SetLabelSize(0.12);
  h2->Draw();
  
  g_dx->GetYaxis()->CenterTitle();
  g_dx->GetYaxis()->SetTitle("Spatial [cm]");
  g_dx->GetYaxis()->SetRangeUser(1.01,6);
  g_dx->GetXaxis()->SetRangeUser(0,17);
  g_dx->GetYaxis()->SetTitleOffset(0.5);
  g_dx->GetYaxis()->SetTitleSize(0.12);
  g_dx->GetYaxis()->SetLabelSize(0.12);
  g_dx->Draw("PL");

  g_dy->GetYaxis()->CenterTitle();
  g_dy->GetYaxis()->SetTitle("Spatial [cm]");
  g_dy->GetYaxis()->SetRangeUser(1.01,6);
  g_dy->GetXaxis()->SetRangeUser(0,17);
  g_dy->GetYaxis()->SetTitleOffset(0.5);
  g_dy->GetYaxis()->SetTitleSize(0.12);
  g_dy->GetYaxis()->SetLabelSize(0.12);
  g_dy->Draw("PL");

  g_dz->GetYaxis()->CenterTitle();
  g_dz->GetYaxis()->SetTitle("Spatial [cm]");
  g_dz->GetYaxis()->SetRangeUser(1.01,6);
  g_dz->GetXaxis()->SetRangeUser(0,17);
  g_dz->GetYaxis()->SetTitleOffset(0.5);
  g_dz->GetYaxis()->SetTitleSize(0.12);
  g_dz->GetYaxis()->SetLabelSize(0.12);
  g_dz->Draw("PL");
  TLegend l_xyz(0.2,0.47,0.58,0.88);
  l_xyz.AddEntry(g_dx,"#Delta injecting x","L");
  l_xyz.AddEntry(g_dy,"#Delta injecting y","L");
  l_xyz.AddEntry(g_dz,"#Delta injecting z","L");
  l_xyz.Draw();
  c_res->cd(2);
  c_res->cd(2)->SetTopMargin(small);
  c_res->cd(2)->SetBottomMargin(0.25);
  gPad->SetPad(0.,0.,1.,0.5);

  h1 =new TH1F("h1","",17,0,17);
  for(int i=0;i<17;i++){
    h1->SetBinContent(i+1,0);
  }
  h1->Draw();
  h1->SetLineWidth(1);
  h1->GetYaxis()->CenterTitle();
  h1->GetYaxis()->SetNdivisions(505);
  h1->SetTitle("");
  h1->GetYaxis()->SetTitle("Angular [deg]");
  h1->GetYaxis()->SetRangeUser(0,0.9);
  h1->GetYaxis()->SetTitleOffset(0.5);
  h1->GetYaxis()->SetTitleSize(0.12);
  h1->GetYaxis()->SetLabelSize(0.12);
  h1->GetXaxis()->SetRangeUser(0,17);
  h1->GetXaxis()->SetTitle("D_{mc} [m]");
  h1->GetXaxis()->SetTitleOffset(0.85);
  h1->GetXaxis()->SetTitleSize(0.12);
  h1->GetXaxis()->SetLabelSize(0.12);
  h1->GetXaxis()->CenterTitle();
  h1->Draw();

  g_dphi->GetYaxis()->SetTitle("Angular [deg]");
  g_dphi->GetYaxis()->SetRangeUser(0,1.1);
  g_dphi->GetYaxis()->SetTitleOffset(0.5);
  g_dphi->GetYaxis()->SetTitleSize(0.12);
  g_dphi->GetYaxis()->SetLabelSize(0.12);
  g_dphi->GetXaxis()->SetRangeUser(0,17);
  g_dphi->GetXaxis()->SetTitle("D_{mc} [m]");
  g_dphi->GetXaxis()->SetTitleOffset(0.85);
  g_dphi->GetXaxis()->SetTitleSize(0.12);
  g_dphi->GetXaxis()->SetLabelSize(0.12);
  g_dphi->GetXaxis()->CenterTitle();

  g_dthe->GetYaxis()->SetTitle("Angular [deg]");
  g_dthe->GetYaxis()->SetRangeUser(0,1.1);
  g_dthe->GetYaxis()->SetTitleOffset(0.5);
  g_dthe->GetYaxis()->SetTitleSize(0.12);
  g_dthe->GetYaxis()->SetLabelSize(0.12);
  g_dthe->GetXaxis()->SetRangeUser(0,17);
  g_dthe->GetXaxis()->SetTitle("D_{mc} [m]");
  g_dthe->GetXaxis()->SetTitleOffset(0.85);
  g_dthe->GetXaxis()->SetTitleSize(0.12);
  g_dthe->GetXaxis()->SetLabelSize(0.12);
  g_dthe->GetXaxis()->CenterTitle();

  g_dphi->Draw("same PL");
  g_dthe->Draw("same PL");
  TLegend l_thephi(0.3,0.57,0.60,0.88);
  l_thephi.AddEntry(g_dthe,"#Delta#theta","L");
  l_thephi.AddEntry(g_dphi,"#Delta#phi","L");
  l_thephi.Draw();
  c_res->SetGridx();
  c_res->SetGridy();
  c_res->SaveAs(picsroot + "res.pdf");

  /*
     for (int i = 0; i < nbins; ++i) {
     double sigmax = fx->Eval(binc[i]);
     double sigmay = fy->Eval(binc[i]);
     double sigmaz = fz->Eval(binc[i]);
     TString selDesc = TString::Format("angle[1]>=0&&genDist>%f&&genDist<%f&&TMath::Abs(recix[1]-genix)<%f&&TMath::Abs(reciy[1]-geniy)<%f&&TMath::Abs(reciz[1]-geniz[1])<<%f&&TMath::Abs(dtheta)<%f&&TMath::Abs(dphi)<%f",i*binWidth,(i+1)*binWidth,sigmax*10*6,sigmay*10*6,sigmaz*10*6,dthe[2][i]*6,dphi[2][i]*6);
     cout<<selDesc.Data()<<endl;
     sucevts[i] = ana->GetEntries(selDesc.Data());
     eff[i] = sucevts[i]/totevts[i];
     efferror[i] = TMath::Sqrt(sucevts[i]*(1-sucevts[i]/totevts[i]))/totevts[i];
     }
     TCanvas *c_eff2 = new TCanvas;
     TGraphErrors* g_eff2 = new TGraphErrors(nbins,binc,eff,errbin,efferror);
     g_eff2->SetTitle("Tracking Effeciency");
     g_eff2->GetXaxis()->SetTitle("D_{mc} [m]");
     g_eff2->Draw();
     c_eff2->SaveAs(picsroot+"/eff2.pdf");
     */
}

void draw(TString drawDesc, TString title, TString hname, TString selDesc, double &mean,double &meanerror,double& sigma,double& sigmaerror){
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
