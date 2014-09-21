#!/bin/bash
cd paper/
pdflatex main
bibtex main
pdflatex main
pdflatex main
cd ..