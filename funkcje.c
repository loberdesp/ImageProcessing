#include "funkcje.h"
#define W_OK 0                      //zdefiniowane kody błędów
#define B_NIEPOPRAWNAOPCJA -1
#define B_BRAKNAZWY   -2
#define B_BRAKWARTOSCI  -3
#define B_BRAKPLIKU   -4

void delay(int number_of_seconds)                   //funkcja dzieki ktorej mozna opoznic pewne wykonanie funkcji
{
    // Converting time into milli_seconds           //byly pewne problemy z rownoczesnym uzyciem -o - oraz -d wiec musialem wprowadzic opoznienie jako funkcje pomocnicza
  int milli_seconds = 1000 * number_of_seconds;     //zrobilbym inaczej gdyby tylko byly proste do uzycia funckje asynchroniczne w C
    // Storing start time
  clock_t start_time = clock();
    // looping till required time is not achieved
  while (clock() < start_time + milli_seconds)
    ;
}

//funkcja czytania danych z pliku wejściowego
int czytaj(str_obrazu *obraz, str_pomoc *pomoc) {
  char buf[DL_LINII];
  int znak;                         //dodatkowe zmienne pomocnicze
  int koniec=0;
  int i,j,k;
  


  if (pomoc->plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
  }

  /* Sprawdzenie numeru magicznego" */
  if (fgets(buf,DL_LINII,pomoc->plik_we)==NULL) {               //jesli pobranie numeru nie powiodlo sie to skoncz dzialanie
    koniec=1;
  }
  if ( (buf[0]!='P') || ((buf[1]!='2') && (buf[1]!='3')) || koniec) {     //koniec pracy jesli niepoprawny numer magiczny
    printf("%c,%c",buf[0],buf[1]);
    fprintf(stderr,"Blad: To nie jest plik PGM ani PPM\n");
    return(0);
  } else {
    if(buf[1]=='2') {       //obraz jest formatu PGM
      obraz->nr_mag=2;
    }
    if(buf[1]=='3') {       //obraz jest formatu PPM
      obraz->nr_mag=3;
    }
  }
  do {
    if ((znak=fgetc(pomoc->plik_we))=='#') {      //czytanie komentarza
      if (fgets(buf,DL_LINII,pomoc->plik_we)==NULL)
	koniec=1;
    }  else {
      ungetc(znak,pomoc->plik_we);
    }
  } while (znak=='#' && !koniec);

  //pobranie i zapisanie wymiarow i ilosci odcieni obrazu
  if (fscanf(pomoc->plik_we,"%d %d %d",&(obraz->wymx),&(obraz->wymy),&(obraz->odcienie))!=3) {
    fprintf(stderr,"Blad: Brak wymiarow obrazu lub liczby stopni szarosci\n");
    return(0);
  }
  
  //alokacja pamieci dla obrazow pgm
  obraz->tablica_obraz=(int**)malloc(obraz->wymy*sizeof(int*));
  for (k = 0; k < obraz->wymy; k++){
    obraz->tablica_obraz[k]=(int*)malloc(obraz->wymx*sizeof(int));
  }
  if(obraz->tablica_obraz==NULL){
    fprintf(stderr,"Przydzielenie pamieci dla .pgm nie powiodlo sie\n");          //komunikat na wypadek gdyby alokacja nie powiodla się
  } 


  //alokacja pamieci dla obrazow ppm
  if(obraz->nr_mag==3){
    obraz->czer=(int**)malloc(obraz->wymy*sizeof(int*));
    obraz->ziel=(int**)malloc(obraz->wymy*sizeof(int*));
    obraz->nieb=(int**)malloc(obraz->wymy*sizeof(int*));
    for (k = 0; k < obraz->wymy; k++){
      obraz->czer[k]=(int*)malloc(obraz->wymx*sizeof(int));
      obraz->ziel[k]=(int*)malloc(obraz->wymx*sizeof(int));
      obraz->nieb[k]=(int*)malloc(obraz->wymx*sizeof(int));
    }
    if(obraz->czer==NULL || obraz->ziel==NULL || obraz->nieb==NULL){
      fprintf(stderr,"Przydzielenie pamieci dla .ppm nie powiodlo sie\n");        //komunikat na wypadek gdyby alokacja nie powiodla się
    }
  }

  //zczytanie wartosci pikseli i zapis do tablicy dla obrazow pgm
  if(obraz->nr_mag==2){
    for (i=0;i<obraz->wymy;i++) {
      for (j=0;j<obraz->wymx;j++) {
        if (fscanf(pomoc->plik_we,"%d",&(obraz->tablica_obraz[i][j]))!=1) {
          fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");           //blad wymiarow
          return(0);
        }
      }
    }
  }

  //zczytanie wartosci pikseli i zapis do trzech tablic kolorow dla obrazow ppm
  if(obraz->nr_mag==3){
    for (i=0;i<obraz->wymy;i++) {
      for (j=0;j<obraz->wymx;j++) {
        if (fscanf(pomoc->plik_we,"%d",&(obraz->czer[i][j]))!=1) {
        fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");                 //blad wymiarow
        return(0);
        }
        if (fscanf(pomoc->plik_we,"%d",&(obraz->ziel[i][j]))!=1) {
        fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");                 //blad wymiarow
        return(0);
        }
        if (fscanf(pomoc->plik_we,"%d",&(obraz->nieb[i][j]))!=1) {
        fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");                 //blad wymiarow
        return(0);
        }
      }
    }
  }

fclose(pomoc->plik_we);
return obraz->wymx*obraz->wymy;         //zamkniecie czytanego pliku i zwrocenie ilosci pikseli
}

