# TIN
Celem projektu była implementacja podsystemu komunikacji dla systemu kontrolowania                
warunków panujących w szklarni, a dokładniej między bramą radiową, panelem kontrolnym                    
i czujnikami, rozmieszczonymi w jej wnętrzu, mierzącymi temperaturę i wilgotność                  
powietrza lub wilgotność gleby. 

System jest podzielony na trzy moduły. Pierwszy z nich symuluje rzeczywiste czujniki                      
dokonujące pomiarów i wysyła dane pomiarowe za pomocą interfejsu komunikacyjnego                  
802.11. Drugi moduł odbiera wysyłane pakiety i zapisuje je w specjalnie sformatowanych                      
plikach. Trzeci zaś pozwala na dynamiczną konfigurację częstotliwości wysyłania pakietów                  
dla każdego czujnika, a także pozwala na wykonanie diagnostyki, polegającej na                    
sprawdzeniu poprawności wysyłania pakietów.

Ponadto dzięki, zaprojektowanemu specjalnie w tym celu, modułowi do programu                  
Wireshark umożliwiliśmy obserwowanie i analizę wysyłanych pakietów.
