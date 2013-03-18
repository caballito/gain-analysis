/*********************************************************
 * Perform analysis of both GridPix and scope data       *
 * from merged root files using the fit of the waveform. *
 *                 --  Rolf Schön, August/September 2012 *
 *    - adjusted paths for Rolf's laptop.   --  Feb 2013 *
 *********************************************************/

#include "GridPix.hpp"
#include "Waveform.hpp"

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TEntryList.h"
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TMath.h"
#include <sstream>
//namespace std ;

int analyse(TString subdir, TString file, TString gridV){
	GridPix* gridpix = new GridPix ;
	Waveform* wtf = new Waveform ;

	//TString dir = "/data/detrd/darwin/Data/GainMeasurements/ArIsobutane_9010/mergedrootfiles/" ;	// For Ar/iC4H10.
	//TString dir = "/data/detrd/darwin/Data/GainMeasurements/Argon4_7/mergedrootfiles/" ;	// For Ar 4.7.
	TString dirData = "/home/caballito/measurements/gain/data/" ;	// For Ar 4.7 on Rolf's laptop.
	TString fName = dirData + subdir + "/" + file ;
	TFile* f = new TFile(fName) ;
	//if ( f->IsOpen() ) {
	//	cout << "++ Processing " << fName << endl << "++ This may take a while. Take a break or do something funny :-)" << endl ;
	//} else return 0 ;
	TTree* tree = dynamic_cast<TTree*>(f->Get("tree")) ;
	tree->SetBranchAddress("det1",&gridpix) ;
	tree->SetBranchAddress("wf",&wtf) ;

	// Time window.
	std::stringstream tcut ;
	tcut << "3940<t&&t<3970" ;

	// Boundaries.
	int tmin = 3940, tmax = 3970 ;
	double xLow = 20., xHigh = 240., yLow = 25., yHigh = 225. ;
	double min = 0., max = 256. ;
	std::stringstream shapecut ;
	shapecut << "MinIf$(x,"<<tcut.str()<<") > 20 && MaxIf$(x,"<<tcut.str()<<") < 240 && MinIf$(y,"<<tcut.str()<<") > 25 && MaxIf$(y,"<<tcut.str()<<") < 225 " ;

	// Find out where to cut on the ratio of the x/y dimensions.
	tree->Draw("( MaxIf$(x,t>3940&&t<3970) - MinIf$(x,t>3940&&t<3970) ) : ( MaxIf$(y,t>3940&&t<3970) - MinIf$(y,t>3940&&t<3970) )>>hDimRatio(100,0,150,100,0,150)","Sum$(t>3940&&t<3970)>5") ;

	// Cut the circularity outside 2/3 and 3/2, respectively.
	// TODO: Find a (physically) sensible ratio from hDimRatio.
	shapecut << " && MaxIf$(x,"<<tcut.str()<<") - MinIf$(x,"<<tcut.str()<<") > 2./3.*(MaxIf$(y,"<<tcut.str()<<") - MinIf$(y,"<<tcut.str()<<")) && MaxIf$(x,"<<tcut.str()<<") - MinIf$(x,"<<tcut.str()<<") < 1.5*(MaxIf$(y,"<<tcut.str()<<") - MinIf$(y,"<<tcut.str()<<")) " ;

	// Cut small number of hits.
	shapecut << " && Sum$(t>3940&&t<3970)>5 " ;

	TH2D* hScatter = new TH2D("hScatter",
			"scatter plot;x (pixels);y (pixels)",256,0,256,256,0,256) ;
	TH2D* hScatter_cut = new TH2D("hScatter_cut",
			"clean scatter plot;x (pixels);y (pixels)",256,0,256,256,0,256) ;
	TH1D* hNhits = new TH1D("hNhits","number of hits;hit pixels;entries",100,0,500) ;
	// TODO: Use calib to give charge in # of e-.
	TH1F* hQ = new TH1F("hQ","charge;waveform amplitude (V);entries",1000,0,0.1) ;
	TH2F* hQvsHits = new TH2F("hQvsHits",
			"charge vs. number of hits;hit pixels;waveform amplitude (V)",100,0,500,1000,0,0.1) ;

	int sum ;	// To count the number of hits per event.
	double x ;	// To calculate function values of oscFunc for hOsc.

	TEntryList* newentrylist = new TEntryList("newentrylist","cut on event shape and position") ;
	tree->Draw(">>newentrylist",(shapecut.str()).c_str(),"entrylist") ;
/*
	// Define fit function with parabolic instead of linear rise.
	TF1* fitterFunc = new TF1("fitterFunc","( (x>[0])*( (x<[1])*([2]-[3]*(x-[1])**2) + [3]*([1]-[0])**2*(x>[1]) ) ) * (1.*(x<[1])+TMath::Exp(([1]-x)/[4])*(x>[1])) + ([5]+[6]*TMath::Sin(2*TMath::Pi()*(x-[7])/[8]))",0,12500) ;
	//double parset[9] = {6350.,7950.,2.5,2.35e-6, 800, -0.11,0.35,-300.,6500.} ;	// Overestimated because of wrong samples*vscale-offset (not as double).
	double parset[9] = {6350.,7950.,6e-2,1.5e-8, 800, -4e-4,1e-3,-300.,6500.} ;
	fitterFunc->SetParameters(parset) ;
	fitterFunc->SetParNames("t_rise","t_fall","height","p_rise","p_decay","offset sin","ampl sin","phase sin","period sin") ;

	// Define function without oscillation contribution.
	TF1* peakFunc = new TF1("peakFunc","( (x>[0])*( (x<[1])*([2]-[3]*(x-[1])**2) + [3]*([1]-[0])**2*(x>[1]) ) ) * (1.*(x<[1])+TMath::Exp(([1]-x)/[4])*(x>[1]))",0,12500) ;
	double parset2[5] = {} ;
	peakFunc->SetParameters(parset2) ;
	peakFunc->SetParNames("t_rise","t_fall","height","p_rise","p_decay") ;
*/
	// Define oscillation function only.
/*	TF1* oscFunc = new TF1("oscFunc","[0]+[1]*TMath::Sin(2*TMath::Pi()*(x-[2])/6500.)",0,12500) ;
	double parset3[3] = {-4e-4, 1e-3, -300} ; //, 6500} ;
	oscFunc->SetParameters(parset3) ;
	oscFunc->SetParNames("offset sin","ampl sin","phase sin") ; //,"period sin") ;
*/
	////////////////////////////////////////
	// Loop over the entries of the tree. //
	////////////////////////////////////////

	//cout << "Working on event " ;	
		
	for (int i=newentrylist->GetEntry(0); i>=0; i=newentrylist->Next()){	// To already apply the shapecut.
		//if (i>10) return 0 ;
		//int i = 8 ;
		tree->GetEntry(i) ;
		//if (i%20 == 0) cout << i << " ... " ;	

		// Histogram for number of hits.
		sum = 0 ;
		for (int ipixel=0; ipixel < gridpix->nhits; ipixel++){
			hScatter->Fill(gridpix->x[ipixel],gridpix->y[ipixel]) ;	
			if ( tmin<gridpix->t[ipixel] && gridpix->t[ipixel]<tmax ){
				sum++ ;
				hScatter_cut->Fill(gridpix->x[ipixel],gridpix->y[ipixel]) ;
			}
		}
		hNhits->Fill(sum) ;

/*		iTH1* oneWaveform = wtf->MakeTH1() ;	// Fills histogram with waveform. NOTE: already in good units.
		TH1F* hOsc = new TH1F("hOsc","hOsc",wtf->nsamples,0,wtf->nsamples) ;
		TH1F* hDiff = new TH1F("hDiff","hDiff",wtf->nsamples,0,wtf->nsamples) ;
*/
/*
		// Start values for the parameter set to help the fit.
		parset[1] = (double)(oneWaveform->GetMaximumBin()) ;  // Define start value for peak position.
		//parset[5] = (double)(allWaveforms->GetBinContent(20)) / 10000 ; // Define start value for sine offset.
		oneWaveform->GetXaxis()->SetRangeUser(0,5000) ; // Just look in the first part.
		parset[6] = (double)(oneWaveform->GetBinContent(oneWaveform->GetMaximumBin())) ;//- parset[5] ; // Define start value for sine amplitude.
		oneWaveform->GetXaxis()->SetRangeUser(0,12500) ; // Reset to full range.

		oneWaveform->Fit("fitterFunc","","",0,12000) ;
		fitterFunc->GetParameters(parset) ;

		for (int p=0; p<5; p++) {	// Copy parameter for second function.
			parset2[p] = parset[p] ;
			//cout << parset[p] << "  " << parset2[p] << endl ;
		}
		peakFunc->SetParameters(parset2) ;
		//peakFunc->SetLineColor(kRed) ;
		//peakFunc->Draw("same") ;
		//cout << "The integral is " << peakFunc->Integral(6000,12000,parset2) << endl ;
*/		
/*		oscFunc->SetParameters(parset3) ;
		oneWaveform->GetXaxis()->SetRangeUser(0,5000) ; // Just look in the first part.
		parset3[1] = (double)(oneWaveform->GetBinContent(oneWaveform->GetMaximumBin())) ;	// Define start value for sine amplitude.
		oneWaveform->GetXaxis()->SetRangeUser(0,12500) ; // Reset to full range.
		
		//oneWaveform->Draw() ;
		oneWaveform->Fit("oscFunc","Q","",10,6000) ;
		oscFunc->GetParameters(parset3) ;
		// Fill second histo with function values.
		for (int ibin=0; ibin<wtf->nsamples; ibin++){
			x = static_cast<double>(ibin) ;
			hOsc->SetBinContent(ibin+1,oscFunc->Eval(x)) ;
		}
		hDiff->Add(oneWaveform,hOsc,1,-1) ;
		
		//double peakInt = peakFunc->Integral(6000,12000,parset2) ;
		double peakInt = hDiff->Integral(6500,9500,"width") ;
		//double peakPosition = oneWaveform->GetMaximumBin() ;
		//double peakHeight = oneWaveform->GetBinContent(oneWaveform->GetMaximumBin()) - (parset[5] + parset[6] * TMath::Sin( 2*TMath::Pi()*peakPosition/parset[8] )) ; // Substract the value of the sine over there.

		hQ->Fill(peakInt) ;
		hQvsHits->Fill(static_cast<double>(sum),peakInt) ;
*/			
		Double_t peakHeight = wtf->Peak(7000,9500) ;
		hQ->Fill(peakHeight) ;	// TODO: Calculate real charge from peak with calibration.
		hQvsHits->Fill(static_cast<Double_t>(sum),peakHeight) ;	// TODO: Same here.

		//delete oneWaveform ;	// Clean up.
		//delete hOsc ;
		//delete hDiff ;
			
	}
	
	TString dirResults = "/home/caballito/measurements/gain/result_files/" ;
	TString nameOut = dirResults + subdir + "_" + gridV + ".root" ;
	TFile* out = new TFile(nameOut,"recreate") ;
	if ( out->IsOpen() ){
		cout << "++ All the histograms will be written into " << nameOut << endl ;
	} else cout << "EE Couldn't create output file." << endl ;
	out->cd() ;

	hNhits->Write() ;
	hQ->Write() ;
	hQvsHits->Write() ;
	hScatter->Write() ;
	hScatter_cut->Write() ;

	out->Close() ;

	return 0 ;
}
