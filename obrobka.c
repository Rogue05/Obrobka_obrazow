#include "obrobka.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define MAX 512            /* Maksymalny rozmiar wczytywanego obrazu */
#define DL_LINII 1024      /* Dlugosc buforow pomocniczych */

/* zbior danych wykorzystywanych przy korzystaniu z plikow */
//static FILE *plik;
static int **obraz_pgm;
static int *wymx, *wymy, *szarosci;
//static char nazwa[100];

int O_getObraz(O_Obraz *obraz){
  obraz->obraz_pgm = obraz_pgm;
  obraz->wymx = *wymx;
  obraz->wymy = *wymy;
  obraz->szarosci = *szarosci;
  return 1;
}

int O_setObraz(O_Obraz *obraz){
  wymx = &(obraz->wymx);
  wymy = &(obraz->wymy);
  szarosci = &(obraz->szarosci);
  obraz_pgm = obraz->obraz_pgm;
  return 1;
}

O_Obraz O_makeObraz(int **obraz_pgm, int *wymx, int *wymy, int *szarosci){
  O_Obraz init;
  init.wymx = (*wymx);
  init.wymy = (*wymy);
  init.szarosci = (*szarosci);
  init.obraz_pgm = obraz_pgm;
  return init;
}

void O_freeObraz(O_Obraz *O_Obraz){

return;
}

void myCLS(void){
    printf("\033[2J");
    printf("\033[0;0f");
}

int zapisz(char *nazwa, O_Obraz *obraz) {
  //char buf[DL_LINII];      /* bufor pomocniczy do czytania naglowka i komentarzy */
  //int znak;                /* zmienna pomocnicza do czytania komentarzy */
  //int koniec=0;            /* czy napotkano koniec danych w pliku */
  int i,j;
  FILE *plik_wy;
  plik_wy = fopen(nazwa, "w");
  //  printf("zaczynam zapis %p\n", plik_wy);
  
  O_setObraz(obraz);
  //printf("ustawilem obraz %p\n", obraz_pgm);

  fprintf(plik_wy,"P2\n");

  //printf("jest p2 %p\n", obraz_pgm);
  
  fprintf(plik_wy,"%d %d\n%d\n", *wymx, *wymy, *szarosci);

  //printf("Wpisalem 3 syfy\n");
  
  for(i=0;i<*wymy;i++){
      for(j=0;j<*wymx;j++){
	  fprintf(plik_wy,"%d ",obraz_pgm[i][j]); 
	  if(j%70==0) fprintf(plik_wy, "\n");
      } /* for j */
      fprintf(plik_wy,"\n");
  }/* for i */

  fclose(plik_wy);
  
  return 1;  
}

int czytaj(FILE *plik_we, O_Obraz *obraz) {
  assert(plik_we != NULL && obraz != NULL);


  char buf[DL_LINII];      /* bufor pomocniczy do czytania naglowka i komentarzy */
  int znak;                /* zmienna pomocnicza do czytania komentarzy */
  int koniec=0;            /* czy napotkano koniec danych w pliku */
  int i,j;
  
  O_setObraz(obraz);
  
  /*Sprawdzenie czy podano prawidłowy uchwyt pliku */
  if (plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
  }
  
  /* Sprawdzenie "numeru magicznego" - powinien być P2 */
  if (fgets(buf,DL_LINII,plik_we)==NULL)   /* Wczytanie pierwszej linii pliku do bufora */
    koniec=1;                              /* Nie udalo sie? Koniec danych! */
  
  if ( (buf[0]!='P') || (buf[1]!='2') || koniec) {  /* Czy jest magiczne "P2"? */
    fprintf(stderr,"Blad: To nie jest plik PGM\n");
    return(0);
  }
  
  /* Zapisanie komentarzy */
  //int currentLine = 0;
  do {
    if ((znak=fgetc(plik_we))=='#') {         /* Czy linia rozpoczyna sie od znaku '#'? */
      if (fgets( buf,DL_LINII,plik_we)==NULL)  /* Przeczytaj ja do bufora                */
	koniec=1;                   /* Zapamietaj ewentualny koniec danych */
    } 
    else {
      ungetc(znak,plik_we);                   /* Gdy przeczytany znak z poczatku linii */
    }                                         /* nie jest '#' zwroc go                 */
  } while (znak=='#' && !koniec);   /* Powtarzaj dopoki sa linie komentarza */
  /* i nie nastapil koniec danych         */
  //printf("%s\n", (*myLog).previousLog[0]);
  /* Pobranie wymiarow obrazu i liczby odcieni szarosci */
  if (fscanf(plik_we,"%d %d %d",wymx,wymy,szarosci)!=3) {
    fprintf(stderr,"Blad: Brak wymiarow obrazu lub liczby stopni szarosci\n");
    return(0);
  }

  /*alokuje pamiec pod tablice*/
  obraz_pgm = (int**)malloc(sizeof(int*)*(*wymy)+1);
  for(i=0;i<(*wymy);i++) obraz_pgm[i] = (int*)malloc(sizeof(int)*(*wymx+1));
  
  /* Pobranie obrazu i zapisanie w tablicy obraz_pgm*/
  for (i=0;i<*wymy;i++) {
    for (j=0;j<*wymx;j++) {
      if (fscanf(plik_we,"%d",&(obraz_pgm[i][j]))!=1) {
	fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");
	return(0);
      }
    }
  }
  
  O_getObraz(obraz); /* zapisuje obraz */
  
  return *wymx**wymy;   /* Czytanie zakonczone sukcesem    */
}                       /* Zwroc liczbe wczytanych pikseli */

