# Makefile
# Autorzy: Magdalena Zych
#	   Krzysztof Gorlach
# Data: 23.04.2020

CC=gcc

all: server sensors control_mod testowanie

server: server.c parameters.c server_func.c socket_preparation.c
		$(CC) server.c parameters.c server_func.c socket_preparation.c -o server -lpthread -D_REENTRANT

sensors: normalDistribution.c sensor_function.c sensors.c sensors_func.c parameters.c socket_preparation.c
		$(CC) normalDistribution.c sensor_function.c sensors.c sensors_func.c parameters.c socket_preparation.c -o sensors -lpthread -D_REENTRANT -lm

control_mod: control_mod.c parameters.c control_func.c
		$(CC) control_mod.c parameters.c control_func.c -o control_mod

testowanie: tests.c control_func.c testowanie.c sensors_func.c parameters.c socket_preparation.c
		$(CC) tests.c control_func.c sensors_func.c testowanie.c parameters.c socket_preparation.c -o testowanie -lpthread -D_REENTRANT -lcunit
