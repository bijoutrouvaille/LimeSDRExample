#include <iostream>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex>
#include <cmath>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Constants.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Errors.hpp>
#include <signal.h>

using namespace std;

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
    static const int RX = SOAPY_SDR_RX;
    static const int TX = SOAPY_SDR_TX;
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
    complex<float>* readMTU() {
      size_t mtu = this->mtu(); 
      complex<float> res[mtu];
      void* buffs[] = {this->buff};
      long long timeNs = 0;
      long timeoutUs = 1e6;
      int flags = 0;
      int ret = this->sdr->readStream(
        this->stream, buffs,
        mtu, flags, timeNs, timeoutUs);
      return this->buff;
    }
    void read(complex<float>* res, int count) {
      int mtu = this->mtu(); 
      
      complex<float>*buff;
      for (int i=0;i < count; i++) {
        if (i%mtu==0) {
          buff = this->readMTU();
        }
        res[i] = buff[i%mtu];
      }
    }
    void gain(double val) {
      this->sdr->setGain(this->o.RXTX, this->o.channel, val);
    }
    void printGainRange() {
      auto r = this->sdr->getGainRange(this->o.RXTX, this->o.channel);
      printf("GAIN RANGE %g - %g, step: %g\n", r.minimum(), r.maximum(), r.step());
    }
    void printRateRange() {
      auto r = this->sdr->getSampleRateRange(this->o.RXTX, this->o.channel);
      for (int i=0;i<r.size();i++)
      printf("SAMPLE RATE RANGE %g - %g, step: %g\n", r[i].minimum(), r[i].maximum(), r[i].step());
    
    }
    void printInfo() {
      this->printGainRange();
      this->printRateRange();
      printf("MTU %zd", this->mtu());
    }
};

