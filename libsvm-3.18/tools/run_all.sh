#!/bin/bash
radfile="rad"
hjpdfile="hjpd"
hodfile="hod"

if [ -e $radfile ]; then
	echo ""
	echo "Starting easy.py rad rad.t"
	./easy.py rad rad.t
	echo "rad complete"
	echo ""
fi

if [ -e $hjpdfile ]; then
	echo ""
	echo "Starting easy.py hjpd hjpd.t"
	./easy.py hjpd hjpd.t
	echo "hjpd complete"
	echo ""
fi

if [ -e $hodfile ]; then
	echo ""
	echo "Starting easy.py hod hod.t"
	./easy.py hod hod.t
	echo "hod complete"
	echo ""
fi
