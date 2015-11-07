PROJECT = cs576_project
STAGE 	= beta
DOC 	= doc/Makefile
SRC 	= src/Makefile

#call src/Makefile and docs/Makefile
all:

#call clean from src/Makefile and doc/Makefile
clean:

#call fclean from src/Makefile and doc/Makefile
fclean:

#call re from src/Makefile and doc/Makefile
re:

#call all from src/Makefile
src_all:

#call clean from src/Makefile
src_clean:

#call all from doc/Makefile
doc_all:

#call clean from doc/Makefile
doc_clean:

submit: fclean doc_all doc_clean
	tar -cvzf $(PROJECT)-$(STAGE).tar.gz ./*

