/*
	AUTORI:     Dikanovic Mihailo RA 84/2021
		        Djurdjevic Aleksa RA 85/2021
		

	KURS:       Osnovi racunarskih mreza I
		
		
	POSLEDNJE   
    IZMENE:		14.1.2024
*/
#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

#include "common.h"

//Checks if a string can be converted into a number.
int IsNumber(const char* string);

//Checks for valid input.
int CheckInput(char* input);

//Error handling during sending or receiving process.
void CheckSendRecv(int expression, const char* message);

/*
    CLIENT-SERVER DIALOGUE FUNCTIONS
*/

//Receives both analog and digital modules from the server.
void ReceiveAllModules(char* buffer, int sock);

//Receives analog modules from the server.
void ReceiveAnalogModules(char* buffer, int sock);

//Receives digital modules from the server.
void ReceiveDigitalModules(char* buffer, int sock);

//Reveices the module the client searches for by its name
void ReceiveModuleByName(char* buffer, int sock);

//Changes the value of the module the client searches for by its name
void ChangeModuleByName(char* buffer, int sock);

#endif