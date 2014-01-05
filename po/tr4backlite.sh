#!/bin/sh
for f in *.po
do
po2ts -i $f -o ${f%.*}.ts -c k9copy
done