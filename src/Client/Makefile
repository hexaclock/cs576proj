CC       = clang++
PRG	     = client
SRC      = Client.cpp Crypto.cpp JsonParsing.cpp ../shared/jsoncpp.cpp \
		   ../shared/base64.cpp
OBJ      = $(SRC:.cpp=.o)
RM       = rm -f
OPENSSL  = /usr/include/openssl
IPATH    = ../../include
LDLIBS   += -l:libwolfssl.so.1.0.0
LDFLAGS  += -L../../lib -Wl,-rpath -Wl,../../lib
CXXFLAGS += -I$(IPATH) -I$(OPENSSL) -ggdb -Wall -Wextra -pedantic -std=c++11

all: $(OBJ)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(SRC) -o $(PRG) $(LDLIBS)

clean:
	$(RM) *.o
	$(RM) *.swp
	$(RM) *~
	$(RM) \#*

fclean: clean
	$(RM) $(PRG)

re: fclean all

