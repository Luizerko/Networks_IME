#!/bin/bash

PIDs=();

mosquitto_sub -t "topico1" -V 5 &
PIDs[1]=$!;

for i in $(seq 1 10); do
	mosquitto_pub -t "topico1" -m "diga${i}" -V 5;
	#sleep 5;
done

sleep 20;

kill PIDs[1];
