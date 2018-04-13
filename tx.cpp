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
  double carrierFreq = 467.637e+6;
  int bandwidth = 1000;
  double ampl = 1.0;
  // double rate = 44100;
  // double rate = 1.01e5;

  AudioFile<double> audioFile;
  audioFile.load("fly44100.wav");
  double rate = audioFile.getSampleRate();
  int numSamples = audioFile.getNumSamplesPerChannel();


  SoapySDR::Kwargs args;
  if (argc > 1)
    args.insert(make_pair("serial", string(argv[1])));

  SoapySDR::Device *sdr = SoapySDR::Device::make(args);

  ChannelOpts o;

  o.carrierFreq = carrierFreq; 
  o.bandwidth = 5e+6;
  o.sampleRate = rate;
  o.channel = 0;
  o.gain = 35;
  o.RXTX = SOAPY_SDR_TX;

  Channel* channel = new Channel(sdr, o);

  // PRINT INFO
  // channel->printInfo(); return 0;

  size_t streamMTU = channel->mtu();
  
  complex<float> buff[streamMTU];
  // int msg[] = {0,0,0,1,1,1,0,0,1,0,0,1,1,1}; //17

  // double mtuspsec = rate / streamMTU;
  complex<float> bb[numSamples];
  for (int i=0;i<numSamples;i++) bb[i] = complex<float>(audioFile.samples[0][i], 0);

  for (int i=0; i<50;i++) cout << bb[i] << endl;
  channel->send(bb, numSamples);
  // for (int q=0; q < mtuspsec*20; q++) {
  //   for (int n=0; n < streamMTU; n++) {
  //     int nt = n + streamMTU*q;
  //     double t = nt / rate;
  //     buff[n] = complex<float>(sin(2 * PI * 1000 * t/rate), 0);
  //   }
  //   int ret = channel->sendMTU(buff, streamMTU);
  //
  //   cout << "cycle " << q << endl;
  // }

  cout << "done";
  channel->gain(-12);
  delete channel;
  SoapySDR::Device::unmake(sdr);
  return 0;
}
