r:
	g++ -std=c++11 rx.cpp -lSoapySDR && mv a.out rx && ./rx 0009070105C63425
t:
	g++ -std=c++11 tx.cpp -lSoapySDR && mv a.out tx && ./tx 0009070105C62B2B
	
