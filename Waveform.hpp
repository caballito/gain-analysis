
#ifndef __DETRD__WAVEFORM__HPP__
#define __DETRD__WAVEFORM__HPP__

#include <cstddef>

#include "TH1.h"

class WaveformDecoder;

class Waveform
{
public:
  typedef char sample_t;

  Waveform(); //default constructor required by ROOT I/O
  Waveform(Float_t Vscale, Float_t Voffset, Float_t Tscale, Float_t Tdelay,
	   Int_t Nsamples, sample_t* Data, bool copydata = true);
  virtual ~Waveform() { delete [] samples; }

  void SetParameters(Float_t Vscale, Float_t Voffset, Float_t Tscale, Float_t Tdelay)
  { vscale=Vscale; voffset=Voffset; tscale=Tscale; tdelay=Tdelay; }
  void SetData(Int_t Nsamples, sample_t* Data);
  void CopyData(Int_t Nsamples, sample_t* Data);

  Int_t ReadEncodedData(WaveformDecoder& decoder);

  TH1* MakeTH1(TH1* hist = 0 /*ignored for the moment*/ );

  Double_t Integral(Int_t firstsample = 0, Int_t lastsample /*included*/ = -1);

public: // will become private:
  Float_t vscale;
  Float_t voffset;
  Float_t tscale; 
  Float_t tdelay;
  std::size_t nalloc;   //! transient for ROOT I/O
  Int_t       nsamples;
  sample_t*   samples;  //[nsamples] ROOT I/O assumes this size


public:

  //ROOT Dictionary and I/O
  ClassDef(Waveform,1);
};


class WaveformDecoder
{
public:
  virtual Float_t GetVscale() = 0;
  virtual Float_t GetVoffset() = 0;
  virtual Float_t GetTscale() = 0;
  virtual Float_t GetTdelay() = 0;
  virtual Int_t GetNsamples() = 0;
  virtual Int_t FillData(Waveform::sample_t* address) = 0;
  virtual Int_t Success() = 0;
};


#endif //__DETRD__WAVEFORM__HPP__
