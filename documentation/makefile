## makefile (for documentation)
## Mac Radigan

target=./Markowitz-Portfolio

all: build

build:
	\ln -fs ../results .
	\ln -fs $(target).pdf ..
	bibtex $(target).aux
	latex -interaction nonstopmode -shell-escape $(target)
	pdflatex -interaction nonstopmode -shell-escape $(target)

preview: 
	evince $(target).pdf

clean:
	\rm $(target).aux
	\rm $(target).bbl
	\rm $(target).blg
	\rm $(target).dvi
	\rm $(target).lof
	\rm $(target).log
	\rm $(target).pdf

## *EOF*
