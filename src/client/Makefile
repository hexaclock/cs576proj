CXX      = clang++
PRG      = klclient
SRC      = Client.cpp JsonParsing.cpp TLS.cpp \
	   ../shared/Crypto.cpp ../shared/jsoncpp.cpp ../shared/base64.cpp \
	   ../shared/Util.cpp
OBJ      = $(SRC:.cpp=.o)
RM       = rm -f
#OPENSSL  = /usr/include/openssl
IPATH    = ../../include
LDLIBS   += -l:libwolfssl.so.1.0.0
LDFLAGS  += -L../../lib -Wl,-rpath -Wl,../../lib
CXXFLAGS += -I$(IPATH) -g -Wall -Wextra -Wno-unused-parameter -pedantic -std=c++11

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

