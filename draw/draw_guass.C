void draw_RMS(){
  const int n=2;
  TFile *f[n];
  f[0] = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-40k-fxl2-300k-vgg16.root");
  f[1] = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-300k-sphere-4.root");
  //f[2] = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-5k-fxl.root");
  //f[3] = new TFile("/junofs/users/liuyan/muon-sim/sim-nn/data-ana/anasim-tt-rotrand-5k-fxl2.root");
  double rangeDist = 16700;
  const int nbins= 10;                              
  double binWidth = rangeDist/nbins;                
  double totevts[nbins];                            
  double sucevts[nbins];
  double eff[nbins],eff1[nbins];                    
  double efferror[nbins],efferror1[nbins];          
  double binc[nbins],binc1[nbins],binc2[nbins];     
  double errbin[nbins] = {0}; 
  double dD[n][nbins],angle[n][nbins];
  for(int j=0; j<n; j++){
    TTree* ana = (TTree*)f[j]->Get("ana");
    for(int i=0; i<nbins; i++){
      double binb = i*binWidth;
      double bine = binb+binWidth;
      binc[i] = (binb+bine)/2;
      binc[i] /= 1000;
      
      TString selDesc = TString::Format("genDist<%f&&genDist>%f",bine,binb);
      double nevents = ana->GetEntries(selDesc);
      totevts[i] = nevents;
      selDesc += TString::Format("&&angle>=0");

      //dD mm->cm
      TString valname = TString::Format("(recDist-genDist)>>hD(500,-1500,1500)");
      TString hname="hD";
      ana->Draw(valname,selDesc,"goff");
      TH1* h = (TH1*)gDirectory->Get(hname.Data());
      dD[j][i]=h->GetStdDev();

      //angle deg
      TString valname = TString::Format("angle>>hangle(10,0,10)");
      TString hname="hangle";
      ana->Draw(valname,selDesc,"goff");
      TH1* h = (TH1*)gDirectory->Get(hname.Data());
      //angle[0][i]=h->GetMean();
      //angle[1][i]=h->GetMeanError();
      angle[j][i]=h->GetStdDev();
      cout<<angle[j][i]<<" ";
      //angle[3][i]=h->GetRMSError();
      
    }
  }
  EColor color[] = { 
    kBlue,
    kCyan-3,
    kOrange-1,
    kRed,
  };
  TString title[] = {
    "CNN",
    "spherical CNN",
    "tt-5k-fxl",
    "tt-5k-fxl2"
  }; 
  TGraphErrors* g_D[n];
  for(int i=0;i<n;i++){
    g_D[i] = new TGraphErrors(nbins,binc,angle[i],0,0);
    g_D[i]->SetTitle(title[i]);
    g_D[i]->GetYaxis()->SetTitle("resolution #Delta D[mm]");
    g_D[i]->GetXaxis()->SetTitle("D_{mc} [m]");
    g_D[i]->GetYaxis()->CenterTitle();
    g_D[i]->GetXaxis()->CenterTitle();
    g_D[i]->SetLineColor(color[i]);
    g_D[i]->SetMarkerColor(color[i]);
    g_D[i]->SetMarkerStyle(20);
  }
  TCanvas* c_dD = new TCanvas;
  c_dD->cd();
  c_dD->SetGridx();
  c_dD->SetGridy();
  g_D[0]->Draw();
  for (int i = 1; i <n; ++i) {
    c_dD->cd();
    g_D[i]->Draw("samePL");
  }
  c_dD->BuildLegend(0.18,0.5,0.45,0.90);
  //c_D->SaveAs(picsroot+"/dD_res.pdf");
}


