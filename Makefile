# Autor: Magdalena Zych
# Data: 23.04.2020

CC=gcc

all: server sensors

server: server.c parameters.c
		$(CC) server.c parameters.c -o server

sensors: normalDistribution.c sensor_function.c sensors.c parameters.c
		$(CC) normalDistribution.c sensor_function.c sensors.c parameters.c -o sensors -lpthread -lm
