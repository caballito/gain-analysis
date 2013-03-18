//#include <iostream>

#include "LecroyWFDecoder.hpp"

using std::ifstream;

LecroyWFDecoder::LecroyWFDecoder(char* filename)
  : Vscale(1.0), Voffset(0.0), Tscale(1.0), Tdelay(0.0)
  , Nsamples(0) , status(false), dstream(filename, ifstream::binary)
{
  if (! dstream.is_open()) return;
    
  DecodeParams();
}

LecroyWFDecoder::~LecroyWFDecoder()
{
}

void LecroyWFDecoder::ChangeFile(char* filename)
{
  dstream.close();
  SetDefaultParams();

  dstream.open(filename);
  if (! dstream.is_open()) return;
    
  DecodeParams();  
}

void LecroyWFDecoder::SetDefaultParams()
{
  Vscale = 1.0;  Voffset = 0.0; Tscale = 1.0; Tdelay = 0.0;
  Nsamples = 0; status = false;
}

void LecroyWFDecoder::DecodeParams()
{
  encoded_t encoded;
  int success = 0;

  dstream.seekg( sampleCountpos );
  dstream.read( encoded.bytes, sizeof(encoded.uintenc) );
  //  cout << "sampleCount: " << encoded.uintenc << "\t" << dstream.good() << endl;
  if ( dstream.good() && encoded.uintenc > 0 && encoded.uintenc < maxsamples) {
    Nsamples = encoded.uintenc;
    success++;
  }

  dstream.seekg( multiplierpos );
  dstream.read( encoded.bytes, sizeof(encoded.floatenc) );
  //  cout << "multiplier: " << encoded.floatenc << "\t" << dstream.good() << endl;
  if ( dstream.good() ) {
    Vscale = encoded.floatenc;
    success++;
  }

  dstream.seekg( offsetpos );
  dstream.read( encoded.bytes, sizeof(encoded.floatenc) );
  //  cout << "offset: " << encoded.floatenc << "\t" << dstream.good() << endl;
  if ( dstream.good() ) {
    Voffset = encoded.floatenc;
    success++;
  }

  dstream.seekg( intervalpos );
  dstream.read( encoded.bytes, sizeof(encoded.floatenc) );
  //  cout << "interval: " << encoded.floatenc << "\t" << dstream.good() << endl;
  if ( dstream.good() ) {
    Tscale = encoded.floatenc;
    success++;
  }

  if (success == 4) {
    status = true;
  }
  else {
    SetDefaultParams();
  }
}

Int_t LecroyWFDecoder::FillData(Waveform::sample_t* address)
{
  if ( ! status ) return status;

  dstream.seekg(datapos);
  std::streamsize bytesToRead = Nsamples * sizeof(Waveform::sample_t);
  //  cout << "Bytes to be read: " << bytesToRead << endl;
  dstream.read( static_cast<char*>(address) , bytesToRead );

  status = dstream.good();
  return status;
}
