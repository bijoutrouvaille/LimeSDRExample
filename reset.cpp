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
// #include <sys/filio.h>
// #include <conio>

using namespace std;


int main(int argc, char *argv[])
{
  SoapySDR::Kwargs args;
  if (argc > 1)
    args.insert(make_pair("serial", string(argv[1])));
  SoapySDR::Device *sdr = SoapySDR::Device::make(args);

  SoapySDR::Device::unmake(sdr);
  return 0;
}
