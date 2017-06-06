LATEX	= latex -shell-escape
#BIBTEX	= bibtex
DVIPS	= dvips
DVIPDF  = dvipdft
XDVI	= xdvi -gamma 4
GH		= gv

#EXAMPLES = $(wildcard *.c)
CC:=gcc
CFLAGS:= -Wall -pthread
SRC	:= $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
TRG	= $(SRC:%.tex=%.dvi)
PSF	= $(SRC:%.tex=%.ps)
PDF	= $(SRC:%.tex=%.pdf)


BASE = ${wildcard *.tex}
FILES = ${BASE:%.tex=%.pdf}

LOPT =	-interaction=batchmode
LOPT2 = -shell-escape ${LOPT}

pdf: pdf1

ps: $(PSF)

$(TRG): %.dvi: %.tex $(EXAMPLES)
	#one way of including source code is to use pygments
	#pygmentize -f latex -o __${EXAMPLES}.tex ${EXAMPLES}
	#requires that you \include{pygments.tex} in your preamble

	$(LATEX) $<
#	$(BIBTEX) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<
	#remove the pygmentized output to avoid cluttering up the directory
	#rm __${SRC}.tex


$(PSF):%.ps: %.dvi
	$(DVIPS) -R -Poutline -t letter $< -o $@

$(PDF): %.pdf: %.ps
	ps2pdf $<
	

show: $(TRG)
	@for i in $(TRG) ; do $(XDVI) $$i & done

showps: $(PSF)
	@for i in $(PSF) ; do $(GH) $$i & done

concurrency2:
	$(CC) concurrency2.c $(CFLAGS) -o concurrency2

all: pdf1 

pdf1:	${BASE:%.tex=%}

%:	%.tex
	pdflatex ${LOPT2} $*
	- bibtex $*
	pdflatex ${LOPT2} $*
	pdflatex ${LOPT2} $*


clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg concurrency2

.PHONY: all show clean ps pdf showps
