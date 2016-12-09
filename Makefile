SRC := $(wildcard *.cpp)
BIN := $(SRC:.cpp=)

all: $(BIN)

main: main.cpp database.hh GeeMail.hh
	g++ -g -std=c++11 -o main main.cpp -lsqlite3 -lgcrypt -lcryptopp -lssl -lcrypto
	
enc: enc.cpp
	g++ -std=c++11 -g -o enc enc.cpp -lcryptopp -lssl -lcrypto

clean:
	$(RM) src