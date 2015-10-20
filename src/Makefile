PROJECT  = cs576_project
STAGE    = alpha
CC       = clang++
#SERVER   = server
CLIENT   = client
SRC1     = Server.cpp jsoncpp.cpp JsonParsing.cpp
SRC2     = Client.cpp jsoncpp.cpp JsonParsing.cpp Crypto.cpp
OBJ1     = $(SRC1:.cpp=.o)
OBJ2     = $(SRC2:.cpp=.o)
OPENSSL  = /usr/include/openssl
IPATH    = ../include
CXXFLAGS += -I$(IPATH) -I$(OPENSSL) -g -O2 -Wall -Wextra -pedantic -std=c++11
RM       = rm -f

# Latex things for docs
DOCSDIR  = ../doc
PDFTEX   = pdflatex -shell-escape -interaction=nonstopmode -file-line-error -output-directory $(DOCSDIR)

all: $(SERVER) $(CLIENT)

$(SERVER):
	$(CC) $(CXXFLAGS) $(SRC1) -o $(SERVER)

$(CLIENT):
	$(CC) $(CXXFLAGS) $(SRC2) -o $(CLIENT)

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
