
{

  //TO BE RUN ONLY ONCE WHEN STARTING ROOT SESSION

  gROOT->Reset();

  gSystem->CompileMacro("/home/caballito/measurements/gain/v0.0.2/GridPix.cpp",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"GridPix");
  gSystem->CompileMacro("/home/caballito/measurements/gain/v0.0.2/Waveform.cpp",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"Waveform");
  gSystem->CompileMacro("/home/caballito/measurements/gain/v0.0.2/LecroyWFDecoder.cpp",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"LecroyWFDecoder");
  gSystem->CompileMacro("/home/caballito/measurements/gain/v0.0.2/readrawdata.cc",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"readrawdata");

  gStyle->SetPalette(1,0);
}  
