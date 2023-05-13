#include "funkcje.h"

/*

Jak uruchomić program??
1. wchodzimy konsola do folderu gdzie mamy wszystkie pliki
2. $ make
3. $ ./output [-d] [-o -|nazwawyjsciowego] [-m s|r|g|b] [-n] [-k] [-r] [-i -|nazwawejsciowego]

-d    wyswietlenie obrazu po zakonczeniu dzialania programu
-o    nazwa pliku do ktorego zapiszemy obraz, jesli jest '-' to wtedy wypisac na domyslne wyjscie
-m    wybor koloru do przetwarzania s-konwersja do szartosci r-czerwony g-zielony b-niebieski
-n    negatyw
-k    kontur
-r    rozmycie poziome
-i    nazwa wczytywanego przez program pliku, jesli jest '-' to wtedy czytaj z domyslnego wyjscia

!WAZNE!

Aby wczytywanie obrazu za pomoca -i - dzialalo nalezy podac opcje -i - na samym koncu naszych funkcji np:
./a.out -d -o nowy.ppm -k -n -i -     DOBRZE
./a.out -d -o nowy.ppm -i - -n -k     ZLE

!WAZNE!


*/

int main(int argc, char ** argv) {


  str_obrazu obraz;           //struktury obrazu i pomocnicza
  str_pomoc pomoc;
  
  char nazwa[DL_NAZWY];       //tutaj bedzie przechowywana nazwa pliku
  int odczytano =0;
  int a;

  int kod_bledu = 0;
  kod_bledu=przetwarzaj_opcje(argc,argv,&pomoc, &obraz);     //wywolanie funkcji przetwarzajacej opcje i potencjalne wylapanie bledow
  if (kod_bledu!=0){
    printf("Blad nr %d\n", kod_bledu);      //wypis bledu jesli wystapil
  }

  if (kod_bledu==0){            //brak bledow, przetwarzamy obraz
    if (pomoc.menu==0){
      //wczytanie obrazu
      czytaj(&obraz,&pomoc);
      if(pomoc.mono==1){   //zostala podana opcja konwersji do pgm wiec ja wykonujemy
        if(obraz.nr_mag==3) {
          konwersja(&obraz,&pomoc);
        } else if(obraz.nr_mag==2) {
          printf("Nie mozna wykonac konwersji do szarosci, obraz juz jest w formacie PGM!\n");
        }
      } 
      if (pomoc.konturowanie==1){             //user chce konturowac obraz, wykonujemy konturowanie
        kontur(&obraz,&pomoc);
      }
      if (pomoc.negatyw==1){                  //user chce zrobic negatyw obrazu, wykonujemy negatyw
        negatyw(&obraz,&pomoc); 
      }
      if (pomoc.rozmycie==1){                 //user chce rozmyc obraz, wykonujemy rozmycie
        rozmycie(&obraz,&pomoc);
      }
      if (pomoc.wyswietlenie==1){             //user chce wyswietlic obraz
        if (obraz.nr_mag==2){               //zapis dla obrazow PGM
          zapis(&obraz,&pomoc);       //wywolanie funkcji zapisu
          wyswietl(&pomoc,nazwa);           //wywolanie funkcji wyswietlanie
        }
        if (obraz.nr_mag==3){               //zapis dla obrazow PPM
          zapis(&obraz,&pomoc);       //wywolanie funkcji zapisu
          wyswietl(&pomoc,nazwa);           //wywolanie funkcji wyswietlania
        }
      }
      if (pomoc.wyswietlenie==0){  /*User nie chce wyswietlic, tylko zapisz*/
        zapis(&obraz,&pomoc);
      }
    }
  }


  free(obraz.tablica_obraz); //zwolnienie pamięci
  if(obraz.nr_mag==3) {
    free(obraz.czer);
    free(obraz.ziel);
    free(obraz.nieb);
  }

  if(pomoc.tmp==1) {
    delay(500);
    if(obraz.nr_mag==3) {
      system("rm tymczasowy.ppm");
    } else {
      system("rm tymczasowy.pgm");
    }
  }
}