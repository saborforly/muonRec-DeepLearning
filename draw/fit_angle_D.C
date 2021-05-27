{

  int n=1;
  TFile* f[1];
  f[0] = TFile::Open("/junofs/users/liuyan2016/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-ellip.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-darknoise.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-sat2000.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/sim-tt-rotrand-40k-fxl2-300k-vgg16-xjl.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-300k-sphere-4.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-cd-300k-vgg16.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anashow-tt-display.root");
  //f[0] = TFile::Open("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-randGeV.root");
  
  for(int i=0; i<n; i++){
    TTree* ana = (TTree*)f[i]->Get("ana");
    ///* 
    TCanvas* c_alpha = new TCanvas("c_alpha", "c", 400, 300);
    c_alpha->cd();
    TString hname_angle="#alpha";
    //TString selDesc_angle = TString::Format("genDist<%f&&energy>12000",17700.0);
    TString selDesc_angle = TString::Format("genDist>%f&&genDist<%f",14700.0,16700.0);
    //TString valname_angle = TString::Format("(angle)>>#alpha(50,0,2.5)");

    TString valname_angle = TString::Format("(angle)>>#alpha(50,0,2.0)");
    ana->Draw(valname_angle,selDesc_angle,"goff");
    TH1* h_angle = (TH1*)gDirectory->Get(hname_angle.Data());
    h_angle->Fit("gaus");
    //h_angle->Fit("landau");
    
    /*
    TF1* Mylan = new TF1("Mylan","[0]*exp(-[3]*( (x-[1])/[2] + exp(([1]-x)/[2])))",0, 2);
    Mylan->SetParameters(2.17474e+02, 2.37, 2.38, 0.5);
    h_angle->Fit("Mylan","RLS");
    func = h_angle->GetFunction("Mylan");
    mean = func->Eval(func.GetParameter(1));
    sigma = func->GetX(0.5*func->Eval(func->GetParameter(1)), func->GetParameter(1), 30)-func->GetX(0.5*func->Eval(func->GetParameter(1)), 0, func->GetParameter(1) );    
    std::cout<<"x0="<<func->GetX(0.5*func->Eval(func->GetParameter(1)), 0, func->GetParameter(1) )<<std::endl;
    std::cout<<"x1="<<func->GetX(0.5*func->Eval(func->GetParameter(1)), func->GetParameter(1), 5 )<<std::endl; 
    */
    h_angle->Draw("samepe");
    h_angle->GetXaxis()->SetTitle("#alpha [deg]");
    h_angle->GetYaxis()->SetTitle("Entries");
    c_alpha->SaveAs("fit-alpha.eps");
    gStyle->SetStatX(0.9); //0.98
    gStyle->SetStatY(0.9); //0.995
    gStyle->SetStatW(0.2); //0.2
    gStyle->SetStatH(0.16); //0.16

    //*/ 
    /* 
    TCanvas* c_dD = new TCanvas("c_dD", "c", 400, 300);
    c_dD->cd();
    //TString selDesc_D = TString::Format("genDist<%f&&energy>12000",17700.0);
    TString selDesc_D = TString::Format("genDist>%f&&genDist<%f",14700.0,16700.0);
    TString valname_D = TString::Format("(recDist-genDist)>>#Delta D(100,-200,250)");
    //TString valname_D = TString::Format("(recDist-genDist)>>#Delta D(100,-300,300)");
    ana->Draw(valname_D,selDesc_D,"goff");
    TString hname_D="#Delta D";
    TH1* h_D = (TH1*)gDirectory->Get(hname_D.Data());
    h_D->Draw("samepe");
    h_D->GetXaxis()->SetTitle("#Delta D [mm]");
    h_D->GetYaxis()->SetTitle("Entries");
    gStyle->SetStatX(0.9); //0.98
    gStyle->SetStatY(0.9); //0.995
    gStyle->SetStatW(0.2); //0.2
    gStyle->SetStatH(0.16); //0.16
    gStyle->SetOptStat("nmr");
    //h_D->GetXaxis()->CenterTitle();
    h_D->Fit("gaus");
    c_dD->SaveAs("fit-dD.eps");
    */
  }

  
}
