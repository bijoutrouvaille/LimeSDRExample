# Lime USB example

Demonstrates sending and receiving using SoapySDR. Currently does _not_ work.

## Requirements

- Linux
- g++
- SoapySDR library installed

## Install

Simply clone the repository

## Run

First, `cd LimeSDRExample`.

Start the Rx side (rx.cpp). Will record several hundred thousand samples into flyrx.wav and quit.

```
make r
```

Start the Tx side (tx.cpp). Will transmit the entire fly44100.wav over the in-phase component and then quit.

```
make t
```

Make sure to run Tx quickly after the Rx in order to capture all the samples.

## Notes

### Channel.cpp

A simple abstraction to write or read data to/from SoapySDR in Stream MTU increments. 

### lib/AudioFile

A library provided by https://github.com/adamstark/AudioFile to easily read/write WAV files.

## LICENSE

Public Domain
