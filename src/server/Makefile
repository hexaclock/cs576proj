CXX      = clang++
PRG      = klserver
SRC      = wolfssl_server.cpp ../shared/jsoncpp.cpp ../shared/base64.cpp ../shared/Crypto.cpp \
	   ../shared/Util.cpp
OBJ      = $(SRC:.cpp=.o)
RM       = rm -f
#OPENSSL  = /usr/include/openssl
IPATH    = ../../include
LDLIBS   += -l:libwolfssl.so.1.0.0
LDFLAGS  += -L../../lib -Wl,-rpath -Wl,../../lib
CXXFLAGS += -I$(IPATH) -g -O2 -Wall -pedantic -std=c++11
#LDFLAGS  += -L../../openssl/ -Wl,--no-as-needed
#LDLIBS   += -ldl -lssl -lcrypto

all: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(PRG)

clean:
	$(RM) *.o
	$(RM) *.swp
	$(RM) *~
	$(RM) \#*

fclean: clean
	$(RM) $(PRG)

re: fclean all
