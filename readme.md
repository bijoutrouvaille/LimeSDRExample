# Lime USB example

Demonstrates sending and receiving using SoapySDR.

## Requirements

- Linux
- g++
- SoapySDR library installed

## Install

Simply clone the repository

## Run

First, `cd LimeSDRExample`.

Start the RX side (rx.cpp). Will record several hundred thousand samples into flyrx.wav and quit.

```
make r
```

Start the TX side (tx.cpp). Will transmit the entire fly44100.wav over the in-phase component and then quit.

```
make t
```

## Notes

### Channel.cpp

A simple abstraction to write or read data to/from SoapySDR.

### lib/AudioFile

A library provided by https://github.com/adamstark/AudioFile to easily read/write WAV files.

## LICENSE

Public Domain
