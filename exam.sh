#!/bin/bash
mkdir mydir
chmod 777 mydir
echo "2023" >myfile
mv moveme ./mydir/
cp copyme ./mydir/
mv ./mydir/copyme ./mydir/copied
cat readme
gcc bad.c 2>err.txt

if (($# == 1))
then
	a=$1
else
	a=10
fi

i=1
mkdir gen
while (($i <= $a))
do
	touch ./gen/${i}.txt
	i=$[$i+1]
done


