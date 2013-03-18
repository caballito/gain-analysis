{
	double vg1100[8] = {350,370,380,390,400,410,420,430} ;
	double hits1100[8] = {30.9,61.72,81.19,102.91,115.48,131.5,140.17,146.8} ;
	double q1100[8] = {12.49,18.32,22.59,30.72,39.95,59.35,81.72,110} ;
	double gain1100[8] = {} ;

	double vg1300[8] = {370,380,390,400,410,420,430,440} ;
	double hits1300[8] = {30.14,48.36,62.13,79.09,97.83,,113.34,124.64,132.03} ;
	double q1300[8] = {12.06,17.5,18.92,23.02,29.53,40.46,56.23,78.03} ;
	double gain1300[8] = {} ;

	double vg1506[9] = {/*370,*/390,400,410,420,430,440,450,460,470} ;
	double hits1506[9] = {/*36.85,*/35.21,52.38,64.05,73.83,83.54,97.13,103.55,109.08,113.62} ;
	double q1506[9] = {/*3.27,*/12.74,15.2,17.58,19.97,23.87,30.46,41.72,57.36,78.81} ;
	double gain1506[9] = {} ;

	double vg1809[12] = {/*410,*/420,430,440,450,460,470,480,490,500,510,520,530} ;
	double hits1809[12] = {/*22.7,*/36.68,52.79,64.57,72.67,76.23,79.01,81.81,82.97,84.28,90.73,88.21,89.4} ;
	double q1809[12] = {/*7.12,*/13.46,15.75,18.12,19.48,23.07,26.19,33.74,42.83,57.18,79.85,107.24,138.43} ;
	double gain1809[12] = {} ;

	double vg2110[10] = {440,450,460,470,480,490,500,510,520,530} ;
	double hits2110[10] = {45.78,57.36,73.42,84.28,84.92,80.57,86.13,89.27,92.15,93.2} ;
	double q2110[10] = {0.67,1.22,1.89,2.08,2.11,2.13,2.13,2.09,1.85,1.79} ;
	double gain2110[10] = {} ;

	// Efficiency plot.

	TGraph* eff1100 = new TGraph(8,vg1100,hits1100) ;
	TGraph* eff1300 = new TGraph(8,vg1300,hits1300) ;
	TGraph* eff1506 = new TGraph(9,vg1506,hits1506) ;
	TGraph* eff1809 = new TGraph(12,vg1809,hits1809) ;
	TGraph* eff2110 = new TGraph(10,vg2110,hits2110) ;

	eff1100->SetMarkerStyle(22) ;
	eff1100->SetMarkerColor(kRed) ;
	eff1100->SetFillColor(0) ;
	eff1300->SetMarkerStyle(23) ;
	eff1300->SetMarkerColor(kBlue) ;
	eff1300->SetFillColor(0) ;
	eff1506->SetMarkerStyle(24) ;
	eff1506->SetMarkerColor(kGreen) ;
	eff1506->SetFillColor(0) ;
	eff1809->SetMarkerStyle(25) ;
	eff1809->SetMarkerColor(kViolet) ;
	eff1809->SetFillColor(0) ;
	eff2110->SetMarkerStyle(26) ;
	eff2110->SetMarkerColor(kOrange) ;
	eff2110->SetFillColor(0) ;

	TMultiGraph* gEff = new TMultiGraph() ;
	gEff->Add(eff1100) ;
	gEff->Add(eff1300) ;
	gEff->Add(eff1506) ;
	gEff->Add(eff1809) ;
	//gEff->Add(eff2110) ;

	TCanvas* cEff = new TCanvas("cEff","cEff",600,400) ;
	cEff->cd() ;
	gEff->SetTitle("Ar/iC_{4}H_{10} 90/10;Grid voltage (V);Detected single electrons") ;
	gEff->Draw("ap") ;

	TLegend* lEff = new TLegend(0.1,0.7,0.37,0.9,"","brNDC") ;
	lEff->AddEntry(eff1100,"1100 mbar, THL 347") ;
	lEff->AddEntry(eff1300,"1300 mbar, THL 347") ;
	lEff->AddEntry(eff1506,"1506 mbar, THL 347") ;
	lEff->AddEntry(eff1809,"1809 mbar, THL 347") ;
	//lEff->AddEntry(eff2110,"2110 mbar, THL 343") ;
	lEff->Draw() ;
	lEff->SetFillColor(0) ;
	lEff->SetShadowColor(0) ;
	
	// Calibration function.
	//TF1* func = new TF1("func","2.37405e5 + (7.75611e13 * x * 4e-10 / 221)") ;	// Wrong function with Cin = 3.35 pF.
	//TF1* func = new TF1("func","3.33076e6 + (1.08817e15 * x * 4e-10 / 221)") ;	// Cin = 47 pF, don't forget to divide by 221 electrons.
	TF1* func = new TF1("func","x * 4e-10") ;	// Wrong function with Cin = 3.35 pF.

	// Calculate the gain.
	for (int i=0; i<12; i++){
		if (i<10){
			if (i<9) gain1506[i] = func->Eval(q1506[i]) ;
			if (i<8){
			 	gain1100[i] = func->Eval(q1100[i]) ;
				gain1300[i] = func->Eval(q1300[i]) ;
			}
			//gain1506[i] = func->Eval(q1506[i]) ;
			gain2110[i] = func->Eval(q2110[i]) ;
		}
		gain1809[i] = func->Eval(q1809[i]) ;
	}

	// Gain plot.
	TGraph* ggain1100 = new TGraph(8,vg1100,q1100) ;
	TGraph* ggain1300 = new TGraph(8,vg1300,q1300) ;
	TGraph* ggain1506 = new TGraph(9,vg1506,q1506) ;
	TGraph* ggain1809 = new TGraph(12,vg1809,q1809) ;
	TGraph* ggain2110 = new TGraph(10,vg2110,q2110) ;
	
	ggain1100->SetMarkerStyle(22) ;
	ggain1100->SetMarkerColor(kRed) ;
	ggain1100->SetFillColor(0) ;
	ggain1300->SetMarkerStyle(23) ;
	ggain1300->SetMarkerColor(kBlue) ;
	ggain1300->SetFillColor(0) ;
	ggain1506->SetMarkerStyle(24) ;
	ggain1506->SetMarkerColor(kGreen) ;
	ggain1506->SetFillColor(0) ;
	ggain1809->SetMarkerStyle(25) ;
	ggain1809->SetMarkerColor(kViolet) ;
	ggain1809->SetFillColor(0) ;
	ggain2110->SetMarkerStyle(26) ;
	ggain2110->SetMarkerColor(kOrange) ;
	ggain2110->SetFillColor(0) ;

	TMultiGraph* mggain = new TMultiGraph() ;
	mggain->Add(ggain1100) ;
	mggain->Add(ggain1300) ;
	mggain->Add(ggain1506) ;
	mggain->Add(ggain1809) ;
	//mggain->Add(ggain2110) ;
	
	TCanvas* cGain = new TCanvas("cGain","cGain",600,400) ;
	cGain->cd() ;
	cGain->SetLogy() ;
	mggain->SetTitle("Ar/iC_{4}H_{10} 90/10;Grid voltage (V);Integrated signal (Vs)") ;
	mggain->Draw("ap") ;

	TLegend* lGain = new TLegend(0.1,0.7,0.37,0.9,"","brNDC") ;
	lGain->AddEntry(ggain1100,"1100 mbar, THL347") ;
	lGain->AddEntry(ggain1300,"1300 mbar, THL347") ;
	lGain->AddEntry(ggain1506,"1506 mbar, THL347") ;
	lGain->AddEntry(ggain1809,"1809 mbar, THL347") ;
	//lGain->AddEntry(ggain2110,"2110 mbar, THL343") ;
	lGain->Draw() ;
	lGain->SetFillColor(0) ;
	lGain->SetShadowColor(0) ;



}
