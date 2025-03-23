# SO2_p

Projekt przedstawia klasyczną implementację problemu Jedzących Filozofów z wykorzystaniem języka C++. Celem projektu jest demonstracja wykorzystania wielowątkowości oraz samodzielna implementacja mechanizmów synchronizacji.
Problem jedzących filozofów to klasyczny problem synchronizacji procesów. Służy on jako model ilustrujący zagrożenia związane z konkurencyjnym dostępem do zasobów współdzielonych oraz koniecznością zabezpieczenia tzw. sekcji krytycznych w programach wielowątkowych.
 
1. Opis problemu:
Grupa N filozofów siedzi przy okrągłym stole. Między każdym z dwóch sąsiednich filozofów znajduje się jeden widelec, co oznacza, że filozof ma dostęp do dwóch widelców: jednego po lewej stronie i drugiego po prawej.

Filozofowie cyklicznie przechodzą przez trzy stany:
	-Myślenie – filozof nie korzysta z żadnych zasobów
	-Głód – filozof chce jeść i próbuje zdobyć oba widelce
	-Jedzenie – filozof je, trzymając dwa widelce (lewy i prawy)

Aby filozof mógł rozpocząć jedzenie, musi jednocześnie zdobyć oba widelce. Po jedzeniu, filozof odkłada oba widelce i ponownie przechodzi w stan myślenia.
Bez odpowiednich zabezpieczeń program może napotkać na poważne problemy:
	-Deadlock (zakleszczenie) - Jeśli wszyscy filozofowie jednocześnie podniosą jeden widelec (np. lewy), a następnie będą czekać na drugi (prawy), żaden z nich nie będzie mógł kontynuować – system się zakleszczy.
	-Starvation (zagłodzenie) - Niektórzy filozofowie mogą nigdy nie uzyskać dostępu do widelców, ponieważ inni ciągle je przechwytują, co prowadzi do sytuacji, w której dany wątek nigdy nie zakończy swojego cyklu.
	-Race condition (warunek wyścigu) - Jeśli wielu filozofów jednocześnie próbuje sięgnąć po ten sam widelec, bez synchronizacji może dojść do niespójnego stanu danych.



2. Reprezentacja wątków:
W programie każdy filozof jest reprezentowany przez oddzielny wątek, uruchamiany za pomocą standardowej biblioteki wątków w C++ (std::thread).
Liczba filozofów (a tym samym liczba wątków) jest pobierana od użytkownika przy uruchomieniu programu. Każdy wątek działa niezależnie i odpowiada za zachowanie jednego filozofa, wykonując powtarzający się cykl: myślenie, próba jedzenia, jedzenie, odkładanie widelców
Stan każdego filozofa jest wypisywany w czasie rzeczywistym w terminalu, a kolory pomagają wizualnie rozróżnić fazy działania poszczególnych wątków.

3. Sekcje krytyczne
Sekcje krytyczne występują w momencie, kiedy filozof próbuje uzyskać dostęp do współdzielonych zasobów – widelców. Każdy widelec jest współdzielony przez dwóch sąsiadujących filozofów, dlatego musi być chroniony przed równoczesnym dostępem z dwóch wątków.
Do synchronizacji dostępu użyto własnoręcznie zaimplementowanego mutexa. Mutex działa w oparciu o blokadę logiczną i technikę aktywnego oczekiwania (busy-waiting).

Aby uniknąć zakleszczenia, filozofowie nie podnoszą widelców w jednakowej kolejności. Część z nich najpierw sięga po lewy widelec, a następnie po prawy, a pozostali – odwrotnie. Dzięki temu eliminowany jest warunek cyklicznego oczekiwania, co pozwala uniknąć całkowitej blokady systemu. W programie filozofowie dzielą się na dwie grupy:
	-Filozofowie o parzystym numerze najpierw sięgają po lewy widelec, a potem po prawy.
	-Filozofowie o nieparzystym numerze robią to w odwrotnej kolejności – najpierw po prawy, potem po lewy.
Dzięki temu nie dochodzi do sytuacji, w której wszyscy filozofowie próbują zdobyć ten sam widelec jako pierwszy.

4. Menu:
Program pobiera liczbę filozofów po czym (minimum 2), a następnie udostępni komendy:
s – start symulacji
p – pauza (zatrzymanie filozofów)
q – zakończenie programu