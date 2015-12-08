PROJECT = cs576_project
STAGE 	= final
MAKE    = make
DOC 	= ./doc/
SRC 	= ./src/

#call all from src/Makefile and doc/Makefile
all:
	$(MAKE) -C $(SRC)
	$(MAKE) -C $(DOC)

#call clean from src/Makefile and doc/Makefile
clean:
	$(MAKE) clean -C $(SRC)
	$(MAKE) clean -C $(DOC)

#call fclean from src/Makefile and doc/Makefile
fclean:
	$(MAKE) fclean -C $(SRC)
	$(MAKE) fclean -C $(DOC)

#call re from src/Makefile and doc/Makefile
re:
	$(MAKE) re -C $(SRC)
	$(MAKE) re -C $(DOC)

#call all from src/Makefile
src_all:
	$(MAKE) -C $(SRC)

#call clean from src/Makefile
src_clean:
	$(MAKE) clean -C $(SRC)

#call fclean from src/Makefile
src_fclean:
	$(MAKE) fclean -C $(SRC)

#call re from src/Makefile
src_re:
	$(MAKE) re -C $(SRC)

#call all from doc/Makefile
doc_all:
	$(MAKE) -C $(DOC)

#call clean from doc/Makefile
doc_clean:
	$(MAKE) clean -C $(DOC)

#call fclean from doc/Makefile
doc_fclean:
	$(MAKE) fclean -C $(DOC)

#call re from doc/Makefile
doc_re:
	$(MAKE) re -C $(DOC)

submit: fclean doc_all doc_clean
	tar -cvzf $(PROJECT)-$(STAGE).tar.gz ./*

