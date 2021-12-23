#!/bin/bash

usage() { echo "Usage: $0 [-o <string>] [-p <int>]" 1>&2; exit 1; }

declare -a queries=("select count(*) from r natural join s natural join t;",3
                                    "select count(*) from r natural join s natural join t natural join u;",4
                                    "select count(*) from r natural join s natural join t natural join u natural join v natural join w;",6)
									
declare -a fan_outs=(1 2 3 4 5)
declare -a ms=(30000 30000 30000 30000 30000)
declare -a ms_go3=(49996 25000 16668 12503 10005)
declare -a ms_go4=(50001 25002 16668 12503 10002)
declare -a ms_go6=(50005 25005 16674 12505 10005)

repeats=1	# cold benchmark per default
db_name="benchDB"
output_file=results/default_run.csv
log_file=results/default_run.log
 
while getopts n:o: OPT
do
	case "$OPT" in
		n)
			repeats=$OPTARG
			;;
		o)
			output_file=$OPTARG
			;;
		*)
			usage
			;;
	esac
done
shift $((OPTIND-1))

IFS='.' read name ending <<< "$output_file"
log_file="${name}.log"

rm $output_file 2> /dev/null
rm $log_file 2> /dev/null
mkdir -p "results/"

echo Repeats: $repeats | tee -a $log_file

for data in "${queries[@]}"
do
	IFS=',' read query query_id <<< "$data"
	mal_code=$(mclient -s "explain ${query}" ${db_name})
	if [[ $mal_code == *"iterator.new"* ]]
	then
		echo WCOJ MAL | tee -a $log_file
	else
		echo MonetDB MAL | tee -a $log_file
	fi
	
	if [[ $query_id == 3 ]]
	then
		ms=("${ms_go3[@]}")
	elif [[ $query_id == 4 ]]
	then
		ms=("${ms_go4[@]}")
	elif [[ $query_id == 6 ]]
	then
		ms=("${ms_go6[@]}")
	fi
	
	echo Benchmarking: $query | tee -a $log_file
	echo $query >> $output_file
	
	for (( i = 1; i <= $repeats ; i++ ))
	do
		echo "r;$i" | tee -a $output_file

		for j in "${!ms[@]}"
		do
			poetry run python3 gendb/main.py $query_id "${ms[$j]}" "${fan_outs[$j]}" > /dev/null
			mclient $db_name  "go${query_id}.sql" > /dev/null
		
			ts=$(date +%s%N)
			mclient -s "${query}" $db_name | tee -a $log_file > /dev/null
			tt=$((($(date +%s%N) - $ts)/1000000))
			
			echo "${ms[$j]};${fan_outs[$j]};$tt" | tee -a $log_file | tee -a $output_file
		done
		echo "" | tee -a $log_file
	done
done
