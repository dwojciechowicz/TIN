#!/bin/sh
# simulation.sh
# Autor: Krzysztof Gorlach
# 25.05.2020
# skrypt symulujacy dzialanie serwera, sensorow i modulu kontrolujacego

# przygotowanie programu
cd ..
make

echo "Rozpoczynam testowanie..."

# symulacja 1
echo "Test 1 start"
./server > integration_tests/sim_server.txt &  # wlaczenie serwera
sleep 1
./sensors 1 1 10 > integration_tests/sim1_sensors.txt & # uruchomienie sensora i przekierowanie do pliku
sleep 5
./control_mod stop > integration_tests/sim1_control.txt # zatrzymanie pracy czujnikow
echo "Test 1 stop"

sleep 1

# symulacja 2
echo "Test 2 start"
./server >> integration_tests/sim_server.txt &  # wlaczenie serwera
sleep 1
./sensors 50 50 50 > integration_tests/sim2_sensors.txt & # uruchomienie sensorów i przekierowanie do pliku
sleep 10
./control_mod diag > integration_tests/sim2_control.txt # sprawdzenie liczby wysłanych i otrzymanych pakietów
sleep 10
./control_mod diag >> integration_tests/sim2_control.txt # sprawdzenie liczby wysłanych i otrzymanych pakietów
sleep 5
./control_mod stop >> integration_tests/sim2_control.txt # zatrzymanie pracy czujnikow
echo "Test 2 stop"

# symulacja 3

echo "Test 3 start"
./server >> integration_tests/sim_server.txt &  # wlaczenie serwera
sleep 1
./sensors 2 2 2 > integration_tests/sim3_sensors.txt & # uruchomienie sensora i przekierowanie do pliku
sleep 3
./control_mod diag > integration_tests/sim3_control.txt # sprawdzenie liczby wysłanych i otrzymanych pakietów
sleep 1
./control_mod para 2 1 10 >> integration_tests/sim3_control.txt # zmiana czasu dla czujnika o typie 2 i numerze 2
sleep 40
./control_mod stop >> integration_tests/sim3_control.txt # zatrzymanie pracy czujnikow
echo "Test 3 stop"

echo "Testowanie zakończone"
