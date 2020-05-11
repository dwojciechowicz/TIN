# Autor: Magdalena Zych
# Data: 23.04.2020

CC=gcc

all: server sensors stop_sensors diagnostics

server: server.c parameters.c
		$(CC) server.c parameters.c -o server -lpthread

sensors: normalDistribution.c sensor_function.c sensors.c parameters.c
		$(CC) normalDistribution.c sensor_function.c sensors.c parameters.c -o sensors -lpthread -lm

stop_sensors: stop_sensors.c parameters.c
		$(CC) stop_sensors.c parameters.c -o stop_sensors

diagnostics: diagnostics.c parameters.c
		$(CC) diagnostics.c parameters.c -o diagnostics