//funkcja obslugujaca zapis obrazu do nowego pliku
int zapis(str_obrazu *obraz, str_pomoc *pomoc) {
//przypadek gdy w opcjach podane jest -o - oraz -d, potrzebujemy pliku tymczasowego do wyswietlenia obrazu
  if(pomoc->tmp==1) {
    if(obraz->nr_mag==3) {
      FILE *plik_wy_tmp=fopen("tymczasowy.ppm","w");
      fprintf(plik_wy_tmp, "P3\n%d %d\n%d\n", obraz->wymx, obraz->wymy, obraz->odcienie);       //zapis wymiarow i ilosci odcieni
      for(int a=0;a<obraz->wymy;a++) {
        for(int b=0;b<obraz->wymx;b++) {
          fprintf(plik_wy_tmp,"%d ",obraz->czer[a][b]);
          fprintf(plik_wy_tmp,"%d ",obraz->ziel[a][b]);
          fprintf(plik_wy_tmp,"%d ",obraz->nieb[a][b]);                   //wypisanie kolejnych wartosci kolorow
        }
      }
      fclose(plik_wy_tmp);
      system("display tymczasowy.ppm &");
    } else if(obraz->nr_mag==2) {
      FILE *plik_wy_tmp=fopen("tymczasowy.pgm","w");
      fprintf(plik_wy_tmp, "P2\n%d %d\n%d\n", obraz->wymx, obraz->wymy, obraz->odcienie);       //zapis wymiarow i ilosci odcieni
      for(int a=0;a<obraz->wymy;a++) {
        for(int b=0;b<obraz->wymx;b++) {
          fprintf(plik_wy_tmp,"%d ",obraz->tablica_obraz[a][b]);        //wypisanie wartosci
        }
      }
      fclose(plik_wy_tmp);
      system("display tymczasowy.pgm &");
    }
    pomoc->plik_wy=stdout;
  }

  FILE *plik_wy=pomoc->plik_wy;

  if(obraz->nr_mag==2) {        //zapis dla obrazow pgm
    fprintf(plik_wy, "P2\n%d %d\n%d\n", obraz->wymx, obraz->wymy, obraz->odcienie);       //zapis wymiarow i ilosci odcieni
    for(int a=0;a<obraz->wymy;a++) {
      for(int b=0;b<obraz->wymx;b++) {
        fprintf(plik_wy,"%d ",obraz->tablica_obraz[a][b]);        //wypisanie wartosci
      }
    }
  }

  if(obraz->nr_mag==3) {        //zapis dla obrazow ppm
  fprintf(plik_wy, "P3\n%d %d\n%d\n", obraz->wymx, obraz->wymy, obraz->odcienie);       //zapis wymiarow i ilosci odcieni
    for(int a=0;a<obraz->wymy;a++) {
      for(int b=0;b<obraz->wymx;b++) {
        fprintf(plik_wy,"%d ",obraz->czer[a][b]);
        fprintf(plik_wy,"%d ",obraz->ziel[a][b]);
        fprintf(plik_wy,"%d ",obraz->nieb[a][b]);                   //wypisanie kolejnych wartosci kolorow
      }
    }
  }
  fclose(plik_wy);
}

