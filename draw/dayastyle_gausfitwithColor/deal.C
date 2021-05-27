double Q0;
double funcgaus(double *x,double *par);
void deal(){

//////////////////////////////////////////////////////////////////////

TString pmt="70222";//70226, 70187, 70220, 70222, 70219

TString infile = "pmt" + pmt + ".txt";
ifstream indat(infile);
double min=100000;
double max =0;
double pv= 0;
double aver = 0;
int nn=0;

double pv, res;

//TH1F *h1 = new TH1F("h1","h1",50,2.5,3.5); //for 70187, pv
//TH1F *h1 = new TH1F("h1","h1",50,2.,4.); //for 70226, pv
//TH1F *h1 = new TH1F("h1","h1",50,3.,5.); //for 70220, 70219 pv
TH1F *h1 = new TH1F("h1","h1",125,0.,5.); //for 70222
    if(indat.fail())
    {
      cout<<"Failed to open the file:"<<infile<<"!"<<endl;
      return -1;
    }
    while(!indat.eof())
    {
indat>>pv>>res;
cout<<nn<<", "<<pv<<" "<<res<<endl;
h1->Fill(pv);

if(pv>max) max = pv;
if(pv<min) min = pv;
aver+=pv;
nn++;
      indat.get();
      if(indat.peek()=='\n'){
        cout<<" All the file has been read correctly!"<<endl;
        break;
      }
}
cout<<" Total line: "<<nn<<endl;
aver /= nn;
double err = (max-min)/(aver);
cout<<" Min: "<<min<<", Max: "<<max<<endl;
cout<<"PV (max-min)/(aver): "<<err*100<<" %"<<endl;
//g1->Draw("alp");
TCanvas* c1 = new TCanvas("c1","good1",30,30,800,600);
    double count=20., mean=3.2, sigma=0.1;
   double fitrangeL = min-0.1;
   double  fitrangeH = max+0.1;
   //double fitrangeL = 3.2;
   //double  fitrangeH = 4.4;
     Q0=0;
     TF1 *fg = new TF1("fg",funcgaus,fitrangeL,fitrangeH,3);
    fg->SetParameters(count, mean, sigma);
    fg->SetParNames("C_{0}","Mean", "#sigma");
    fg->SetParLimits(1,3,5);
    fg->SetParLimits(2,0.,1);
h1->GetXaxis()->SetTitle("PV ratio");
h1->GetYaxis()->SetTitle("Counts");
h1->GetYaxis()->SetTitleOffset(0.8);
h1->GetXaxis()->CenterTitle();
h1->GetYaxis()->CenterTitle();

h1->Draw();
h1->Fit("fg","","",fitrangeL,fitrangeH);
    fg->Draw("same");
    fg->SetLineColor(2);

cout<<" Mean: "<<h1->GetMean()<<endl;
double fitmean = fg->GetParameter(1);
double fitsigma = fg->GetParameter(2);
    cout<<"Peak Count: "<<fg->GetParameter(0)<<" +-"<<fg->GetParError(0)<<endl;
    cout<<"Mean      : "<<fg->GetParameter(1)<<" +-"<<fg->GetParError(1)<<endl;
    cout<<"Sigma     : "<<fg->GetParameter(2)<<" +-"<<fg->GetParError(2)<<endl;

   cout<<" 4*sigma: "<<4*fitsigma<<", range [-4sigma, 4sigma]: "<<endl;
   cout<<"     ["<<fitmean-4*fitsigma<<", "<<fitmean+4*fitsigma<<"]"<<endl;
   cout<<" Min: "<<min<<", Max: "<<max<<endl;
   if(min < fitmean-4*fitsigma) cout<<" WARNING!, min < fitmean-4*fitsigma!!!"<<endl;
   if(max > fitmean+4*fitsigma) cout<<" WARNING!, max > fitmean+4*fitsigma!!!"<<endl;
   cout<<pmt<<" "<<fitmean<<" "<<fitsigma<<" "<<fitmean-4*fitsigma<<" "<<fitmean+4*fitsigma<<" "<<min<<" "<<max<<endl;



  fg->Update();
    c1->Modified();
    c1->Update();


indat.close();

}
double funcgaus(double *x,double *par)
{
  Double_t Delsq = par[2]*par[2];
  Double_t fgaus = TMath::Exp(-(x[0]-par[1]-Q0)*(x[0]-par[1]-Q0)/2./Delsq);///TMath::Sqrt(2*Delsq*TMath::Pi());
  fgaus = par[0]*fgaus;
  return fgaus;
}
