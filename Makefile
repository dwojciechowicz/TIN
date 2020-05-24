# Makefile
# Autorzy: Magdalena Zych
#	   Krzysztof Gorlach
# Data: 23.04.2020

CC=gcc

all: server sensors control_mod testowanie

server: server.c parameters.c
		$(CC) server.c parameters.c -o server -lpthread

sensors: normalDistribution.c sensor_function.c sensors.c sensors_func.c parameters.c
		$(CC) normalDistribution.c sensor_function.c sensors.c sensors_func.c parameters.c -o sensors -lpthread -lm

control_mod: control_mod.c parameters.c control_func.c
		$(CC) control_mod.c parameters.c control_func.c -o control_mod

testowanie: tests.c control_func.c testowanie.c sensors_func.c parameters.c
		$(CC) tests.c control_func.c sensors_func.c testowanie.c parameters.c -o testowanie -lpthread -lcunit
