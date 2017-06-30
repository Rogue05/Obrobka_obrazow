/* Wojciech Sopot 
235291
Przetwarzanie obrazow final

testy:
./test -n -d
otrzymojemy negatyw Leny.pgm
diała: urzywanie domyslnych danych

./test -k -d -i kubus3.pgm
otrzymujemy kontury kubusa 3
dziala: dawanie wlasnego inputu

./test -k -d -o wynik.pgm
to co wrzej ale zapisane w wynik.pgm
dziala: narzucanie outputu

./test
-n
-d
#
wpisac: Ctrl + z
wynik: negatyw leny
zatem bash działa

program jest w stanie wykonać operacje na 2 zdjeciach OSOBNO:
./test -n -d -- -k -d

 */


#include <stdio.h>
#include <assert.h>
#include "obrobka.h"

#define W_OK 0                   /* wartosc oznaczajaca brak bledow */
#define B_NIEPOPRAWNAOPCJA -1    /* kody bledow rozpoznawania opcji */
#define B_BRAKNAZWY   -2
#define B_BRAKWARTOSCI  -3
#define B_BRAKPLIKU   -4

/*
dostepne opcje:
i nazwain domyslnie Lena.pgm
-i <nazwa>
o nazwout domyslnie tmp.pgm
-o <nazwa>
p progowanie 
-p <prog>
n negatyw
-n
k konturowanie
-k
d wyswietlanie
-d
g gamma
-g <gamma>
z zmiana poziomow
-z <biel> <czern>
x rozm poziome
-x
h histogram
-h
s maska
-s s1 s2 s3 s4 s5 s6 s7 s8 s9
układ:
s1 s2 s3 
s4 s5 s6 
s7 s8 s9

- kolejny plik
<opcje> -- <opcje>

*/

int przetwarzaj_opcje(int argc, char **argv, w_opcje *wybor, int start);
int wykonaj_opcje(w_opcje *wybor, O_Obraz *obraz);
void wyzeruj_opcje(w_opcje *wybor);
void showhelp();

int main(int argc, char **argv) {

  //if(*argv[argc-1]=='#') --argc;
  
  int i;
  int odczytano = 0;
  FILE *plik = NULL;

  O_Obraz *obraz = NULL;
  w_opcje *wybor = NULL;

  O_Obraz *lista_obrazow[100]; /* 100 to maksymalna liczba obrazow */
  for(i=0;i<100;i++) lista_obrazow[i] = NULL;
  int pozycja_w_liscie=0, pozycja_w_opcjach=0;

  
  i=0;
  if(argc==1){
    printf("Po zakonczeniu nalezy wcisnac Ctrl + z\nAby wukonac polecenia nalezy wpisac #\n");
    char buf[10000]; /*1000 to maks. dl. polecen*/
    char polecenie[10000];
    while(1){
      printf(">> ");
      scanf("%s", &buf[i]);
      if(buf[i]=='#') break;
      while(buf[i] != '\0') i++;
      buf[i++]=' ';
      
    }
    
    strcpy(polecenie,"./test "); 
    strcat(polecenie, buf);
    printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
    system(polecenie);             /* wykonanie polecenia        */
  }

  while(pozycja_w_opcjach < argc){
    if(argc >=2){
      if(argv[1][pozycja_w_opcjach] == '-' ){

	lista_obrazow[pozycja_w_liscie] = (O_Obraz*)malloc(sizeof(O_Obraz));

	++pozycja_w_opcjach;
	
	obraz = lista_obrazow[pozycja_w_liscie];
	wybor = &(obraz->opcje);

	++pozycja_w_liscie;
	
	wyzeruj_opcje(wybor);
	
	wybor->nazwain = "Lena.pgm";
	wybor->nazwaout = "tmp.pgm";

	pozycja_w_opcjach = przetwarzaj_opcje(argc, argv, wybor, pozycja_w_opcjach);
	plik = fopen(wybor->nazwain, "r");
	czytaj(plik, obraz);
	fclose(plik);
	wykonaj_opcje(wybor, obraz);
	zapisz(wybor->nazwaout, obraz);
	
	if(wybor->wyswietlenie==1) wyswietl(obraz, wybor->nazwaout);
      }
    }
  }
  return odczytano;
}

