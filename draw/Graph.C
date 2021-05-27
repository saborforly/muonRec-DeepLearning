{
Int_t n = 6;
Float_t x[n] = {1000000,2000000,3000000,4000000,5000000,6000000};

/*
Float_t y0[n] = {31.086,65.262,96.065,124.613,146.248,173.426};
Float_t y1[n] = {16.164,33.424,48.425,61.716,73.994,87.551};
Float_t y2[n] = {6.755,13.588,20.779,26.289,30.449,35.704};
Float_t y3[n] = {3.781,7.503,11.022,14.52,16.652,19.139};
*/

/*
Float_t y0[n] = {1,1,1,1,1,1};
Float_t y1[n] = {1.92,1.95,1.96,1.97,1.97,1.98};
Float_t y2[n] = {4.6,4.7,4.7,4.7,4.8,4.81};
Float_t y3[n] = {8.2,8.4,8.4,8.5,8.8,9.06};
*/

//Int_t n=7;
//Float_t x[n] = {400000,800000,1200000,1600000,2000000,2400000,2800000};
//Float_t y0[n]={10.9076,22.352,33.409,43.65,53.367,62.898,72.675};
//Float_t y1[n]={0.832,1.597,2.523,3.229,4.169,4.913,5.6688};

/*
Int_t n=7;
Float_t x[n] = {400000,800000,1200000,1600000,2000000,2400000,2800000};
Float_t y0[n] = {1,1,1,1,1,1,1};
Float_t y1[n] = {13.1, 13.3, 13.3, 13.6, 13.82, 13.91, 14.23};
*/

//Int_t n=5;
//Float_t x[n] = {2000000, 4000000, 8000000, 16000000, 32000000};
//Float_t y0[n] = {1.54, 1.73, 1.87, 1.88, 1.9};
//Float_t y1[n] = {2.57, 2.91, 3.109, 3.49, 3.623};
//
/*
Int_t n=5;
Float_t x[n] = {100, 200, 400, 800, 1400};
Float_t y0[n] = {1,1,1,1,1};
Float_t y1[n] = {1.68,1.85,1.94,1.97,1.97};
Float_t y2[n] = {2.6,2.9,3.2,3.5,3.8};
Float_t y3[n] = {3.3,4.0,5.1,5.8,6.2};
*/
Int_t n=5;
Float_t x[n] = {100, 200, 400, 800, 1400};
Float_t y0[n] = {5.9, 10.5, 20.5,39,69};
Float_t y1[n] = {3.5,6.08,10.9,20.9,35.5 };
Float_t y2[n] = {2.27, 3.6,6.3,11.3,19.26};
Float_t y3[n] = {1.75,2.6,3.99,6.8,11.4 };


EColor color[] = {
    kBlack,
    kRed,
    kBlue,
    kPink+9,
    kOrange,
};
int ms[] = {
    24,
    25,
    26,
    27,
    28,
    30,
};

TGraph *gr0 = new TGraph(n,x,y0);
gr0->SetMarkerStyle(ms[0]);
gr0->SetMarkerColor(color[0]);
gr0->SetLineColor(color[0]);
gr0->Draw();

TGraph *gr1 = new TGraph(n,x,y1);
gr1->SetMarkerStyle(ms[1]);
gr1->SetMarkerColor(color[1]);
gr1->SetLineColor(color[1]);
//gr1->GetYaxis()->SetRangeUser(1,80);
//gr1->Draw("same PL");
gr1->Draw();

TGraph *gr2 = new TGraph(n,x,y2);
gr2->SetMarkerStyle(ms[2]);
gr2->SetMarkerColor(color[2]);
gr2->SetLineColor(color[2]);
gr2->Draw();

TGraph *gr3 = new TGraph(n,x,y3);
gr3->SetMarkerStyle(ms[3]);
gr3->SetMarkerColor(color[3]);
gr3->SetLineColor(color[3]);
gr3->Draw();


TMultiGraph *mg = new TMultiGraph();
mg.Add(gr0);
mg.Add(gr1);
mg.Add(gr2);
mg.Add(gr3);
mg.Draw("ALP");

//TString title[] = {"Full Data","50% selection","20% selection","10% selection"};
//TString title[] = {"normal","Index"};
TString title[] = {"1 Node","2 Nodes", "4 Nodes", "8 Nodes"};
TLegend* leg = new TLegend(0.4,0.6,0.6,0.8);
leg->AddEntry(gr0,title[0],"PL");
leg->AddEntry(gr1,title[1],"PL");
leg->AddEntry(gr2,title[2],"PL");
leg->AddEntry(gr3,title[3],"PL");
leg->SetX1NDC(0.01);
leg->SetX2NDC(0.9);
leg->Draw();

}
