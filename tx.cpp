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
#include <cmath>

using namespace std;
const double PI = std::acos(-1);
const double ii = sqrt(-1);
struct ChannelOpts {
  int RXTX;
  long carrierFreq;
  long bandwidth;
  double sampleRate;
  int channel = 0;
  int gain = 10;
  int timeoutUs = 2e+6;
};
class Channel {
  private:
    SoapySDR::Device *sdr;
    SoapySDR::Stream *stream;
    ChannelOpts o;
    size_t _mtu;
    complex<float>* buff;
  public:
    ~Channel() {

      this->sdr->deactivateStream(this->stream);
      this->sdr->closeStream(this->stream); 
    }
    Channel(SoapySDR::Device *sdr, ChannelOpts o) {
      this->o = o;
      this->sdr = sdr;
      this->stream = sdr->setupStream(o.RXTX, SOAPY_SDR_CF32);
      sdr->activateStream(this->stream); 
      this->_mtu = sdr->getStreamMTU(this->stream);
      sdr->setSampleRate(o.RXTX, o.channel, o.sampleRate);
      sdr->setGain(o.RXTX, o.channel, o.gain);
      this->buff = new complex<float>[this->_mtu]();
    }
    size_t mtu() { return this->_mtu; }
    int sendMTU(complex<float>* buff, long len) {
      void *buffs[] = {buff};
      int flags;
      int timeNs;
      int timeoutUs = this->o.timeoutUs;

      this->sdr->setFrequency(
          SOAPY_SDR_TX, this->o.channel, this->o.carrierFreq);

      int ret = this->sdr->writeStream(
          this->stream, buffs, len, flags, timeNs, timeoutUs);
      return ret;
    }
    int send(complex<float>* buff, long len) {
      int mtu = this->mtu();
      int chunks = ceil(len/mtu);
      int sent = 0;
      for (int n=0;n<chunks;n++) {
        int l2 = n==chunks-1 ? len%mtu : mtu;
        for (int i=0;i<l2;i++) {
          this->buff[i] = buff[n*mtu + i];
        }
        int ret = this->sendMTU(this->buff, l2);
        sent += ret;
      }
      return sent;
    }
};
// string stamp() {
//   using namespace std:chrono;
//   auto n = system_clock::now();
//   time_t t = system_clock::to_time_t(n);
//
//   return ctime(t)
// }

complex<float> bitFunc(bool x, int n, int bw, double carrier, double rate) {
  double t = n / rate;
  // complex<float> th(0, 2 * PI );
  // complex<float> th(0, 2 * PI * t * (carrier + bw*x));
  // complex<float> b = exp(th);// * complex<float>(256,0);
  // complex<float> b(1 - 2*x,0);
  
  complex<float> b((1 - 2*x)*sin(carrier*t),0);
  return b;
}

int bitLen(double rate, double durSec) {
  return ceil(rate * durSec);
}
void bit(complex<float> *buff, int len, bool x, int bw, double carrier, double rate) {
  for (int n=0; n < len; n++)
    buff[n] = bitFunc(x, n, bw, carrier, rate);
}
int main(int argc, char *argv[])
{
  double carrierFreq = 467.637e+6;
  int bandwidth = 1000;
  float ampl = 1.0;
  double rate = 44100;


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

  size_t streamMTU = channel->mtu();
  // int buffLen = bitLen(rate, 1);
  
  // complex<float> buff[buffLen];
  complex<float> buff[streamMTU];
  int msg[] = {0,0,0,1,1,1,0,0,1,0,0,1,1,1}; //17
  int msgi = 0;
  int msgl = 17;
  // int msgl = sizeof(msg)/sizeof(msg[0]);
  cout << "msg length: " << msgl << endl; //17
  for (int q=0; q < 10 || true; q++) {
    for (int n=0; n<streamMTU; n++) {
      if (true) { // send message
        bool b = msg[n%msgl];
        buff[n] = bitFunc(
            b, q*streamMTU + n
            , bandwidth, carrierFreq, rate
        );
      } else {
        buff[n] = complex<float>(0,0);
      }
    }

    // for (int n=0; n<streamMTU; n++) {
    //   int rep = 2;
    //   bool b = n%rep < rep/2;
    //   buff[n] = bitFunc(b, q*streamMTU + n, bandwidth, carrierFreq, rate);
    // }
    cout << "cycle " << q << endl;
    int ret = channel->send(buff, streamMTU);
  }

  cout << "hello";
  delete channel;
  SoapySDR::Device::unmake(sdr);
  return 0;
}
