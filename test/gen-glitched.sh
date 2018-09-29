#!/bin/sh
rm -rf oframes
mkdir oframes
xscreenshot > shot.ff

for i in $(seq 0 63); do
	cat shot.ff | ff-glitch2 > $(printf "oframes/%04d.ff" $i)
done

cat oframes/*.ff | ../fhpack loop 1 1 > anim.fh
