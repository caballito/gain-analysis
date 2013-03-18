/**************************************************************************
 * Function to fit the waveform of the oscilloscope with an oscillation,  *
 * a parabolic rise and exponential decay.                                *
 * Used to calculate the integral of the signal (or determine the peak).  *
 **************************************************************************/

#include "TF1.h"
#include "TCanvas.h"
#include "TEntryList.h"
#include "TH1D.h"

double fitWaveform(int event, TH1D* allWaveforms,TTree* tree) {
	// Fit function: oscillation + linear rise with exp. decay.
	/*
	TF1* fitFunc = new TF1("fitFunc","(([0]*(x-[1])+[2])*(x>[1])) * (1.*(x<[3])+TMath::Exp(([3]-x)/[4])*(x>[3])) + ([5]+[6]*TMath::Sin(2*TMath::Pi()*(x-[7])/[8]))",0,12500) ;
	double parset[9] = {3e-3,6400.,0.18, 7600.,720., -0.1,0.4,-300.,6400.} ;
	fitFunc->SetParameters(parset) ;
	fitFunc->SetParNames("p_lin","t_rise","offset lin","t_fall","p_decay","offset sin","ampl sin","phase sin","period sin") ;
	parset[3] = (double)(total_wf->GetMaximumBin()) * 125 ; // Define start value for peak position.
	parset[5] = (double)(total_wf->GetBinContent(20)) / 10000 ; // Define start value for sine offset.
	parset[6] = (double)(single_wf->GetMaximumBin()) - parset[5] ; // Define start value for sine amplitude.
	*/

	TCanvas* canvas = new TCanvas() ;
	TH1F* oneWaveform = new TH1D("oneWaveform","single waveform",100,0,12500) ;
	//oneWaveform = new TH1D("oneWaveform","single waveform",100,0,12500) ;
	tree->Draw("Iteration$>>oneWaveform","static_cast<double>(samples)*vscale-voffset","",1,event) ;

	// Parabolic instead of linear rise.
	TF1* fitterFunc = new TF1("fitterFunc","( (x>[0])*( (x<[1])*([2]-[3]*(x-[1])**2) + [3]*([1]-[0])**2*(x>[1]) ) ) * (1.*(x<[1])+TMath::Exp(([1]-x)/[4])*(x>[1])) + ([5]+[6]*TMath::Sin(2*TMath::Pi()*(x-[7])/[8]))",0,12500) ;
	double parset2[9] = {6350.,7650.,2.5,1.5e-6, 1219, -0.11,0.4,-300.,6519.} ;
	fitterFunc->SetParameters(parset2) ;
	fitterFunc->SetParNames("t_rise","t_fall","height","p_rise","p_decay","offset sin","ampl sin","phase sin","period sin") ;

	// Start values for the second parameter set to ease the fit.
	parset2[1] = (double)(allWaveforms->GetMaximumBin()) * 125 ;  // Define start value for peak position.
	parset2[5] = (double)(allWaveforms->GetBinContent(20)) / 10000 ; // Define start value for sine offset.
	oneWaveform->GetXaxis()->SetRangeUser(0,5000) ; // Just look in the first part.
	parset2[6] = (double)(oneWaveform->GetBinContent(oneWaveform->GetMaximumBin())) - parset2[5] ; // Define start value for sine amplitude.
	oneWaveform->GetXaxis()->SetRangeUser(0,12500) ; // Reset to full range.
	
	oneWaveform->Fit("fitterFunc","Q","",0,12000) ;
	fitterFunc->GetParameters(parset2) ;

	double peakPosition = oneWaveform->GetMaximumBin() * 125 ;
	double peakHeight = oneWaveform->GetBinContent(oneWaveform->GetMaximumBin()) 
		- (parset2[5] + parset2[6] * TMath::Sin( 2*TMath::Pi()*peakPosition/parset2[8] )) ; // Substract the value of the sine over there.

	// Clean up memory.
	delete oneWaveform ;
	delete canvas ;

	cout << "peakPosition: " << peakPosition << endl ;
	return peakHeight ;
}
