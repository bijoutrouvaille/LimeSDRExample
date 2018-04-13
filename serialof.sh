#!/bin/bash

# gets and prints the serial number
# of device specified by number
# 0-based: the first device is indexed at 0

# Found device 1
#   addr = 1d50:6108
#   driver = lime
#   label = LimeSDR-USB [USB 2.0] 9070105C62B2B
#   media = USB 2.0
#   module = STREAM
#   name = LimeSDR-USB
#   serial = 0009070105C62B2B

n=${1:-0} # 0-based device index
d=$(expr $n + 1) # 1-based device index

q=$(SoapySDRUtil --find | awk "
/serial =/ {print \$3;}
" | head -"$d" | tail -1)

if [ -z "$q" ]; then
  exit 1
fi

echo "$q"
