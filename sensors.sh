#!/bin/bash

function debug()
{
	local msg="$1"

	echo "[debug] $msg" >&2
}

if ! which sensors > /dev/null 2>&1; then
	echo "ERROR: Sensors command is not available."
	echo "       Try installing lm(-|_)sensors package"
	exit 1
fi

tmp=0
num=0
coretemps="$(sensors 2> /dev/null | grep "Core" | awk '{split($0, a, ":"); split(a[2], b, " "); print b[1];}' | tr -d '+°C')"
for ct in ${coretemps[@]}
do
	tmp="$(python -c "print '%.2f' % (float($ct) + float($tmp))")"
	debug "Core$num: $ct °C"

	let num=$num+1
done

coretemps="$(sensors 2> /dev/null | grep "Core" | awk '{split($0, a, ":"); split(a[2], b, " "); print b[1];}' | tr -d '+°C')"
max="$(sensors 2> /dev/null | grep "Core" | awk '{split($0, a, "crit"); c=split(a[2], b, " "); print b[c]}' | tr -d '+°C)' | head -n 1)"

if [ "x$num" == "x0" ]; then
	echo "-1/-1"
	exit 0
fi

at="$(python -c "print '%.2f' % float($tmp / $num)")"
if [ "x$HTOP_FORMAT" == "x1" ]; then
	echo "$at/$max"
else
	echo "Average temperature is $at°C (max $max°C)"
fi
