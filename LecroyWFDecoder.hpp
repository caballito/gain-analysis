
#ifndef __DETRD__LECROYWFDECODER__HPP__
#define __DETRD__LECROYWFDECODER__HPP__

#include <fstream>

#include "Waveform.hpp"

class LecroyWFDecoder : public WaveformDecoder
{
public:
  LecroyWFDecoder(char* filename = 0);
  virtual ~LecroyWFDecoder();

  virtual void ChangeFile(char* filename);

  virtual Float_t GetVscale()  { return  Vscale; }
  virtual Float_t GetVoffset() { return  Voffset; }
  virtual Float_t GetTscale()  { return  Tscale; }
  virtual Float_t GetTdelay()  { return  Tdelay; }
  virtual Int_t GetNsamples()  { return  Nsamples; }
  virtual Int_t FillData(Waveform::sample_t* address);
  virtual Int_t Success() { return status ; };

private:
  LecroyWFDecoder(const LecroyWFDecoder&);
  void DecodeParams();
  void SetDefaultParams();

public: // will become private
  Float_t Vscale;
  Float_t Voffset;
  Float_t Tscale;
  Float_t Tdelay;
  Int_t Nsamples;

  bool status;

  std::ifstream dstream;

  union encoded_t
  {
    char    bytes[4];
    Float_t floatenc;
    UInt_t  uintenc;
  };

  static const int sampleCountpos = 123; // seems also 127, while at 131 is sampleCount-2
  static const int multiplierpos = 167;
  static const int offsetpos = 171;
  static const int intervalpos = 187;
  static const int datapos = 357;

  static const unsigned int maxsamples = 100000;
};


#endif //__DETRD__LECROYWFDECODER__HPP__
