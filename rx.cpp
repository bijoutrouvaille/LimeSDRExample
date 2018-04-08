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

  double nudge = 5000;
  double carrierFreq = 467.637e+6;
  double threshold = 0.02; // amplitude detection threshold
  int bandwidth = 2000;
  int rxChan = 0;
  float ampl = 1.0;
  int rate = 44100;
  long txbw = 5e+6;
  sdr->setSampleRate(SOAPY_SDR_RX, rxChan, rate);
  sdr->setGain(SOAPY_SDR_RX, rxChan, 30);
  SoapySDR::Stream* rx = sdr->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32);
  sdr->activateStream(rx);
  sdr->setFrequency(SOAPY_SDR_RX, rxChan, carrierFreq);
  size_t streamMTU = sdr->getStreamMTU(rx);
  complex<float> buff[streamMTU];
  void *buffs[] = {buff};
  int flags;
  long long timeNs;
  const long timeoutUs = 100000;

  float maxMag;
  maxMag = 0;


  ofstream logfile;
  logfile.open("logfile.txt");

  unsigned int badChunkCount = 0;
  const unsigned int maxBadChunks = 1;
  const unsigned int minUniformChunks = 4;
  const unsigned int frameSize = 5;
  deque<int> bit;
  deque<int> msg;

  for (int z=0;true;z++) {

    int ret = sdr->readStream(
        rx, buffs, streamMTU, flags, timeNs, timeoutUs);

    if (ret < 0)
      cout << "ERROR" << endl;
    else if (ret == 0)
      cout << "ZERO samples" << endl;
    else {
      double mtusum = 0;
      for (int i=0; i < ret; i++) {
        complex<float> sample = buff[i];
        float mag = abs(sample);
        if (mag > maxMag) {
          maxMag = mag;
          printf("NEW MAX MAG: %f\n", mag);
        }
        if (mag > threshold) {
          mtusum += real(sample);
        }
      }
      double mtumean = mtusum/ret;
      int val;
      if (abs(mtumean) > threshold/2) {
        val = mtumean > 0 ? 1 : -1; 
      } else val = 0;
      bit.push_back(val);
      if (bit.size() == frameSize) {
        complex<int> s = regSum(bit);
        int ones = real(s);
        int zeroes = imag(s);
        int unis = max(ones, zeroes);
        if (unis >= minUniformChunks) {
          if (msg.size()==0) cout << streamMTU << " starting message... " << endl;
          msg.push_back(ones > zeroes);
          
          logfile << msg.back();
          cout << msg.back();
          bit.clear();
          badChunkCount = 0;
        } else {
          badChunkCount++;
          if (badChunkCount > maxBadChunks) {
            badChunkCount = 0;
            if (msg.size()!=0) {
              logfile << endl;
              cout << "message done!" << endl;
            }
            msg.clear();  
          }
          bit.pop_front();
        }
      }
    }
  }

  logfile.close();

  sdr->deactivateStream(rx);
  sdr->closeStream(rx);
  SoapySDR::Device::unmake(sdr);

  cout << "good bye!";
  return 0;
}