//funkcja tworzaca polecenie za pomoca ktorego mozemy wyswietlic obraz
int wyswietl(str_pomoc *pomoc, char nazwa[DL_NAZWY]) {
  if(pomoc->tmp==1) {
    return 0;
  } 
  char polecenie[DL_LINII];       //tablica pomocnicza do stworzenia polecenia
  strcpy(polecenie,"display ");
  strcat(polecenie,pomoc->nazwa_nowy);      //dodanie nazwy nowego pliku do polecenia
  strcat(polecenie," &");
  printf("%s\n",polecenie);
  system(polecenie);          //wywolanie polecenia
}

//funkcja przeksztalcajaca obrazy PPM do formatu PGM
int konwersja(str_obrazu *obraz, str_pomoc *pomoc) {
  int a = 0;
  obraz->nr_mag=2;        //zmiana numeru magicznego
  for(int i=0;i<obraz->wymy;i++) {
    for(int j=0;j<obraz->wymx;j++) {
      obraz->tablica_obraz[i][j] = ((obraz->czer[i][a]+obraz->ziel[i][a]+obraz->nieb[i][a])/3);       //usrednienie trzech wartosci i zapisanie do jednej tablicy
      a++;
    }
    a=0;
  }
  a=0;
}

//funkcja obslugujaca konturowanie obrazu
int kontur(str_obrazu *obraz, str_pomoc *pomoc) {
  int a,b,c;
  for(a=0;a<obraz->wymy-1;a++) {
    for(b=0;b<obraz->wymx-1;b++) {
      if(pomoc->mono==1 || obraz->nr_mag==2) {                //konturowanie dla obrazow PGM i obrazow PPM ktore zostaly wywolane z funkcja przeksztalcenia do PGM
        obraz->tablica_obraz[a][b]=abs(obraz->tablica_obraz[a+1][b]-obraz->tablica_obraz[a][b])+abs(obraz->tablica_obraz[a][b+1]-obraz->tablica_obraz [a][b]);
      }
      if(pomoc->r==1 && obraz->nr_mag==3) {
        obraz->czer[a][b]=abs(obraz->czer[a+1][b]-obraz->czer[a][b])+abs(obraz->czer[a][b+1]-obraz->czer[a][b]);  
      }
      if(pomoc->g==1 && obraz->nr_mag==3) {                 //konturowanie dla obrazow PPM z wybranymi kolorami do przetwarzania
        obraz->ziel[a][b]=abs(obraz->ziel[a+1][b]-obraz->ziel[a][b])+abs(obraz->ziel[a][b+1]-obraz->ziel[a][b]);
      }
      if(pomoc->b==1 && obraz->nr_mag==3) {
        obraz->nieb[a][b]=abs(obraz->nieb[a+1][b]-obraz->nieb[a][b])+abs(obraz->nieb[a][b+1]-obraz->nieb[a][b]);
      }
      if(pomoc->r==0 && pomoc->g==0 && pomoc->b==0 && obraz->nr_mag==3) {       //konturowanie dla obrazow PPM bez wybrania szczegolnych kolorow
        obraz->czer[a][b]=abs(obraz->czer[a+1][b]-obraz->czer[a][b])+abs(obraz->czer[a][b+1]-obraz->czer[a][b]);
        obraz->ziel[a][b]=abs(obraz->ziel[a+1][b]-obraz->ziel[a][b])+abs(obraz->ziel[a][b+1]-obraz->ziel[a][b]);
        obraz->nieb[a][b]=abs(obraz->nieb[a+1][b]-obraz->nieb[a][b])+abs(obraz->nieb[a][b+1]-obraz->nieb[a][b]);
      }
      if(obraz->nr_mag==3) {                      //niektore wartosci byly wieksze od ilosci odcieni co powodowalo bledy
        if(obraz->czer[a][b]>obraz->odcienie) {
          obraz->czer[a][b]=obraz->odcienie;
        }
        if(obraz->ziel[a][b]>obraz->odcienie) {   //jesli wartosc jest wieksza niz ilosc odcieni to przypisywana jest jej maksymalna wartosc odcieni
          obraz->ziel[a][b]=obraz->odcienie;
        }
        if(obraz->nieb[a][b]>obraz->odcienie) {
          obraz->nieb[a][b]=obraz->odcienie;
        }
      }
      if(obraz->tablica_obraz[a][b]>obraz->odcienie) {    //naprawienie bledu dla obrazow PGM
        obraz->tablica_obraz[a][b]=obraz->odcienie;
      }
    }
  }
}

