PROJECT  = cs576_project
STAGE    = alpha
CC       = clang++
#SERVER   = server
CLIENT   = client
SRC1     = Server.cpp jsoncpp.cpp JsonParsing.cpp Crypto.cpp base64.cpp
SRC2     = Client.cpp jsoncpp.cpp JsonParsing.cpp Crypto.cpp base64.cpp
OBJ1     = $(SRC1:.cpp=.o)
OBJ2     = $(SRC2:.cpp=.o)
OPENSSL  = /usr/include/openssl
IPATH    = ../include
LDLIBS   += -l:libwolfssl.so.1.0.0
LDFLAGS  += -L../lib -Wl,-rpath -Wl,../lib
CXXFLAGS += -I$(IPATH) -I$(OPENSSL) -ggdb -Wall -Wextra -pedantic -std=c++11
RM       = rm -f

# Latex things for docs
DOCSDIR  = ../doc
PDFTEX   = pdflatex -shell-escape -interaction=nonstopmode -file-line-error -output-directory $(DOCSDIR)

all: $(SERVER) $(CLIENT)

$(SERVER):
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(SRC1) -o $(SERVER) $(LDLIBS)

$(CLIENT):
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(SRC2) -o $(CLIENT) $(LDLIBS)

clean:
	-$(RM) *.o *~ \#*

fclean: clean
	-$(RM) $(SERVER) $(CLIENT)
	-$(RM) ../doc/*.pdf

docsclean:
	-$(RM) ../doc/*.aux ../doc/*.log

re: fclean all

# make latex docs in ../doc
docs:
	$(PDFTEX) $(DOCSDIR)/proposal.tex
	$(PDFTEX) $(DOCSDIR)/design.tex

# make a tar.gz with docs in ../../
submit: fclean docs docsclean
	tar -cvzf ../../$(PROJECT)-$(STAGE).tar.gz ../*
