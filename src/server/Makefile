CC       = clang++
PRG      = server
SRC      = connect_to_host.cpp get_listener.cpp panic.cpp main.cpp \
		   start_proxy.cpp add_tls_cli.cpp add_tls_srv.cpp get_time.cpp \
		   ../shared/jsoncpp.cpp ../shared/base64.cpp
OBJ      = $(SRC:.cpp=.o)
RM       = rm -f
OPENSSL  = /usr/include/openssl
IPATH    = ../../include
#LDLIBS   += -l:libwolfssl.so.1.0.0
#LDFAGS   += -L../../lib -Wl,-rpath -Wl,../../lib
CXXFLAGS  += -I$(OPENSSL) -I$(IPATH) -g -O2 -Wall -pedantic -std=c++11
LDFLAGS  += -L../../openssl/ -Wl,--no-as-needed
LDLIBS   += -ldl -lssl -lcrypto

all: #$(OBJ)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(SRC) -o $(PRG) $(LDLIBS)

clean:
	$(RM) *.o
	$(RM) *.swp
	$(RM) *~
	$(RM) \#*

fclean: clean
	$(RM) $(PRG)

re: fclean all
