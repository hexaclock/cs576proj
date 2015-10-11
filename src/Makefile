CC       = g++
SERVER   = server
CLIENT   = client
SRC1     = $(SERVER).cpp
SRC2     = $(CLIENT).cpp
OBJ1     = $(SRC1:.cpp=.o)
OBJ2     = $(SRC2:.cpp=.o)
OPENSSL  = /usr/include/openssl
IPATH    = ../include
CXXFLAGS += -I$(IPATH) -I$(OPENSSL) -g -O2 -Wall -Wextra -pedantic
RM       = rm -f

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ1)

$(CLIENT): $(OBJ2)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(SERVER) $(CLIENT)

fclean: clean

re: fclean all
