#!/bin/bash
cd paper/
pdflatex -output-directory=build main
cd build
bibtex main
cd ..
pdflatex -output-directory=build main
pdflatex -output-directory=build main
cd ..