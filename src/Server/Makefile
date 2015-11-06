CC       = clang++
NAME     = proxyserver
SRCS     = connect_to_host.cpp get_listener.cpp panic.cpp main.cpp start_proxy.cpp add_tls_cli.cpp add_tls_srv.cpp get_time.cpp ../shared/jsoncpp.cpp ../shared/base64.cpp
OBJS     = $(SRCS:.cpp=.o)
OPENSSL  = /usr/include/openssl
IPATH    = ../../include
#LDLIBS   += -l:libwolfssl.so.1.0.0
#LDFAGS   += -L../../lib -Wl,-rpath -Wl,../../lib
CXXFLAGS  += -I$(OPENSSL) -I$(IPATH) -g -O2 -Wall -pedantic -std=c++11
LDFLAGS += -L../../openssl/ -Wl,--no-as-needed
LDLIBS  += -ldl -lssl -lcrypto
RM       = rm -f

all: $(NAME)

$(NAME): #$(OBJS)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o $(NAME) $(LDLIBS)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(NAME)

fclean: clean

re: fclean all
