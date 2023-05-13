#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>               //potrzebne biblioteki i definicje
#include <time.h>
#define DL_LINII 1024
#define DL_NAZWY 50

typedef struct {
    int **tablica_obraz;                                                        //zdefiniowanie tablicy dla obrazow pgm
    int wymx, wymy, odcienie, nr_mag;                                           //zdefiniowanie zmiennych przechowujacych wymiary, ilosc odcieni i numer magiczny
    int **czer, **ziel, **nieb;                                                 //zdefiniowanie trzech tablic przechowujacych informacje o pikselach obrazu PPM
} str_obrazu;

typedef struct {
    FILE *plik_we, *plik_wy;
    int menu,negatyw,konturowanie,rozmycie,wyswietlenie,r,g,b,mono,tmp;             //definicje potrzebne do poprawnego dzialania opcji
    char *nazwa_nowy;
} str_pomoc;

int czytaj(str_obrazu *obraz, str_pomoc *pomoc);
int zapis(str_obrazu *obraz, str_pomoc *pomoc);
int wyswietl(str_pomoc *pomoc, char nazwa[DL_NAZWY]);
int przetwarzaj_opcje(int argc, char **argv, str_pomoc *pomoc, str_obrazu *obraz);
int konwersja(str_obrazu *obraz, str_pomoc *pomoc);
int kontur(str_obrazu *obraz, str_pomoc *pomoc);
int negatyw(str_obrazu *obraz, str_pomoc *pomoc);
int rozmycie(str_obrazu *obraz, str_pomoc *pomoc);
void delay(int number_of_seconds);