//funkcja odpowiedzialna za operacje negatywu
int negatyw(str_obrazu *obraz, str_pomoc *pomoc) {
  int a,b;
  for(a=0;a<obraz->wymy;a++) {
    for(b=0;b<obraz->wymx;b++) {
      if(pomoc->mono==1 || obraz->nr_mag==2) {                  //negatyw dla obrazow PGM i obrazow PPM ktore zostaly wywolane z funkcja przeksztalcenia do PGM
        obraz->tablica_obraz[a][b]=obraz->odcienie-obraz->tablica_obraz[a][b];
      }
      if(pomoc->r==1 && obraz->nr_mag==3) {
        obraz->czer[a][b]=obraz->odcienie-obraz->czer[a][b];    //negatyw dla obrazow PPM z wybranymi kolorami do przetwarzania
      }
      if(pomoc->g==1 && obraz->nr_mag==3) {
        obraz->ziel[a][b]=obraz->odcienie-obraz->ziel[a][b];
      }
      if(pomoc->b==1 && obraz->nr_mag==3) {
        obraz->nieb[a][b]=obraz->odcienie-obraz->nieb[a][b];
      }
      if(pomoc->r==0 && pomoc->g==0 && pomoc->b==0 && obraz->nr_mag==3) {   //negatyw dla obrazow PPM bez wybrania szczegolnych kolorow
        obraz->czer[a][b]=obraz->odcienie-obraz->czer[a][b];
        obraz->ziel[a][b]=obraz->odcienie-obraz->ziel[a][b];
        obraz->nieb[a][b]=obraz->odcienie-obraz->nieb[a][b];
      }
    }
  }
}

