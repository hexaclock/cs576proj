CXX      = clang++
PRG      = client
SRC      = Client.cpp Crypto.cpp JsonParsing.cpp TLS.cpp \
		   ../shared/jsoncpp.cpp ../shared/base64.cpp
OBJ      = $(SRC:.cpp=.o)
RM       = rm -f
#OPENSSL  = /usr/include/openssl
IPATH    = ../../include
LDLIBS   += -l:libwolfssl.so.1.0.0
LDFLAGS  += -L../../lib -Wl,-rpath -Wl,../../lib
CXXFLAGS += -I$(IPATH) -ggdb -Wall -Wextra -pedantic -std=c++11

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

