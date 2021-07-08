#!/usr/bin/env bash

for i in /contato /error /;
do
	echo "Fetch $i"
	curl -s -D - "http://localhost$i"
done
