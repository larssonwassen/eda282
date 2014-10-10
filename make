#!/bin/bash

function makePaper {
	echo "Building paper... \n"
	cd paper/
	pdflatex -output-directory=build main
	cd build
	bibtex main
	cd ..
	pdflatex -output-directory=build main
	pdflatex -output-directory=build main
	cd ..
	cp paper/build/main.pdf ./paper.pdf
}

function makeLab {
	echo "Building lab report... \n"
	cd lab/report
	pdflatex -output-directory=build main
	pdflatex -output-directory=build main
	cd ../..
	cp lab/report/build/main.pdf ./labreport.pdf
}

function quit {
	echo "Usage ./make <paper, lab, all>"
	exit
}


INPUT=$1
if [ $# -eq 0 ]; then
	quit
fi
if [ $INPUT = "paper" ];then
	makePaper
elif [ $INPUT = "lab" ];then
	makeLab
elif [ $INPUT = "all" ];then
	makePaper
	makeLab
else
	quit
fi


