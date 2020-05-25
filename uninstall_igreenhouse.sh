#!/bin/sh
# uninstall_igreenhouse.sh
# Autor: Krzysztof Gorlach
# 25.05.2020
# plik deinstalacyjny

echo "Rozpoczynam odinstalowywanie iGreenhouse."
echo "Odinstalowuję moduł serwera..."
rm server
echo "Odinstalowuję moduł sensorów..."
rm sensors
echo "Odinstalowuję moduł kontroli..."
rm control_mod
echo "Odinstalowuję moduł testów jednostkowych..."
rm testowanie
echo "Czy chcesz usunąć także bazę danych iGreenhouse?"
read -p "Wpisz 1 (Tak) lub 2 (Nie): " choice
if [ $choice -eq 1 ]; then
	echo "Usuwam bazę danych..."
	rm ./*-*-*.txt
fi
echo "Czy chcesz usunąć wyniki testów integracyjnych?"
read -p "Wpisz 1 (Tak) lub 2 (Nie): " choice2
if [ $choice2 -eq 1 ]; then
	echo "Usuwam wyniki testów integracyjnych..."
	rm ./integration_tests/*.txt
fi
echo "Deinstalacja zakończona sukcesem."
