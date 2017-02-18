#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

/*
 * TODO
 *
-sprawdzanie czy nie próbujemy odwołać się do tekstury out of range (złwaszcza po otwarciu pliku)
-context menu dla animacji, dla klatek animacji, dla kratem mapy
-możliwość umieszczania adnotacji w dowolnym miejscu planszy (gra może interpretować te adnotacje jako np. umieszenie jakiegoś ruchomego obiektu, albo punkt startu gry, albo meta, albo wroga postać)


tileselector
-addtile - dodaje klocek o podanej grafice pixmap
-sam segreguje sobie tilesy tak, żeby zmieścić je na danej szerokości
-tilesselected - emituje sygnał wybrania tilesa,
-getselectedtileindex - zwraca indexy zaznaczonych tilesów
-settilebuffer - bierze obiekt tilebuffer i ma dostęp do grafik i ich indextów

tiledmapeditor
-setcursortile - bierze indexy klocków i wyświetla je jako kursor
-settilebuffer - bierze obiekt tilebuffer i ma dostęp do grafik i ich indextów
-settilemap - bierze obiekt tilemap i może go edytować, odczytywać itd
-edytuje swój tilemap po kliknięciu w danym miejscu

tilebuffer
-posiada listę pixmapów, każdy pixmap ma przyporządkowany index
-potrawi wczytać plik textur i pociąć go sobie na poszczególne tilesy

tilemap
-posiada dwuwymiarową tablicę indexów, którym odpowiada każdy tiles
-potrawi wczytywać i zapisywać do pliku mapę

-------------------------------------

struktura pliku:

nagłówek typu pliku
nazwa planszy
wymiar kafelków x
wymiar kafelków y
ilość animacji
animacja 0
animacja 1
animacja 2
animacja 3
...
nagłówek początku mapy
ilość map(warstw)
mapa/warstwa 0
mapa/warstwa 1
mapa/warstwa 2
...
nagłówek końca pliku


--------------------
sekcja animacja:
ilość klatek
czy jest to animacja typu ping pong
szybkość fps 0 .. 50
klatka 0
klatka 1
klatka 2
...

--------------------
sekcja mapa/warstwa:
wymiar mapy x
wymiar mapy y
wartość 00
wartość 10
wartość 20
wartość 01
wartość 11
wartość 21
...

*/
