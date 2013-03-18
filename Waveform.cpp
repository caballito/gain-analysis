
#include <cstring>

#include "Waveform.hpp"

Waveform::Waveform()
  : vscale(1.0), voffset(0.0), tscale(1.0), tdelay(0.0)
  , nalloc(0), nsamples(0), samples(0)
{}

Waveform::Waveform(Float_t Vscale, Float_t Voffset, Float_t Tscale, Float_t Tdelay,
		   Int_t Nsamples, Waveform::sample_t* Data, bool copydata)
  : vscale(Vscale), voffset(Voffset), tscale(Tscale), tdelay(Tdelay)
  , nalloc(0), nsamples(0), samples(0)
{
  if (copydata) {
    CopyData(Nsamples, Data);
  } else {
    SetData(Nsamples, Data);
  }
}


void Waveform::SetData(Int_t Nsamples, Waveform::sample_t* Data)
{
  if ( Nsamples <= 0 ) return;  //Are you fooling me?

    nsamples=Nsamples; nalloc=Nsamples; samples=Data;
}


void Waveform::CopyData(Int_t Nsamples, Waveform::sample_t* Data)
{
  if ( Nsamples <= 0 ) return;  //Are you fooling me?
  
  if ( Nsamples > static_cast<Int_t>(nalloc) )
    {
      delete [] samples;
      samples = new sample_t[Nsamples];
      nalloc = Nsamples;
      nsamples = Nsamples;
    } 
  else 
    {
      nsamples = Nsamples;
    };
  std::memcpy(samples, Data, Nsamples);
}

Int_t Waveform::ReadEncodedData(WaveformDecoder& decoder)
{ 
  Int_t Nsamples = decoder.GetNsamples();
  if ( !decoder.Success() || Nsamples <= 0) return 0;

  SetParameters(decoder.GetVscale(), decoder.GetVoffset(), 
		decoder.GetTscale(), decoder.GetTdelay());

  if ( Nsamples > static_cast<Int_t>(nalloc) )
    {
      delete [] samples;
      samples = new sample_t[Nsamples];
      nalloc = Nsamples;
      nsamples = Nsamples;
    } 
  else 
    {
      nsamples = Nsamples;
    };

  decoder.FillData(samples);
  return decoder.Success();
}


TH1* Waveform::MakeTH1(TH1* hist)
{
  //For the moment the passed histogram is ignored.
  TH1F* histo = new TH1F("__class_Waveform__","Waveform;Time (s);Amplitude (mV)",
			 nsamples, 0., nsamples);
  histo->SetDirectory(0); //out of the current ROOT directory!

  for (int i=0; i<nsamples; ++i)
    histo->Fill( i, static_cast<Double_t>(samples[i])*vscale-voffset );
    
  return histo;
}

Double_t Waveform::Integral(Int_t firstsample, Int_t lastsample)
{
  if (lastsample < 0 || lastsample >= nsamples) lastsample = nsamples - 1;
  if (firstsample < 0) firstsample = 0;
  if (firstsample > lastsample) return 0.;

  Int_t sum = 0.;
  for (int i = firstsample; i<=lastsample; ++i)
    sum += samples[i];

  return static_cast<Double_t>(sum)*vscale-voffset*(lastsample-firstsample+1);
}

// Return the maximum peak height.
// Rolf 19-02-2013.
Double_t Waveform::Peak(Int_t firstsample, Int_t lastsample){
	if (lastsample < 0 || lastsample >= nsamples) lastsample = nsamples - 1 ;
	if (firstsample < 0) firstsample = 0 ;
  if (firstsample > lastsample) return 0. ;
	
	// Initialize with 0 (on raw data scale).
	Double_t max = voffset/vscale ;
	for (Int_t i=firstsample; i<=lastsample; i++){
		if (max < samples[i]) max = static_cast<Double_t>(samples[i]) ;
	}

	return max*vscale-voffset ;
}

