#!/bin/bash

#make-run.sh
#make sure a Proc is always running.

process="residential 56"
makerun="/root/residential 56"

if ps ax | grep -v grep | grep "$process" > /dev/null
then
	exit
else
	$makerun &
fi

process="pretreatment.py"
makerun="/root/pretreatment.py"

if ps ax | grep -v grep | grep "$process" > /dev/null
then
        exit
else
        $makerun &
fi

