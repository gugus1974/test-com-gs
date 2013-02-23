/****************************************************************/
/* 27.feb.2001       v.2.02      \SWISV\EMUC\A\CCU\SRC\Util.c   */
/****************************************************************/
/* Funzioni di uso comune su CCU Emu Norvegia                   */
/****************************************************************/
/*
    $Date: 2007/09/17 13:05:53 $                               $Revision: 1.5 $
    $Author: schiavor $
*/

#include "stdio.h"
#include "string.h"     /* gestione stringhe */

#include "util.h"



/*********************************/
/* Define di Modulo              */
/*********************************/


/******************************/
/* variabili Globali di Modulo*/
/******************************/
/*********************************/
/* Extern Variabili              */
/*********************************/


/*********************************/
/* Prototipi Funzioni di Modulo  */
/*********************************/

//static short gest_sim_temp(short argc, char* argv[]);

/*******************************************************/
/*    Comando e Controllo                              */
/*******************************************************/
/*******************************************************/
/*    Test a tempo Condizione vera per time            */
/*******************************************************/








/**************************************************************************/
/*                                                                        */
/* -> get_char_bit(x,y)   : estrae il bit in pos. "y" di char "x"         */
/* -> get_int_bit(x,y)    : estrae il bit in pos. "y" di int "x"          */
/* -> get_long_bit(x,y)   : estrae il bit in pos. "y" di long "x"         */
/*                                                                        */
/* -> set_char_bit(x,y,z) : imposta a "z" il bit in pos. "y" di char "x"  */
/* -> set_int_bit(x,y,z)  : imposta a "z" il bit in pos. "y" di int "x"   */
/* -> set_long_bit(x,y,z) : imposta a "z" il bit in pos. "y" di long "x"  */
/*                                                                        */
/* -> print_char_bin(x)   : stampa a video in binario char "x"            */
/* -> print_int_bin(x)    : stampa a video in binario int "x"             */
/* -> print_long_bin(x)   : stampa a video in binario long "x"            */
/*                                                                        */
/* -> char_to_int(x1,x2)  : trasforma in int i due char "x1" e "x2"       */
/* -> int_to_char(x,y)    : estrae il char in posizione "y" da int "x"    */
/*                                                                        */
/* -> diff_mod_char(x,y)  : genera differenza in modulo tra "x" e "y"     */
/* -> diff_mod_int(x,y)   : genera differenza in modulo tra "x" e "y"     */
/*                                                                        */
/**************************************************************************/



unsigned char get_char_bit(unsigned char *variabile, char posizione)
{
    int tmp;

    if (posizione<8) tmp=(*variabile>>posizione) & 0x01;
    else tmp=9;

    return (tmp);
}

/*
unsigned char get_int_bit(unsigned int *variabile, char posizione)
{
    int tmp;

    if (posizione<16) tmp=(*variabile>>posizione) & 0x01;
    else tmp=9;

    return (tmp);
}


unsigned char get_long_bit(unsigned long *variabile, char posizione)
{
    int tmp;

    if (posizione<32) tmp=(*variabile>>posizione) & 0x01;
    else tmp=9;

    return (tmp);
}



unsigned char set_char_bit(unsigned char *variabile, unsigned char posizione, unsigned char valore)
{
    char tmp=0;

    if ((posizione<8)&&(valore<2))
      *variabile=(valore<<posizione) | (*variabile & ~(0x01<<posizione));
    else
      tmp=9;

    return (tmp);
}



unsigned char set_int_bit(unsigned int *variabile, unsigned char posizione, unsigned char valore)
{
    char tmp=0;

    if ((posizione<16)&&(valore<2))
      *variabile=(valore<<posizione) | (*variabile & ~(0x01<<posizione));
    else
      tmp=9;

    return (tmp);
}


unsigned char set_long_bit(unsigned long *variabile, unsigned char posizione, unsigned char valore)
{
    char tmp=0;

    if ((posizione<16)&&(valore<2))
      *variabile=(valore<<posizione) | (*variabile & ~(0x01<<posizione));
    else
      tmp=9;

    return (tmp);
}
*/
void print_char_bin(unsigned char valore)
{
    char i;

    for (i=7; i>=0; i--)
    {
      if (i==3) printf(" ");
      printf("%01bd",get_char_bit(&valore,i));
    }
}

/*
void print_int_bin(unsigned int valore)
{
    char i;

    for (i=15; i>=0; i--)
    {
      if ((i==3)||(i==7)||(i==11)) printf(" ");
      printf("%d",get_int_bit(&valore,i));
    }
}


void print_long_bin(unsigned long valore)
{
    char i;

    for (i=31; i>=0; i--)
    {
      if ((i==3)||(i==7)||(i==11)||(i==15)||(i==19)||(i==23)||(i==27)) printf(" ");
      printf("%d",get_long_bit(&valore,i));
    }
}
*/