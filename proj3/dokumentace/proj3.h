/**
 * \file	proj3.h
 * \brief	Hlavičkový soubor pro práci s bludištěm (ak. rok 2018/19)
 * \author	Tomáš Milostný (xmilos02)
 * \date	2019-12-19
 * \mainpage <a href="proj3_8h.html">Dokumentace souboru proj3.h</a>
 */

#ifndef PROJ3_H
#define PROJ3_H

/**
 * \brief	Struktura mapy
 * 
 */
typedef struct {
    int rows; 				/**< Počet řádků matice mapy 	*/
    int cols;				/**< Počet sloupců matice mapy	*/
    unsigned char *cells; 	/**< Pole buněk mapy 			*/
} Map;

/**
 * \brief	Bitové váhy jednotlivých hranic
 * 
 */
enum borders
{
	BLEFT=0x1,	/**< Levá hranice	*/
	BRIGHT=0x2, /**< Pravá hranice	*/
	BTOP=0x4, 	/**< Horní hranice	*/
	BBOTTOM=0x4	/**< Spodní hranice	*/
};

/**
 * \brief		Dealokace mapy
 * \post		Úspěšně uvolněné místo v paměti
 * 
 * \param map	Ukazatel na strukturu mapy
 */
void free_map(Map *map);

/**
 * \brief			Načtení mapy ze souboru do struktury
 * \pre				Platný ukazatel na strukturu mapy
 * \pre				Existující neprázdný soubor k otevřeni
 * \post			Struktura mapy byla úspěšně načtena z daného souboru
 * 
 * \param filename 	Textový soubor mapy
 * \param map 		Ukazatel na strukturu mapy, do které načítáme soubor
 * \return 0		Ok
 * \return 1		Error
 */
int load_map(const char *filename, Map *map);

/**
 * \brief 			Mapovací funkce, která zjisťuje zda je daná hranice stěna
 * \pre				Platný ukazatel na strukturu mapy
 * \pre				Souřadnice v mapě jsou kladná čísla od 1
 * 
 * \param map 		Ukazatel na strukturu mapy
 * \param r,c		Souřadnice v mapě (řádek, sloupec)
 * \param border 	Hranice buňky bludiště ke kontrole (BLEFT, BRIGHT, BTOP, BBOTTOM)
 * \return true 	Hranice je stěna
 * \return false	Hranice je průchozí
 */
bool isborder(Map *map, int r, int c, int border);

/**
 * \brief			Zjišťuje, jestli buňka na dané souřadnici má spodní hranici
 * \pre				Souřadnice v mapě jsou kladná čísla od 1
 * 
 * \param r,c		Souřadnice v mapě (řádek, sloupec)
 * \return true 	Má spodní hranici
 * \return false 	Nemá spodní hranici
 */
bool hasbottom(int r, int c);

/**
 * \brief 				Funkce určující, která hranice se má po vstupu následovat (tj. která má být vedena rukou)
 * \pre					Platný ukazatel na strukturu mapy
 * \post				Nalezena hranice směrem do bludiště
 * 
 * \param map 			Ukazatel na strukturu mapy
 * \param r,c			Souřadnice vstupni buňky mapy (řádek, sloupec)
 * \param leftright 	Pravidlo levé nebo pravé ruky
 * \return int 			Hodnota hranice
 */
int start_border(Map *map, int r, int c, int leftright);

/**
 * \brief 		Zkontroluje zda se jedná o platnou mapu
 * \pre			Platný ukazatel na strukturu mapy
 * 
 * \param map 	Ukazatel na strukturu mapy
 * \return 0	Validní mapa
 * \return 1	Nevalidní mapa
 */
int check_map(Map *map);

/**
 * \brief 			Funkce, která načte a zkontroluje platnost mapy
 * \pre				Platný ukazatel na strukturu mapy
 * \pre				Existující neprázdný soubor k otevření
 * \post			Soubor obsahuje platná data pro strukturu mapy
 * 
 * \param filename 	Řetězec s názvem souboru s mapou
 * \param map 		Ukazatel na strukturu mapy, do které se načítají informace ze souboru
 * \return 0		Validní mapa
 * \return 1		Nevalidní mapa
 */
int load_and_check_map(const char *filename, Map *map);

/**
 * \brief			Jsme za hranicemi?
 * \pre				Platný ukazatel na strukturu mapy
 * 
 * \param map 		Ukazatel na strukturu mapy
 * \param r,c		Souřadnice v mapě (řádek, sloupec)
 * \return true 	Zadané souřadnice jsou mimo mapu
 * \return false	Zadané souřadnice jsou v mapě
 */
bool is_out(Map *map, int r, int c);

/**
 * \brief 				Vytiskne nalezenou cestu
 * \pre					Platný ukazatel na strukturu mapy
 * \pre					Platne souřadnice struktury mapy
 * 
 * \param map 			Ukazate na strukturu mapy
 * \param r,c			Souřadnice v mapě (řádek, sloupec)
 * \param leftright 	Pravidlo levé nebo pravé ruky
 */
void print_path(Map *map, int r, int c, int leftright);

#endif
