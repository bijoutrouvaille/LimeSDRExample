#include <iostream>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Constants.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Errors.hpp>
#include <signal.h>
#include <fstream>
#include <deque>
#include <cstdint> // int32_t
#include <string>
#include "Channel.cpp"
#include "./lib/AudioFile/AudioFile.cpp"

using namespace std;

double PI = acos(-1);

// sum of the register
complex<int> regSum(deque<int> q) {
  complex<int> r(0,0);
  deque<int> :: iterator it;
  for (it = q.begin(); it != q.end(); ++it) {
    // if 0 then 0, if 1 then 1, if -1 then complex unit i
    r += sqrt(*it); 
  }
  return r;
}

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
  // double rate = 1.01e5;

  ChannelOpts o;

  o.carrierFreq = carrierFreq; 
  o.bandwidth = 5e+6;
  o.sampleRate = rate;
  o.channel = 0;
  o.gain = 35;
  o.RXTX = Channel::RX;

  Channel* channel = new Channel(sdr, o);

  size_t streamMTU = channel->mtu();
  complex<float>* buff;

  wavelog.open("wavlog.csv");
  wavelog << "signalI" << endl;


  long rec = 800000;
  AudioFile<float> audio;
  audio.setAudioBufferSize(1, rec);
  audio.setBitDepth(16);
  audio.setSampleRate(rate);

  complex<float> samples[rec];
  channel->read(samples, rec);
  for (int i=0; i<rec; i++) 
    audio.samples[0][i] = real(samples[i]);
  audio.save("flyrx.wav", AudioFileFormat::Wave);

  wavelog.close();

  delete channel;
  SoapySDR::Device::unmake(sdr);

  cout << "good bye!";
  return 0;
}
