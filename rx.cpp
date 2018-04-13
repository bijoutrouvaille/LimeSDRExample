#include <iostream>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Constants.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Errors.hpp>
#include <fstream>
#include <string>
#include "Channel.cpp"
#include "./lib/AudioFile/AudioFile.cpp"

using namespace std;

double PI = acos(-1);

int main(int argc, char *argv[])
{
  SoapySDR::Kwargs args;
  if (argc > 1)
    args.insert(make_pair("serial", string(argv[1])));
  SoapySDR::Device *sdr = SoapySDR::Device::make(args);

  printf("imag unit: %f", imag(sqrt(-1)));

  double carrierFreq = 467.637e+6;
  double threshold = 0.02; // amplitude detection threshold
  int rxChan = 0;
  float ampl = 1.0;
  int rate = 44100;

  ChannelOpts o;

  o.carrierFreq = carrierFreq; 
  o.bandwidth = 5e+6;
  o.sampleRate = rate;
  o.channel = 0;
  o.gain = 35;
  o.RXTX = Channel::RX;

  Channel* channel = new Channel(sdr, o);

  long numOfSamples = 800000; // number of samples to record
  
  AudioFile<float> audio;
  audio.setAudioBufferSize(1, numOfSamples); // 1 channel
  audio.setBitDepth(16);
  audio.setSampleRate(rate);

  complex<float> samples[numOfSamples];
  channel->read(samples, numOfSamples); 

  for (int i=0; i<numOfSamples; i++) 
    audio.samples[0][i] = real(samples[i]);
  audio.save("flyrx.wav", AudioFileFormat::Wave);

  delete channel;
  SoapySDR::Device::unmake(sdr);

  cout << "good bye!";
  return 0;
}
