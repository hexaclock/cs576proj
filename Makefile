PROJECT = cs576_project
STAGE 	= beta
DOC 	= doc/Makefile
SRC 	= src/Makefile

#call all from src/Makefile and doc/Makefile
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

#call fclean from src/Makefile
src_fclean:

#call re from src/Makefile
src_re:

#call all from doc/Makefile
doc_all:

#call clean from doc/Makefile
doc_clean:

#call fclean from doc/Makefile
doc_fclean:

#call re from doc/Makefile
doc_re:

submit: fclean doc_all doc_clean
	tar -cvzf $(PROJECT)-$(STAGE).tar.gz ./*

