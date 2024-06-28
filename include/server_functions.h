/*
	AUTORI:     Dikanovic Mihailo RA 84/2021
		        Djurdjevic Aleksa RA 85/2021
		

	KURS:       Osnovi racunarskih mreza I
		
		
	POSLEDNJE   
    IZMENE:		14.1.2024
*/
#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "common.h"
#include <pthread.h>

//Maximum amount of clients allowed to connect to the server.
#define MAX_CLIENTS 128 

//Maximum amount of analog modules allowed to be created.
#define MAX_ANALOG_MODULES 512

//Maximum amount of digital modules allowed to be created. 
#define MAX_DIGITAL_MODULES 512

//This constant represents a "false" state when printing value of a digital module.
#define STATE_0 "off"
//This constant represents a "true" state when printing value of a digital module.
#define STATE_1 "on"

extern short g_connectedClients;

extern short g_activeReadersAnalog;
extern short g_waitingWritersAnalog;
extern short g_activeReadersDigital;
extern short g_waitingWritersDigital;

extern pthread_mutex_t _analogMutex, _digitalMutex, m_connectedClients;
extern pthread_cond_t _analogRead, _analogWrite, _digitalRead, _digitalWrite;

extern ANALOG* analogModules[MAX_ANALOG_MODULES];
extern DIGITAL* digitalModules[MAX_DIGITAL_MODULES];

extern short totalAnalog;
extern short totalDigital;

/*
    HELPING FUNCTIONS
*/

//Frees memory occupied by analog modules in case of bad memory allocation or insufficient space.
void FreeAnalogMemory(short amountOfModules);

//Frees memory occupied by digital modules in case of bad memory allocation or insufficient space.
void FreeDigitalMemory(short amountOfModules);

//Prepares a string for printing analog module information.
void FormatAnalog(ANALOG* analogModule, char* destinationArray);

//Prepares a string for printing digital module information.
void FormatDigital(DIGITAL* digitalModule, char* destinationArray);

//Attempts to find a module with specified name.
searchResult FindModule(char* buffer); 

//Compares two strings by removing excess '\n' and '\0' characters.
bool CompareStrings(char *str1, char *str2); 


/*
    GLOBAL CLIENT COUNTER FUNCTIONS THAT ENSURE THREAD SAFETY
*/

//Increments global client counter.
void IncrementCounter();

//Decrements global client counter.
void DecrementCounter();

//Returns the value of the global client counter.
short ReadCounter();


/*
    SERVER-CLIENT DIALOGUE FUNCTIONS
*/

//Displays all modules to a client.
void SendAllModules(char* buffer, int socket);

//Displays all analog modules to a client.
void SendAnalogModules(char* buffer, int socket);

//Displays all digital modules to a client.
void SendDigitalModules(char* buffer, int socket);

//Displays the module searched for by its name to a client.
void SendModuleByName(char* buffer, int socket);

//Changes the value of the module searched for by its name, specified by a client.
void ChangeModuleByName(char* buffer, int socket);

//Signals end of communication to the client.
void SendEndFlag(int socket);


/*
    THREAD SAFETY FUNCTIONS
*/

//Locks analog module read mutex.
void AnalogReadLock();

//Unlocks analog module read mutex.
void AnalogReadUnlock();

//Locks analog module write mutex.
void AnalogWriteLock();

//Unlocks analog module write mutex.
void AnalogWriteUnlock();

//Locks digital module read mutex.
void DigitalReadLock();

//Unlocks digital module read mutex.
void DigitalReadUnlock();

//Locks digital module write mutex.
void DigitalWriteLock();

//Unlocks digital module write mutex.
void DigitalWriteUnlock();

//Error handling during sending or receiving process.
void CheckSendRecv(int expression, const char* message);

#endif