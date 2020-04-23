# Autor: Magdalena Zych
# Data: 23.04.2020

CC=gcc

all: server sensors

server: server.c
		$(CC) server.c -o server

sensors: sensor_function.c sensors.c
		$(CC) sensor_function.c sensors.c -o sensors -lpthread
