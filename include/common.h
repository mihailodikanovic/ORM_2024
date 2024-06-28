/*
	AUTORI:     Dikanovic Mihailo RA 84/2021
		        Djurdjevic Aleksa RA 85/2021
		

	KURS:       Osnovi racunarskih mreza I
		
		
	POSLEDNJE   
    IZMENE:		14.1.2024
*/
#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define B2 2
#define B4 4
#define B8 8
#define B16 16
#define B24 24
#define B32 32
#define B48 48

#define DEFAULT_PORT   27018
#define DEFAULT_BUFLEN 512

#define aio "Select an option ->\n1) List all modules\n2) List all analog modules\n3) List all digital modules\n4) Search  module (by name)\n5) Change module\n6) Exit\n"
#define displayFrame "\n---------------\n"

typedef struct searchResult
{
    char analogOrDigital;
    int index;
    
}searchResult;

typedef struct analogModule
{
    char name[32];
    short value;
}ANALOG;

typedef struct digitalModule
{
    char name[32];
    bool state;
}DIGITAL;

#endif