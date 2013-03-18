{
	//TFile f("../120821_Ar4.7_p1806mbar/120821_470V_570V_2370V.root") ;
	//tree->Draw("y:x>>h2(256,0,256,256,0,256)","t","colz",1,5) ;
	//double tmean=htimespectrum->GetMean() ;
	//double trms=htimespectrum->GetRMS() ;
	hTime->Draw() ;	// Determine time window manually.
	//cout << "Please double-click on the canvas to continue..." << endl ;
	//c1->WaitPrimitive() ;
	//cout << "++ Continuing..." << endl ;

	tree->Draw("y:x>>hScatter(256,0,256,256,0,256)","","colz") ;
	//tree->Draw("y:x>>scatter_timecut(256,0,256,256,0,256)",((tcut.str()).c_str()).c_str(),"colz",7,5) ;
	tree->Draw("Sum$(t>3600&&t<4400)>>hNhits(500,0,500)") ;
	tree->Draw("wf->Peak(7000,9500)>>hPeak") ; /*Returns in converted units*/
	tree->Draw("wf->Peak(7000,9500):Sum$(t>3940&&t<3970)>>hQvsHits","","box") ;
	tree->Draw("wf->Integral(7000,9500):Sum$(t>3940&&t<3970)>>hQaltvsHits(100,0,200,100,0,10)","","box") ;

	// Time window.
	stringstream tcut ;
	tcut << "t>3940&&t<3970" ;

	// Boundaries.
	double xLow = 20., xHigh = 240., yLow = 25., yHigh = 225. ;
	double min = 0., max = 256. ;
	stringstream shapecut ;
	shapecut<<"MinIf$(x,"<<(tcut.str()).c_str()<<") > 20 && MaxIf$(x,"<<(tcut.str()).c_str()<<") < 240 && MinIf$(y,"<<(tcut.str()).c_str()<<") > 25 && MaxIf$(y,"<<(tcut.str()).c_str()<<") < 225 " ;
	
	// Find out where to cut on the ratio of the x/y dimensions.
	tree->Draw("( MaxIf$(x,t>3940&&t<3970) - MinIf$(x,t>3940&&t<3970) ) : ( MaxIf$(y,t>3940&&t<3970) - MinIf$(y,t>3940&&t<3970) )>>hDimRatio(100,0,150,100,0,150)","Sum$(t>3940&&t<3970)>5") ;
	
	// Cut small number of hits.
	shapecut << " && Sum$(t>3940&&t<3970)>5 " ;
	
	// Cut the circularity outside 2/3 and 3/2, respectively.
	shapecut<<" && MaxIf$(x,"<<(tcut.str()).c_str()<<") - MinIf$(x,"<<(tcut.str()).c_str()<<") > (2./3.)*(MaxIf$(y,"<<(tcut.str()).c_str()<<") - MinIf$(y,"<<(tcut.str()).c_str()<<")) && MaxIf$(x,"<<(tcut.str()).c_str()<<") - MinIf$(x,"<<(tcut.str()).c_str()<<") < 1.5*(MaxIf$(y,"<<(tcut.str()).c_str()<<") - MinIf$(y,"<<(tcut.str()).c_str()<<")) " ;
	

	tree->Draw("y:x>>hScatterShapecut(256,0,256,256,0,256)",(shapecut.str()).c_str(),"colz") ;

	TLine* x1 = new TLine(xLow,min,xLow,max) ;
	TLine* x2 = new TLine(xHigh,min,xHigh,max) ;
	TLine* y1 = new TLine(min,yLow,max,yLow) ;
	TLine* y2 = new TLine(min,yHigh,max,yHigh) ;
	x1->SetLineColor(kViolet) ;
	x2->SetLineColor(kViolet) ;
	y1->SetLineColor(kViolet) ;
	y2->SetLineColor(kViolet) ;
	x1->Draw("same") ;
	x1->Draw("same") ;
	x2->Draw("same") ;
	y1->Draw("same") ;
	y2->Draw("same") ;
	cout << endl << "++ This is where the fiducial area is." << endl ;
	cout << "++ Please double-click on the canvas to continue..." << endl ;
	c1->WaitPrimitive() ;
	cout << "++ Continuing..." << endl ;

	/*
	TEntryList* entrylist = new TEntryList("entrylist","some cut") ;
	tree->Draw(">>entrylist","Sum$(t>3700&&t<4300)>5","entrylist") ; // Or any other cut.
	// TEntryList* entrylist = gDirectory->FindObject("entrylist") ; // If defined on the fly, this is how to retrieve it.
	tree->SetEntryList(0) ;
	*/
	cout << endl << "++ Creating a new entrylist for the tree with cuts on " << endl 
		<< "   the time, the fiducial area, the number of hits, and the cloud shape." << endl ;
	cout << "++ This may take a while..." << endl << endl ;
	TEntryList* newentrylist = new TEntryList("newentrylist","cut on event shape and position") ;
	tree->Draw(">>newentrylist",(shapecut.str()).c_str(),"entrylist") ;
	tree->SetEntryList(newentrylist) ; // To reset: tree->SetEntryList(0) ;
	
	tree->Draw("Sum$(t>3940&&t<3970)>>hNhitsShapecut(500,0,500)",(shapecut.str()).c_str()) ;
	//tree->Draw("y:x>>hScatter_shapecut(256,0,256,256,0,256)",(shapecut.str()).c_str(),"colz") ;
	//tree->Draw("samples*vscale-voffset:Iteration$","","",1,5) ; /*Converted to real units*/
	tree->Draw("wf->Peak(7000,9500)>>hPeakShapecut",(shapecut.str()).c_str()) ;
	tree->Draw("wf->Peak(7000,9500):Sum$(t>3940&&t<3970)>>hQvsHitsShapecut",(shapecut.str()).c_str()) ;
	tree->Draw("t>>hTimeCut","t>3940&&t<3970") ;
/*
	TH1F* single_wf = new TH1F("single_wf","Single waveform",100,0,12500) ;
	TH1F* total_wf = new TH1F("total_wf","All waveforms",100,0,12500) ;

	tree->Draw("Iteration$>>single_wf","samples*vscale-voffset","",1,5) ;
	tree->Draw("Iteration$>>total_wf","samples*vscale-voffset") ; // All waveforms.
*/

	/* Now in fitWaveform(TH1D* single, TH1D* total).
	// Fit function: oscillation + linear rise with exp. decay.
	TF1* fitFunc = new TF1("fitFunc","(([0]*(x-[1])+[2])*(x>[1])) * (1.*(x<[3])+TMath::Exp(([3]-x)/[4])*(x>[3])) + ([5]+[6]*TMath::Sin(2*TMath::Pi()*(x-[7])/[8]))",0,12500) ;
	double parset[9] = {3e-3,6400.,0.18, 7600.,720., -0.1,0.4,-300.,6400.} ;
	fitFunc->SetParameters(parset) ;
	fitFunc->SetParNames("p_lin","t_rise","offset lin","t_fall","p_decay","offset sin","ampl sin","phase sin","period sin") ;
	// Now parabolic instead of linear rise.
	TF1* fitterFunc = new TF1("fitterFunc","( (x>[0])*( (x<[1])*([2]-[3]*(x-[1])**2) + [3]*([1]-[0])**2*(x>[1]) ) ) * (1.*(x<[1])+TMath::Exp(([1]-x)/[4])*(x>[1])) + ([5]+[6]*TMath::Sin(2*TMath::Pi()*(x-[7])/[8]))",0,12500) ;
	double parset2[9] = {6350.,7650.,2.5,1.5e-6, 1219, -0.11,0.4,-300.,6519.} ;
	fitterFunc->SetParameters(parset2) ;
	fitterFunc->SetParNames("t_rise","t_fall","height","p_rise","p_decay","offset sin","ampl sin","phase sin","period sin") ;
	
	parset[3] = (double)(total_wf->GetMaximumBin()) * 125 ;	// Define start value for peak position.
	parset[5] = (double)(total_wf->GetBinContent(20)) / 10000 ; // Define start value for sine offset.
	parset[6] = (double)(single_wf->GetMaximumBin()) - parset[5] ; // Define start value for sine amplitude.
	// The same for the second parameter set.
	parset2[1] = (double)(total_wf->GetMaximumBin()) * 125 ;	// Define start value for peak position.
	parset2[5] = (double)(total_wf->GetBinContent(20)) / 10000 ; // Define start value for sine offset.
	parset2[6] = (double)(single_wf->GetMaximumBin()) - parset[5] ; // Define start value for sine amplitude.

	*/

	cout << "++ Done!" << endl ;
}

