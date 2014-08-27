all: compilar

compilar:
	pdflatex main.tex 
	bibtex main.aux
	pdflatex main.tex 
	pdflatex main.tex 

clean:
	rm -f *.bak *.lof *.lol *.lot *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl main.pdf
