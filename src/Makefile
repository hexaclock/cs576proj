SERVER   = server
CLIENT   = client
SRC1     = $(SERVER).c
SRC2     = $(CLIENT).c
OBJ1     = $(SRC1:.c=.o)
OBJ2     = $(SRC2:.c=.o)
OPENSSL  = /usr/include/openssl
IPATH    = ../include
CFLAGS  += -I$(IPATH) -I$(IPATH) -g -O2 -Wall -Wextra -pedantic
RM       = rm -f

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ1)

$(CLIENT): $(OBJ2)

clean:
	-$(RM) *.o *~ \#*
	-$(RM) $(SERVER) $(CLIENT)

fclean: clean

re: fclean all
