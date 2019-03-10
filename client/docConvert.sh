#!/bin/bash

error () {
    echo $@ >/dev/stderr
}

die () {
    error $@
    exit 1
}

#Sourcing BASEURL
source ~/.docconvertrc

infile=$1
outfile=$2

if [ ! -r $infile ]
then
    die Cannot open "${infile}"
fi

inmime=$(file -b --mime-type $infile)

insuffix="${infile##*.}"
outsuffix="${outfile##*.}"

tmpfile=$(mktemp)

cmd="curl -s -D - -F \"file=@${infile};type=${inmime}\" $BASEURL/$outsuffix -o $tmpfile";
echo "$cmd"
result=$(curl -s -D - -F "file=@${infile};type=${inmime}" $BASEURL/$outsuffix -o $tmpfile | egrep -o '^HTTP/[[:digit:]]\.[[:digit:]] [[:digit:]]{3}' | cut -d' ' -f2 | tail -n 1)

errorState=false

case $result in
    200)
	mv $tmpfile $outfile
	;;
    404)
	echo "No converter available for converting to '$outsuffix'"
	errorState=true
	;;
    415)
	echo "Converting '$insuffix' to '$outsuffix' is not supported"
	errorState=true
	;;
    *)
	echo Unknown error $result
	errorState=true
	;;
esac

if $errorState
then
    echo $tmpfile
    #rm $tmpfile
    exit 1
fi
