#!/bin/bash
# ----------------------
# do not edit below here 
# ----------------------
# Startverzögerung
echo "Scanner for Cli"
echo ""
i=5
while [[ $i -gt 0 ]]; do
	sleep 1
	echo "starts in "$i
	i=$[$i-1]
done
echo ""
while true; do
    echo "Scanning ..."
    sudo iwlist wlan0 scanning | grep "Address" | awk '{print $5}' && sudo iwlist wlan0 scanning | grep "ESSID:" | awk '{print $1}'
    NUMOFLINES=$(sudo iwlist wlan0 scanning | grep "Address" | awk '{print $5}' && sudo iwlist wlan0 scanning | grep "ESSID:" | awk '{print $1}')
		for j in $NUMOFLINES ; do
    sleep 1
	done
	done
exit 0
