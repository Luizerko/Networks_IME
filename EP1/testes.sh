#!/bin/bash

PIDs=();

for i in $(seq 1 100); do
	mosquitto_sub -t "topico${i}" -V 5 &
	PIDs[${i}]=$!;
done

for i in $(seq 1 50); do
	for j in $(seq 1 100); do
		mosquitto_pub -t "topico${j}" -m "diga, rapaz ${j}" -V 5;
		sleep 0.1;
	done
	#sleep 20;
done

#sleep 10;

for i in $(seq 1 100); do
	kill ${PIDs[${i}]};
done

sleep 30;
