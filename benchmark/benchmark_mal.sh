#!/bin/bash

for m in 10 100 500 1000 2000 5000 10000 15000 20000 30000 40000
do
    poetry run python3 gendb/main.py 3 ${m} 1 > /dev/null
	mclient benchDB go3.sql > /dev/null

    echo ""
    echo ${i}
	ts=$(date +%s%N)
	mclient  -l msql -d benchDB go3_wcoj_mal > /dev/null
	tt=$((($(date +%s%N) - $ts)/1000000))
			
    echo "$m;$tt"
done

