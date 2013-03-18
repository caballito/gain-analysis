// Load the environment on the server.
{

  //TO BE RUN ONLY ONCE WHEN STARTING ROOT SESSION

  gROOT->Reset();

  gSystem->CompileMacro("/project/detrd/darwin/Software/GainMeasurements/v0.0.3/GridPix.cpp",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"GridPix");
  gSystem->CompileMacro("/project/detrd/darwin/Software/GainMeasurements/v0.0.3/Waveform.cpp",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"Waveform");
  gSystem->CompileMacro("/project/detrd/darwin/Software/GainMeasurements/v0.0.3/LecroyWFDecoder.cpp",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"LecroyWFDecoder");
  gSystem->CompileMacro("/project/detrd/darwin/Software/GainMeasurements/v0.0.3/readrawdata.cc",
			"k"/*(f)orce, (k)eep after end of ROOT session, debu(g), (O)ptimize, (c)ompile only, (-)flat dir structure*/,
			"readrawdata");

  gStyle->SetPalette(51,0);	// 1: nice rainbow colours, 51: blue scale.
}  
