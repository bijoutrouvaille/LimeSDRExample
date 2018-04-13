#include <iostream>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Constants.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Errors.hpp>
#include <cmath>
#include "Channel.cpp"
#include "./lib/AudioFile/AudioFile.cpp"

const double PI = std::acos(-1);

using namespace std;


int main(int argc, char *argv[])
{

  AudioFile<double> audioFile;
  audioFile.load("fly44100.wav");

  double rate = audioFile.getSampleRate();
  int numSamples = audioFile.getNumSamplesPerChannel();


  SoapySDR::Kwargs args;
  if (argc > 1)
    args.insert(make_pair("serial", string(argv[1])));

  SoapySDR::Device *sdr = SoapySDR::Device::make(args);

  ChannelOpts o;

  o.carrierFreq = 467.637e+6; 
  o.bandwidth = 5e+6;
  o.sampleRate = rate;
  o.channel = 0;
  o.gain = 35;
  o.RXTX = SOAPY_SDR_TX;

  Channel* channel = new Channel(sdr, o);

  complex<float> buff[numSamples];
  for (int i=0;i<numSamples;i++) 
    buff[i] = complex<float>(audioFile.samples[0][i], 0);

  channel->send(buff, numSamples);

  // kill the volume, otherwise the SDR will continue 
  // transmitting the carrier wave even after shutdown.
  channel->gain(-12); 
  delete channel;
  SoapySDR::Device::unmake(sdr);

  cout << "done";
  return 0;
}