void wyswietl(O_Obraz *obraz, char *n_pliku) {
  assert(n_pliku != NULL && obraz != NULL );
  zapisz(n_pliku, obraz);
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,n_pliku);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}

void negatyw(O_Obraz *obraz){
  assert(obraz != NULL);
  O_setObraz(obraz);
  
  int i, j;    
  for(i=0;i<*wymy;i++){
    for(j=0;j<*wymx;j++)
      obraz_pgm[i][j]=*szarosci-obraz_pgm[i][j];}
  O_getObraz(obraz);
  printf("Jest negatyw\n"); 
  return;
}

void Progowanie(O_Obraz *obraz, float prog){
  assert(obraz != NULL && 0<=prog && prog<=100);
  O_setObraz(obraz);
  int i, j;
  int max_szarosci = *szarosci;
  
  prog = prog * 0.01 * (float)(*szarosci);
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      ( (obraz_pgm[i][j] <= prog) ? (obraz_pgm[i][j] = 0) : (obraz_pgm[i][j] = max_szarosci) );
  
  printf("Zakonczono progowanie\n");
  O_getObraz(obraz);
  return;
}


void Polprogowanie_czerni(O_Obraz *obraz, float prog){
  assert(obraz != NULL && 0<=prog && prog<=100);
  O_setObraz(obraz);
  int i, j;
  //int max_szarosci = *szarosci;
  prog = prog * 0.01 * (float)(*szarosci);
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      ( (obraz_pgm[i][j] <= prog) ? (obraz_pgm[i][j] = 0) : 0 );
  
  printf("Zakonczono polrogowanie czerni\n");
  O_getObraz(obraz);
  return;
}

void Polprogowanie_bieli(O_Obraz *obraz, float prog){
  assert(obraz != NULL && 0<=prog && prog<=100);
  O_setObraz(obraz);
  int i, j;
  int max_szarosci = *szarosci;
  
  prog = prog * 0.01 * (float)(*szarosci);
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      ( (obraz_pgm[i][j] <= prog) ? 0 : (obraz_pgm[i][j] = max_szarosci) );
  
  printf("Zakonczono polrogowanie bieli\n");
  O_getObraz(obraz);
  return;
}

void Korekcja_gamma(O_Obraz *obraz, float gamma){
  assert(obraz != NULL);
  O_setObraz(obraz);
  int i, j;
  int max_szarosci = *szarosci;
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      obraz_pgm[i][j] = pow( (double)obraz_pgm[i][j] / (double)max_szarosci, 1 / gamma ) * max_szarosci;
  
  printf("Zakonczono korekcje gamma\n");
  O_getObraz(obraz);
  return;
}

void Zmiana_poziomow(O_Obraz *obraz, float czern, float biel){
  assert(obraz != NULL && czern<=100 && czern>=0 && 0<=biel && biel<=100);
  O_setObraz(obraz);
  int i, j;
  int max_szarosci = *szarosci;
  
  czern = 0.01 * czern * (*szarosci);
  biel = 0.01 * biel * (*szarosci);
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++){
      if(czern>=obraz_pgm[i][j]) obraz_pgm[i][j] = 0;
      else if(czern<obraz_pgm[i][j] && obraz_pgm[i][j]<biel) 
	obraz_pgm[i][j] = ((double)obraz_pgm[i][j]-czern)*((double)max_szarosci)/(biel-czern);
      else if(obraz_pgm[i][j]>=biel) obraz_pgm[i][j] = max_szarosci;
    }
  printf("Zakonczono zmiane poziomow\n");
  O_getObraz(obraz);
  return;
}

float absf(float a){
    return a>0 ? a : -a;
}

