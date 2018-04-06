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

using namespace std;

double PI = acos(-1);

// int kbhit()
// {
//    int i;
//    ioctl(0, FIONREAD, &i);
//    return i; #<{(| return a count of chars available to read |)}>#
// }

int main(int argc, char *argv[])
{
  SoapySDR::Kwargs args;
  if (argc > 1)
    args.insert(make_pair("serial", string(argv[1])));
  SoapySDR::Device *sdr = SoapySDR::Device::make(args);

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

  int ptime;
  ptime = 0;
  float maxMag;
  maxMag = 0;

  // system("stty raw ");
  system("stty -icanon");

  int scani = 0;
  int scanp = 40;
  ofstream logfile;
  logfile.open("logfile.txt");
  for (int z=0;true;z++) {

    int ret = sdr->readStream(
        rx, buffs, streamMTU, flags, timeNs, timeoutUs);
    long long tdiff = timeNs - ptime;

    ptime = timeNs;
    if (ret < 0)
      cout << "ERROR" << endl;
    else if (ret == 0)
      cout << "ZERO samples" << endl;
    else {
      double magsum = 0;
      for (int i=0; i < ret; i++) {
        complex<float> sample = buff[i];
        float mag = abs(sample);
        if (mag > maxMag) {
          maxMag = mag;
          printf("NEW MAX MAG: %f\n", mag);
        }
        if (mag > threshold) {

          printf("signal (I=%f, Q=%f) %f * e ^ %f\n", real(sample), imag(sample), mag, arg(sample));       
          string del = "\t";
          logfile  
            << real(sample) << del
            << imag(sample) << del
            << mag << del
            << arg(sample) << endl
            ;
        }
      }
    }
  }
  logfile.close();
  system("stty cooked echo");
  cout << "hello";
  sdr->deactivateStream(rx);
  sdr->closeStream(rx);
  SoapySDR::Device::unmake(sdr);
  return 0;
}
