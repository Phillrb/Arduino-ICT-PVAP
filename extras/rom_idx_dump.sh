#!/bin/bash

function log2 {
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

test -s "$1" || { echo "Need a file with size greater than 0!"; exit 1; }

FILESIZE=$(stat -f %z "$1")
no_of_idx=$(log2 "$FILESIZE")
a=0
b=0
ARRAY=()
c=0

#populate array
while [ "$b" -lt "$no_of_idx" ]
do
	c=$((2**$b))
	((c++))
	ARRAY+=($c)
((b++))
done

#Checksum
CHKSTR="$(crc32 $1)"
CHK="$(echo $CHKSTR | awk '{printf toupper($0)}')"
printf 'CRC: 0x%s\n' "$CHK"


#construct useable string
printf "static const UINT16 s_romData_$(basename $1)[] = {"

for i in "${ARRAY[@]}"
do 
	:
	 cat $1 | head -c$i | tail -c1 | xargs -0 -I{} printf '0x%02x' "'{}"
	 
	 #comma separate values
	 ((a++))
	 if [ "$a" -lt "$no_of_idx" ]; then
	 	printf ","
	 fi
done

printf "};\n"
