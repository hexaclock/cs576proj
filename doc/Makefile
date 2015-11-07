DESIGN = design.tex
PROPOSAL = proposal.tex
PDFTEX = pdflatex -shell-escape -interaction=nonstopmode -file-line-error
RM = rm -f

all:
	$(PDFTEX) $(DESIGN)
	$(PDFTEX) $(PROPOSAL)

clean:
	$(RM) *.aux
	$(RM) *.log

fclean: clean
	$(RM) $(DESIGN:.tex=.pdf)
	$(RM) $(PROPOSAL:.tex=.pdf)

re: fclean all