//funkcja rozmywajaca obraz
int rozmycie(str_obrazu *obraz, str_pomoc *pomoc) {
  int a,b;
  for(a=0;a<obraz->wymy;a++) {
    for(b=0;b<obraz->wymx;b++) {
      if(b!=0 || b!=1 || b!=obraz->wymy-2 || b!=obraz->wymy-1) {    //pominiecie pikseli ktore nie pasuja do ogolnej postaci funkcji i powoduja segfaulta
        if(pomoc->mono==1 || obraz->nr_mag==2) {
          obraz->tablica_obraz[a][b]=(obraz->tablica_obraz[a][b-2]+obraz->tablica_obraz[a][b-1]+obraz->tablica_obraz[a][b]+obraz->tablica_obraz[a][b+1]+obraz->tablica_obraz[a][b+2])/5;            //rozmycie dla obrazow PGM i obrazow PPM ktore zostaly wywolane z funkcja przeksztalcenia do PGM
        }
        if(pomoc->r==1 && obraz->nr_mag==3) {
          obraz->czer[a][b]=(obraz->czer[a][b-2]+obraz->czer[a][b-1]+obraz->czer[a][b]+obraz->czer[a][b+1]+obraz->czer[a][b+2])/5;
        }
        if(pomoc->g==1 && obraz->nr_mag==3) {         //rozmycie dla obrazow PPM z wybranymi kolorami do przetwarzania
          obraz->ziel[a][b]=(obraz->ziel[a][b-2]+obraz->ziel[a][b-1]+obraz->ziel[a][b]+obraz->ziel[a][b+1]+obraz->ziel[a][b+2])/5;
        }
        if(pomoc->b==1 && obraz->nr_mag==3) {
          obraz->nieb[a][b]=(obraz->nieb[a][b-2]+obraz->nieb[a][b-1]+obraz->nieb[a][b]+obraz->nieb[a][b+1]+obraz->nieb[a][b+2])/5;
        }
        if(pomoc->r==0 && pomoc->g==0 && pomoc->b==0 && obraz->nr_mag==3) {   //rozmycie dla obrazow PPM bez wybrania szczegolnych kolorow
          obraz->czer[a][b]=(obraz->czer[a][b-2]+obraz->czer[a][b-1]+obraz->czer[a][b]+obraz->czer[a][b+1]+obraz->czer[a][b+2])/5;
          obraz->ziel[a][b]=(obraz->ziel[a][b-2]+obraz->ziel[a][b-1]+obraz->ziel[a][b]+obraz->ziel[a][b+1]+obraz->ziel[a][b+2])/5;
          obraz->nieb[a][b]=(obraz->nieb[a][b-2]+obraz->nieb[a][b-1]+obraz->nieb[a][b]+obraz->nieb[a][b+1]+obraz->nieb[a][b+2])/5;
        }
      }
    }
  }
}

/********************************************************************/
/*                                                                  */
/* ALTERNATYWNA DO PRZEDSTAWIONEJ NA WYKLADZIE WERSJA OPRACOWANIA   */
/* PARAMETROW WYWOLANIA PROGRAMU UWZGLEDNIAJACA OPCJE Z PARAMETRAMI */
/* Z ODPOWIEDNIO ZAPROPONOWANYMI STRUKTURAMI DANYCH PRZEKAZUJACYMI  */
/* WCZYTANE USTAWIENIA                                              */
/*                                    COPYRIGHT (c) 2007-2020 KCiR  */
/*                                                                  */
/* Autorzy udzielaja kazdemu prawa do kopiowania tego programu      */
/* w calosci lub czesci i wykorzystania go w dowolnym celu, pod     */
/* warunkiem zacytowania zrodla                                     */
/*                                                                  */
/********************************************************************/

/*******************************************************/
/* Funkcja inicjuje strukture wskazywana przez wybor   */
/* PRE:                                                */
/*      poprawnie zainicjowany argument wybor (!=NULL) */
/* POST:                                               */
/*      "wyzerowana" struktura wybor wybranych opcji   */
/*******************************************************/

/************************************************************************/
/* Funkcja rozpoznaje opcje wywolania programu zapisane w tablicy argv  */
/* i zapisuje je w strukturze wybor                                     */
/* Skladnia opcji wywolania programu                                    */
/*         program {[-i nazwa] [-o nazwa] [-p liczba] [-n] [-r] [-d] }  */
/* Argumenty funkcji:                                                   */
/*         argc  -  liczba argumentow wywolania wraz z nazwa programu   */
/*         argv  -  tablica argumentow wywolania                        */
/*         wybor -  struktura z informacjami o wywolanych opcjach       */
/* PRE:                                                                 */
/*      brak                                                            */
/* POST:                                                                */
/*      funkcja otwiera odpowiednie pliki, zwraca uchwyty do nich       */
/*      w strukturze wyb�r, do tego ustawia na 1 pola dla opcji, ktore  */
/*	poprawnie wystapily w linii wywolania programu,                     */
/*	pola opcji nie wystepujacych w wywolaniu ustawione sa na 0;         */
/*	zwraca wartosc W_OK (0), gdy wywolanie bylo poprawne                */
/*	lub kod bledu zdefiniowany stalymi B_* (<0)                         */
/* UWAGA:                                                               */
/*      funkcja nie sprawdza istnienia i praw dostepu do plikow         */
/*      w takich przypadkach zwracane uchwyty maja wartosc NULL         */
/************************************************************************/

