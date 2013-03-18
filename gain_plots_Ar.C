{
	double vg1301[5] = {410,415,420,425,430} ;
	double hits1301[5] = {173.7,173.7,214.3,242.6,173.7} ;
	double q1301[5] = {2.05,2.05,2.15,2.21,2.05} ;
	double gain1301[5] = {} ;

	double vg1806[6] = {420,430,440,450,470,480} ;
	double hits1806[6] = {38.0,67.8,94.6,125,208,265} ;
	double q1806[6] = {4.28,4.89,4.59,4.63,4.91,4.79} ;
	double gain1806[6] = {} ;

	double vg2120[4] = {450,460,470,480} ;
	double hits2120[4] = {41.7,78.6,101.1,129.3} ;
	double q2120[4] = {5.0,3.53,4.44,4.8} ;
	double gain2120[4] = {} ;

	// Efficiency plot.

	TGraph* eff1301 = new TGraph(5,vg1301,hits1301) ;
	TGraph* eff1806 = new TGraph(6,vg1806,hits1806) ;
	TGraph* eff2120 = new TGraph(4,vg2120,hits2120) ;

	eff1301->SetMarkerStyle(23) ;
	eff1301->SetMarkerColor(kBlue) ;
	eff1301->SetFillColor(0) ;
	eff1806->SetMarkerStyle(25) ;
	eff1806->SetMarkerColor(kViolet) ;
	eff1806->SetFillColor(0) ;
	eff2120->SetMarkerStyle(26) ;
	eff2120->SetMarkerColor(kOrange) ;
	eff2120->SetFillColor(0) ;

	TMultiGraph* gEff = new TMultiGraph() ;
	//gEff->Add(eff1301) ;
	gEff->Add(eff1806) ;
	gEff->Add(eff2120) ;

	TCanvas* cEff = new TCanvas("cEff","cEff",600,400) ;
	cEff->cd() ;
	gEff->SetTitle("Ar 4.7;Grid voltage (V);Detected single electrons") ;
	gEff->Draw("ap") ;

	TLegend* lEff = new TLegend(0.1,0.7,0.37,0.9,"","brNDC") ;
	//lEff->AddEntry(eff1301,"1301 mbar, THL 340") ;
	lEff->AddEntry(eff1806,"1806 mbar, THL 341") ;
	lEff->AddEntry(eff2120,"2120 mbar, THL 341") ;
	lEff->Draw() ;
	lEff->SetFillColor(0) ;
	lEff->SetShadowColor(0) ;
	
	// Calibration function.
	TF1* func = new TF1("func","2.46e5 + 7.741e13 * x * 4e-10") ;

	// Calculate the gain.
	for (int i=0; i<6; i++){
		if (i<5){
			if (i<4){
				gain2120[i] = func->Eval(q2120[i]) ;
			}
			gain1301[i] = func->Eval(q1301[i]) ;
		}
		gain1806[i] = func->Eval(q1806[i]) ;
	}

	// Gain plot.
	TGraph* ggain1301 = new TGraph(5,vg1301,gain1301) ;
	TGraph* ggain1806 = new TGraph(6,vg1806,gain1806) ;
	TGraph* ggain2120 = new TGraph(4,vg2120,gain2120) ;
	
	ggain1301->SetMarkerStyle(23) ;
	ggain1301->SetMarkerColor(kBlue) ;
	ggain1301->SetFillColor(0) ;
	ggain1806->SetMarkerStyle(25) ;
	ggain1806->SetMarkerColor(kViolet) ;
	ggain1806->SetFillColor(0) ;
	ggain2120->SetMarkerStyle(26) ;
	ggain2120->SetMarkerColor(kOrange) ;
	ggain2120->SetFillColor(0) ;

	TMultiGraph* mggain = new TMultiGraph() ;
	mggain->Add(ggain1301) ;
	mggain->Add(ggain1806) ;
	mggain->Add(ggain2120) ;
	
	TCanvas* cGain = new TCanvas("cGain","cGain",600,400) ;
	cGain->cd() ;
	cGain->SetLogy() ;
	mggain->SetTitle("Ar 4.7;Grid voltage (V);Gain") ;
	mggain->Draw("ap") ;

	TLegend* lGain = new TLegend(0.1,0.7,0.37,0.9,"","brNDC") ;
	lGain->AddEntry(ggain1301,"1301 mbar, THL340") ;
	lGain->AddEntry(ggain1806,"1806 mbar, THL341") ;
	lGain->AddEntry(ggain2120,"2120 mbar, THL341") ;
	lGain->Draw() ;
	lGain->SetFillColor(0) ;
	lGain->SetShadowColor(0) ;

}