void Konturowanie(O_Obraz *obraz){
  assert(obraz != NULL);
  O_setObraz(obraz);
  int i, j;
  //int max_szarosci = *szarosci;
  int tmp_pgm[MAX][MAX];
  
  for(i=1;i<*wymy-1;i++)
    for(j=1;j<*wymx-1;j++)
      tmp_pgm[i][j] = absf( obraz_pgm[i+1][j]-obraz_pgm[i][j] ) + absf( obraz_pgm[i][j+1]-obraz_pgm[i][j] );
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      obraz_pgm[i][j] = tmp_pgm[i][j];
  
  printf("Zakonczono konturowanie\n");
  O_getObraz(obraz);
  return;
}

void Rozmycie_poziome(O_Obraz *obraz){
  assert(obraz != NULL);
  O_setObraz(obraz);
  int i, j;
  //int max_szarosci = *szarosci;
  int tmp_pgm[MAX][MAX];
  
  for(i=1;i<*wymy-1;i++)
    for(j=1;j<*wymx-1;j++)
      tmp_pgm[i][j] = ((double)obraz_pgm[i][j-1] + (double)obraz_pgm[i][j] + (double)obraz_pgm[i][j+1] )/3 ;
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      obraz_pgm[i][j] = tmp_pgm[i][j];
  
  printf("Zakonczono rozmycie poziome\n");
  O_getObraz(obraz);
  return;
}

void Rozciagniecie_histogramu(O_Obraz *obraz){
  assert(obraz != NULL);
  O_setObraz(obraz);  
  int my_max=0;
  int my_min=1000000000;
  int i, j;
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++){
      if(obraz_pgm[i][j] > my_max) my_max = obraz_pgm[i][j];
      else if(obraz_pgm[i][j] < my_min) my_min = obraz_pgm[i][j];
    }
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      obraz_pgm[i][j] = ((double)obraz_pgm[i][j] - my_min) * ((double)(*szarosci))/(my_max-my_min);
  
  printf("Zakonczono rozciagniecie histogramu\n");
  O_getObraz(obraz);
  return;
}

void Urzyj_filtru(O_Obraz *obraz, int *filtr){
  assert(obraz != NULL && filtr != NULL);
  
  O_setObraz(obraz);

  int tmp_pgm[MAX][MAX];
  int i, j;
  /*int suma_wag = filtr[0] + filtr[1] + filtr[2] + 
    filtr[3] + filtr[4] + filtr[5] + 
    filtr[6] + filtr[7] + filtr[8];
    if(!suma_wag) suma_wag=1;*/
  /*
    for(i=0;i<9;i++) printf("%d ", *(filtr+i));
    printf("\n%d %d\n", *wymx, *wymy);
  */
  for(i=1;i<*wymy-1;i++)
    for(j=1;j<*wymx-1;j++)
      tmp_pgm[i][j] = ( filtr[0] * obraz_pgm[i-1][j-1] + filtr[1] * obraz_pgm[i][j-1] + filtr[2] * obraz_pgm[i+1][j-1] + 
			filtr[3] * obraz_pgm[i-1][j] + filtr[4] * obraz_pgm[i][j] + filtr[5] * obraz_pgm[i+1][j] +
			filtr[6] * obraz_pgm[i-1][j+1] + filtr[7] * obraz_pgm[i][j+1] + filtr[8] * obraz_pgm[i+1][j+1] ); /* suma_wag*/ 
  
  for(i=0;i<*wymy;i++)
    for(j=0;j<*wymx;j++)
      obraz_pgm[i][j] = tmp_pgm[i][j];
  
  printf("Zakonczono filtrowanie\n");
  O_getObraz(obraz);
  return;
}

O_Obraz Dodaj_obrazy(O_Obraz *obraz1 ,O_Obraz *obraz2){
  O_Obraz wynik;
  int i, j;
  
  wynik.wymx = MY_MAX(obraz1->wymx, obraz2->wymy);
  wynik.wymy = MY_MAX(obraz1->wymy, obraz2->wymy);
  wynik.szarosci = obraz1->szarosci + obraz2->szarosci;
  
  wynik.obraz_pgm = (int**)malloc(sizeof(int*)*(wynik.wymy)+1);
  for(i=0;i<(wynik.wymy);i++) wynik.obraz_pgm[i] = (int*)malloc(sizeof(int)*(wynik.wymx+1));

  for(i=0;i<obraz1->wymy;i++)
    for(j=0;j<obraz1->wymx;j++)
      wynik.obraz_pgm[i][j] = obraz1->obraz_pgm[i][j];

  for(i=0;i<obraz2->wymy;i++)
    for(j=0;j<obraz2->wymx;j++)
      wynik.obraz_pgm[i][j] += obraz2->obraz_pgm[i][j];
  
  return wynik;
}
