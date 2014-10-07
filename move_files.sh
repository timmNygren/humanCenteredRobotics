#!/bin/bash
radfile="rad"
hjpdfile="hjpd"
hodfile="hod"

if [ -e $radfile ]; then
	cp rad libsvm-3.18/tools/rad
	cp rad.t libsvm-3.18/tools/rad.t
fi

if [ -e $hjpdfile ]; then
	cp hjpd libsvm-3.18/tools/hjpd
	cp hjpd.t libsvm-3.18/tools/hjpd.t
fi
