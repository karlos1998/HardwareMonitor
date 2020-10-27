# HardwareMonitor

Projekt stworzony w środowisku Arduino mający na celu wyświetlenie informacji o zużyciu komponentów Twojego PC, typu:

CPU: temperatura i obciążenie;
GPU: temperatura i obciążenie;
RAM: zużycie procentowe i liczbowe;
Nazwa komputera;


----------------------
Jak zacząć...
Przede wszystkim potrzebujesz swojej płytki ESP i wyświetlacza na którym te dane będą się wyświetlaly.
Żeby odtworzyć mój projekt dam Ci wszystko, czego potzebujesz.

-- Płytka Wemos D1 mini
Zakupiłem ją tutaj:
https://allegro.pl/oferta/modul-wifi-nodemcu-d1-mini-esp8266-arduino-7412645249

-- Wyświetlacz OLED:
Zakupiłem go tutaj:
https://allegro.pl/oferta/wyswietlacz-oled-0-96-ssd1306-blue-arduino-spi-i2c-7636893024

-- Obudowa, w której umieściłem akcesoria
https://www.thingiverse.com/thing:3080488
! Jeśli jesteś zainteresowany wydrukiem takiej samej obudowy napisz do mnie!
Dam Ci namiar na zaprzyjaźnioną mi firmę, której zleciłem wykonanie :)


-----------------------
Instalacja...

Na początek na swoim komputerze musisz zainstalować oprogramowanie Open Hardware Monitor dostępne do pobrania tutaj:
https://openhardwaremonitor.org
Po co Ci ona? Ta aplikacja ma w sobie wbudowany webserwer. Oznacza to, że po wejściu na lokalne ip twojego komputera o danym porcie dostaniesz dane w JSON z aktualnym zużyciem komputera. Właśnie z tego korzystam.

Po instalacji uruchom aplikację i skonfiguruj ją jak następuje:
Na górze kliknij przycisk Options i zaznacz "Run on Windows Startup" - powód wiadomy
Następnie mamy "Remote Web Server" i zaznaczamy "Run"
Od teraz zawsze aplikacja będzie nam się sama włączać z dostępnym na porcie 8085 monitor.
Sprawdź to wchodząc na http://localhost:8085

Jeśli nic się tam nie znajduje zmień proszę port w "Remote Web Server" -> "Port" na ten 8085. Jepiej byś użył tego co ja, bo po prostu aplikacja będzie z niego korzystać. Oczywiście zawsze możesz wybrać jaki Ci pasuje i zmienić moim projekcie, ale tak będzie łatwiej dla nas obojga.

Dalej, potrzebujesz dostać się do zapory sieciowej i zrobić wyjątek na wyjście i wejście.
Stwórz nowy wyjątek i podaj wtedy ten port.
Zapisz i najlepiej spróbuj z drugiego urządzenia wejść na ip lokalne Twojego PC z tym portem i zobacz, czy po prostu się coś pojawia.

Jeśli działa, aplikację możesz zamknąć "X" a ona nadal pozostanie włączona w tle.

Dalej, potrzebujesz Arduino IDE, wgrania kodu no i instalację brakujących bibliotek, jeśli ich nie masz.

Jeśli projekt poprawinie się skompiluje i wgra na urządzenie po USB - to dobry znak, ale jest jeszcze jedna niepewność....

Na tym wyświetlaczu miałem problem z pokazaniem czegokolwiek, bo niby miałem działające biblioteki, ale niewłaściwe. Jeśli zajdzie potrzeba odszukam Ci to co ja mam i wgram do projektu. Liczę jednak, że Ci się uda :)
Na pomoc zawsze możesz liczyć. Napisz chociażby na maila podanego na dole tego opisu.

Jeśli wszystko poszło okej i widzisz na wyświetlaczu informacje o próbie łączenia z wifi za kilka sekund prawdopodobnie dostaniesz informację, że się to nie udało i płytka stworzyła swój Access Point. Wszystkie dane logowania zostaną wyświetlone na OLEDzie.

Połacz się z siecią nawet w telefonie i w formularzu znajdź swoją nazwę sieci domowej i wprowadź hasło. Płytka się zresetuje i połaczy z WIFI :)
Po połączeniu wemos przeskanuje sieć w poszukiwaniu wewnętrznego adresu z otwartym portem wspomnianym wyżej.
Jeśli mu się to nie uda za pierwszym razem, za drugim zrobi to wolniej, czekając dłuzej na odpowiedź ze strony hosta. Poczekaj do końca :)

Od teraz masz już działajacy projekt ! :)


-------------------------------------

Kontakt:
kontakt@letscode.it
