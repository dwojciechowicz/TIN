# Autor: Magdalena Zych
# Data: 23.04.2020

CC=gcc

all: server sensors control_mod

server: server.c parameters.c
		$(CC) server.c parameters.c -o server -lpthread

sensors: normalDistribution.c sensor_function.c sensors.c parameters.c
		$(CC) normalDistribution.c sensor_function.c sensors.c parameters.c -o sensors -lpthread -lm

control_mod: control_mod.c parameters.c
		$(CC) control_mod.c parameters.c -o control_mod
