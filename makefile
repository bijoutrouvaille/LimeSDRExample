# designed to work on a machine with 2 limesdrs,
# but will work when only 1 is present.

# get the serial number for the first device
R:=$(shell ./serialof.sh 0)
T:=$(shell ./serialof.sh 1)

r:
	g++ -std=c++11 rx.cpp -lSoapySDR -o out.rx && ./out.rx $(R)
t:
	g++ -std=c++11 tx.cpp -lSoapySDR -o out.tx && ./out.tx $(T)
	

reset: export device?=1
reset: 
	g++ -std=c++11 reset.cpp -lSoapySDR && mv a.out r && ./r $(shell ./serialof.sh $(device)) && rm r