int przetwarzaj_opcje(int argc, char **argv, w_opcje *wybor, int start) {
  int i, j;
  if(argc==2 && argv[start][0]=='h'&& argv[start][1]=='e'&& argv[start][2]=='l'&& argv[start][3]=='p') showhelp();

  for (i=start; i<argc; i++) {
    if (argv[i][0] != '-')  /* blad: to nie jest opcja - brak znaku "-" */
      return B_NIEPOPRAWNAOPCJA;

    switch (argv[i][1]) {
    case 'i': {                 /* opcja z nazwa pliku wejsciowego */
      if (++i<argc) {   /* wczytujemy kolejny argument jako nazwe pliku */
	wybor->nazwain=argv[i];
	break;
      }
    }
    case 'o': {                 /* opcja z nazwa pliku wyjsciowego */
      if (++i<argc)   /* wczytujemy kolejny argument jako nazwe pliku */
	wybor->nazwaout=argv[i];
      break;
    }
      
    case 'p': {
      if (++i<argc) { /* wczytujemy kolejny argument jako wartosc progu */
	if (sscanf(argv[i],"%d",&(wybor->prog))==1) {
	  wybor->progowanie=1;
	} else
	  return B_BRAKWARTOSCI;     /* blad: niepoprawna wartosc progu */
      } else 
	return B_BRAKWARTOSCI;             /* blad: brak wartosci progu */
      break;
    }
      
    case 'n': {                 /* mamy wykonac negatyw */
      wybor->negatyw=1;
      printf("nadal ok\n");
      break;
    }
      
    case 'k': {                 /* mamy wykonac konturowanie */
      wybor->konturowanie=1;
      break;
    }

    case 'd': {                 /* mamy wyswietlic obraz */
      wybor->wyswietlenie=1;
      break;
    }
      /* mamy zrobic korekcje gamma */
    case 'g': {
      wybor->korgamma = 1;
      break;
    }
      /* mamy zrobic zmiane poziomow */
    case 'z': {
      wybor->zmpoz = 1;
      if(i+2<argc){
	sscanf(argv[++i],"%d",&(wybor->czern));
	sscanf(argv[++i],"%d",&(wybor->biel));
      }
      else return B_BRAKWARTOSCI;
    }
      /* mamy zrobic rozmycie poziome */
    case 'x': {
      wybor->rozmpoz = 1;
      break;
    }
      /* mamy zrobic rozciagniecie histogramu */
    case 'h': {
      wybor->hist = 1;
      break;
    }
      
    case 's': {
      wybor->filtr = 1; //biore tab
      if(!(i+9<argc)) return B_BRAKWARTOSCI;
      for(++i, j=0;j<9;++i, ++j) sscanf(argv[i],"%d",wybor->maska+j);
      --i;
      break;
    }

    case '-':{
      return i;
    }
      /*dodaj dwa obrazy*/
    case 'a':{

      break;
    }
      
    default:{
      return B_NIEPOPRAWNAOPCJA;
    }
    } /*koniec switch */
  } /* koniec for */
  return i;
}

int wykonaj_opcje(w_opcje *wybor, O_Obraz *obraz){
  printf("zaczolem wykonanie\n");
  if(wybor->negatyw) negatyw(obraz);
  if(wybor->progowanie) Progowanie(obraz, wybor->prog);
  if(wybor->konturowanie) Konturowanie(obraz);
  if(wybor->polprogc) Polprogowanie_czerni(obraz, wybor->progc);
  if(wybor->polprogb) Polprogowanie_bieli(obraz, wybor->progb);
  if(wybor->korgamma) Korekcja_gamma(obraz, wybor->gamma);
  if(wybor->zmpoz) Zmiana_poziomow(obraz, wybor->czern, wybor->biel);
  if(wybor->rozmpoz) Rozmycie_poziome(obraz);
  if(wybor->hist) Rozciagniecie_histogramu(obraz);
  if(wybor->filtr) Urzyj_filtru(obraz, wybor->maska);
  return 1;
}
    
void showhelp(){
  printf("dane liczbowe podajemy w punktach procetowych\ndostepne opcje: \ni nazwain domyslnie Lena.pgm \n-i <nazwa> \no nazwout domyslnie tmp.pgm \n-o <nazwa> \np progowanie  \n-p <prog> \nn negatyw \n-n \nk konturowanie \n-k \nd wyswietlanie \n-d \ng gamma \n-g <gamma> \nz zmiana poziomow \n-z <biel> <czern> \nx rozm poziome \n-x \nh histogram \n-h \ns maska \n-s s1 s2 s3 s4 s5 s6 s7 s8 s9 \nukład: \ns1 s2 s3  \ns4 s5 s6  \ns7 s8 s9 \n");
}

void wyzeruj_opcje(w_opcje *wybor) {
  wybor->negatyw=0;
  wybor->konturowanie=0;
  wybor->progowanie=0;
  wybor->wyswietlenie=0;
  wybor->polprogc=0;
  wybor->polprogb=0;
  wybor->korgamma=0;
  wybor->zmpoz=0;
  wybor->rozmpoz=0;
  wybor->hist=0;
  wybor->filtr=0;
  wybor->nazwain=NULL;
  wybor->nazwaout=NULL;
}
