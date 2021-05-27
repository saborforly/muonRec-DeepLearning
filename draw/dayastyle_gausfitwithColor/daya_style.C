{
cout << endl << "Daya Bay Experiment plot defaults." << endl;

//
// KAJ 08-DEC-2006
// Based on a style from ATLAS,
// which was based on a style file from BaBar
// and then lightly seasoned with input from Liu Jianglai
//

//Creating new style "DAYA"
TStyle *dayaStyle= new TStyle("DAYA","Daya Bay style");

// use plain black on white colors
Int_t icol=0;
dayaStyle->SetFrameBorderMode(icol);
dayaStyle->SetCanvasBorderMode(icol);
dayaStyle->SetPadBorderMode(icol);
dayaStyle->SetPadColor(icol);
dayaStyle->SetCanvasColor(icol);
dayaStyle->SetStatColor(icol);
dayaStyle->SetTitleFillColor(icol);

dayaStyle->SetPalette(1); // set a good color palette 
//dayaStyle->SetFillColor(0); // explicitly don't fill

// set the paper & margin sizes
dayaStyle->SetPaperSize(TStyle::kUSLetter);

//Margins, give some additional space on the Left/Bottom where axis labels go
dayaStyle->SetPadTopMargin(.05);
//dayaStyle->SetPadLeftMargin(.12);
dayaStyle->SetPadLeftMargin(.15);
dayaStyle->SetPadRightMargin(.15);
//dayaStyle->SetPadBottomMargin(.12);
dayaStyle->SetPadBottomMargin(.15);

// set master font
Int_t font=132;      //times new roman, regular (13), high precision (2)
//Int_t font=42;     //tnr, bold (4), high precision (2)

//set master font size
Double_t tsize=11./220; //should be set between 0.03-0.05, is in units of "% of pad"
Double_t titlesize=13./220; //should be set between 0.03-0.05, is in units of "% of pad"

// Text, Labels, Titles
// All set the same here
dayaStyle->SetTextFont(font);
dayaStyle->SetTextSize(titlesize);

dayaStyle->SetLabelFont(font,"xyz");
dayaStyle->SetLabelSize(tsize,"xyz");
dayaStyle->SetLabelOffset(0.01,"xyz");

dayaStyle->SetTitleFont(font,"xyz");
dayaStyle->SetTitleSize(titlesize,"xyz");
dayaStyle->SetTitleYOffset(1.2); //offset the title of y axis a bit
dayaStyle->SetTitleBorderSize(1);

//use bold lines and markers
dayaStyle->SetMarkerStyle(20);
dayaStyle->SetMarkerSize(0.15);
dayaStyle->SetHistLineWidth(2.);
dayaStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

//get rid of X error bars and y error bar caps
//dayaStyle->SetErrorX(0.001);

//do not display any of the standard histogram decorations
//dayaStyle->SetOptTitle(1000);     //no title box
dayaStyle->SetOptTitle(0);     //no title box
//dayaStyle->SetOptStat(1111); //standard stat info
dayaStyle->SetOptStat(0);    //no stat info
//dayaStyle->SetOptStat(0000);     //number of entries only
dayaStyle->SetOptFit(1111);  //standard fit info
//dayaStyle->SetOptFit(0);       //no fit info

//legend and stat pad
dayaStyle->SetStatBorderSize(1);
dayaStyle->SetStatFont(132);
dayaStyle->SetStatX(0.95);
dayaStyle->SetStatY(0.95);
dayaStyle->SetLegendBorderSize(1);

// put tick marks on top and RHS of plots
//dayaStyle->SetPadTickX(1);
//dayaStyle->SetPadTickY(1);
// tick marks only on left and bottom
dayaStyle->SetPadTickX(0);
dayaStyle->SetPadTickY(0);

// additional series styles (Thanks, Jianglai)
  //   1 = solid
  //   2 = long dash (30 10)
  //   3 = dotted (4 8)
  //   4 = dash-dot (15 12 4 12)
  //   5 = short dash ( 15 15 )
  //   6 = dash-dot-dot   gStyle->SetLineStyleString(1,"[]");
  dayaStyle->SetLineStyleString(2,"[30 10]");
  dayaStyle->SetLineStyleString(3,"[4 8]");
  dayaStyle->SetLineStyleString(4,"[15 12 4 12]");
  dayaStyle->SetLineStyleString(5,"[15 15]");
  dayaStyle->SetLineStyleString(6,"[15 12 4 12 4 12]");
  dayaStyle->SetOptDate(0);
  dayaStyle->SetDateY(.98);
  dayaStyle->SetStripDecimals(kFALSE);

//  dayaStyle->SetEndErrorSize(5.0); //make the end of error bar longer 


gROOT->SetStyle("DAYA");
gROOT->ForceStyle();    
}

