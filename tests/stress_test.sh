COUNTER=0

while [ 1 ]
do
	if [ $COUNTER -eq "100" ]
	then
		exit
	fi

	echo $COUNTER
	COUNTER=$(expr $COUNTER + 1)
	curl http://localhost:8080 > to_del &
	# sleep 1
done
