#ifndef obrobka_h
#define obrobka_h

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define MAX 512            /* Maksymalny rozmiar wczytywanego obrazu */
#define DL_LINII 1024      /* Dlugosc buforow pomocniczych */

#define MY_MAX(x, y) (((x)>(y))?(x):(y))

typedef struct {
  int negatyw,
    progowanie,
    konturowanie,
    wyswietlenie,
    polprogc,
    polprogb,
    korgamma,
    zmpoz,
    rozmpoz,
    hist,    
    filtr;
  char *nazwain;
  char *nazwaout;
  int prog,
    progc,
    progb,
    gamma,
    czern,
    biel,
    maska[9];
} w_opcje;

/* struktura O_Obraz i jej destroktor i konstroktor */
typedef struct O_Obraz{
  int wymx;
  int wymy;
  int szarosci;
  int **obraz_pgm;
  w_opcje opcje;  
}O_Obraz;
O_Obraz O_makeObraz(int **obraz_pgm, int *wymx, int *wymy, int *szarosci);
void O_freeObraz(O_Obraz *O_Obrazd);

/* opcje menu */
#define M_Zczytaj 1
#define M_Zapisz 2
#define M_Wyswietl 3
#define M_Wyczysc_ekran 4
#define M_Koniec 5

#define     O_Progowanie 1
#define     O_Negatyw 2
#define     O_Polprogowanie_czerni 3 
#define     O_Polprogowanie_bieli 4
#define     O_Korekcja_gamma 5
#define     O_Zmiana_poziomow 6
#define     O_Konturowanie 7 
#define     O_Rozmycie_poziome 8
#define     O_Rozm_pionowe 9
#define     O_Rozciagniecie_histogramu 10
#define O_filtr 11

/* funkcje odczytu i zapisu */
int czytaj(FILE *plik_we, O_Obraz *obraz); 
int zapisz(char *nazwa, O_Obraz *obraz);

/* Funkcje graficzne */
void wyswietl(O_Obraz *obraz, char *n_pliku);
void negatyw(O_Obraz *obraz);
void Progowanie(O_Obraz *obraz, float prog);
void Polprogowanie_czerni(O_Obraz *obraz, float prog);
void Polprogowanie_bieli(O_Obraz *obraz, float prog);
void Korekcja_gamma(O_Obraz *obraz, float gamma);
void Zmiana_poziomow(O_Obraz *obraz, float czern, float biel);
void Konturowanie(O_Obraz *obraz);
void Rozmycie_poziome(O_Obraz *obraz);
void Rozciagniecie_histogramu(O_Obraz *obraz);
void Urzyj_filtru(O_Obraz *obraz, int *filtr);
void Dodoaj_obrazy(O_Obraz *obraz1, O_Obraz *obraz2); /* doda obrazy i zapisze w obraz1 */

float absf(float a); /* wymagana do funkcji Konturowanie bo abs z math nie dziala dla float*/
void myCLS(void);

#endif
