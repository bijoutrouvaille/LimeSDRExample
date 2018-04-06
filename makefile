r:
	g++ -std=c++11 rx.cpp -lSoapySDR && mv a.out out.rx && ./out.rx 0009070105C63425
t:
	g++ -std=c++11 tx.cpp -lSoapySDR && mv a.out out.tx && ./out.tx 0009070105C62B2B
	
# rrr:
# 	g++ -std=c++11 reset.cpp -lSoapySDR && mv a.out r && ./r 0009070105C63425 && rm r

ttt:
	g++ -std=c++11 reset.cpp -lSoapySDR && mv a.out r && ./r 0009070105C62B2B && rm r
