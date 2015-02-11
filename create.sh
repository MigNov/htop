#!/bin/bash

rm -f htop.tgz

sourcedir="$(rpm --eval "%{_sourcedir}")"
name="$(cat htop.spec  | grep Name: | awk '{split($0, a, ":"); print a[2]}' | tr -d ' ')"
ver="$(cat htop.spec  | grep Version: | awk '{split($0, a, ":"); print a[2]}' | tr -d ' ')"
dir="$name-$ver"
mkdir -p $dir
cp -af * $dir 2> /dev/null
tar -zcf htop.tgz $dir
rm -rf $dir
rm -rf $sourcedir/htop.tgz
cp -af htop.tgz $sourcedir

rpms="$(rpmbuild -bb htop.spec | grep "Wrote:" | awk '{split($0, a, ":"); print a[2]}' | tr -d ' ')"

echo
echo "Moving generated packages ..."
echo
for file in ${rpms[@]}
do
	rm -f rpms/$file
	mv $file rpms
	echo "Package saved as rpms/$(basename $file) ..."
done

rm -f htop.tgz

echo
echo "All done"
exit 0
