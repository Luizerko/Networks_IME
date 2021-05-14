#!/bin/bash

PIDs=();

for i in $(seq 1 50); do
	mosquitto_sub -t "topico1" -V 5 &
	PIDs[${i}]=$!;
done

for i in $(seq 1 5); do
	mosquitto_pub -t "topico1" -m "diga${i}" -V 5;
	sleep 5;
done

for i in $(seq 1 5); do
	kill ${PIDs[${i}]};
done
