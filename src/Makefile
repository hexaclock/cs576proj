CC       = clang++
SERVER   = server
CLIENT   = client
SRC1     = Server.cpp
SRC2     = Client.cpp
OBJ1     = $(SRC1:.cpp=.o)
OBJ2     = $(SRC2:.cpp=.o)
OPENSSL  = /usr/include/openssl
IPATH    = ../include
CXXFLAGS += -I$(IPATH) -I$(OPENSSL) -g -O2 -Wall -Wextra -pedantic -std=c++11
RM       = rm -f

all: $(SERVER) $(CLIENT)

$(SERVER):
	$(CC) $(CXXFLAGS) $(SRC1) -o $(SERVER)

$(CLIENT): 
	$(CC) $(CXXFLAGS) $(SRC2) -o $(CLIENT)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(SERVER) $(CLIENT)

fclean: clean

re: fclean all