int przetwarzaj_opcje(int argc, char **argv, str_pomoc *pomoc, str_obrazu *obraz) {


  pomoc->plik_we=NULL;        //wyzerowanie opcji
  pomoc->plik_wy=NULL;
  pomoc->negatyw=0;
  pomoc->konturowanie=0;
  pomoc->wyswietlenie=0;
  pomoc->rozmycie=0;



  int i;
  char *nazwa_we;

  pomoc->plik_wy=stdout;        /* na wypadek gdy nie podano opcji "-o" */

  for (i=1; i<argc; i++) {
    if (argv[i][0] != '-') { /* blad: to nie jest opcja - brak znaku "-" */
      return B_NIEPOPRAWNAOPCJA;
    }
    switch (argv[i][1]) {
    case 'i': {                 /* opcja z nazwa pliku wejsciowego */
      if (++i<argc) {   /* wczytujemy kolejny argument jako nazwe pliku */
	      nazwa_we=argv[i];
	      if (strcmp(nazwa_we,"-")==0) { /* gdy nazwa jest "-"        */
          fscanf(stdin,"%s",nazwa_we);         //czytamy wpisana przez usera nazwe
          pomoc->plik_we=fopen(nazwa_we,"r");
        } else {                              /* otwieramy wskazany plik   */
	        pomoc->plik_we=fopen(nazwa_we,"r");
        }
      } else {
	      return B_BRAKNAZWY;                   /* blad: brak nazwy pliku */
      }
      break;
    }
    case 'o': {                 /* opcja z nazwa pliku wyjsciowego */
      if (++i<argc) {   /* wczytujemy kolejny argument jako nazwe pliku */
	      pomoc->nazwa_nowy=argv[i];
	      if (strcmp(pomoc->nazwa_nowy,"-")==0) {       /* gdy nazwa jest "-"         */
          pomoc->tmp=1;
	        pomoc->plik_wy=stdout;          /* ustwiamy wyjscie na stdout */
        } else {                             /* otwieramy wskazany plik    */
	        pomoc->plik_wy=fopen(pomoc->nazwa_nowy,"w");
        }
      } else {
	      return B_BRAKNAZWY;                   /* blad: brak nazwy pliku */
      }
      break;
    }
    case 'n': {                 /* mamy wykonac negatyw */
      pomoc->negatyw=1;
      break;
    }
    case 'r': {                 /* mamy wykonac rozmycie */
      pomoc->rozmycie=1;
      break;
    }
    case 'k': {                 /* mamy wykonac konturowanie */
      pomoc->konturowanie=1;
      break;
    }
    case 'd': {                 /* mamy wyswietlic obraz */
      pomoc->wyswietlenie=1;
      break;
    }
    case 'm': {                 //wybor koloru do przetwarzania
      if(++i<argc) {
        switch (*argv[i]) {
            case 'r': {           //wybieramy czerwony
              pomoc->r=1;
              break;
            }
            case 'g': {           //wybieramy zielony
              pomoc->g=1;
              break;
            }
            case 'b': {           //wybieramy niebieski
              pomoc->b=1;
              break;
              }
          case 's': {           //wybieramy kowersje do szarosci
            pomoc->mono=1;
            break;
          }
          default:              //podanie niepoprawnej opcji zwraca blad
            return B_NIEPOPRAWNAOPCJA;
        }
      }
      break;                    //koniec switcha
    }
    default:                    //niepoprawna opcja 
      return B_NIEPOPRAWNAOPCJA;
    } /* koniec switch */
  } /* koniec for */

  if (pomoc->plik_we!=NULL)     /* ok: wej. strumien danych zainicjowany */
    return W_OK;
  else 
    return B_BRAKPLIKU;         /* blad:  nie otwarto pliku wejsciowego  */
}
