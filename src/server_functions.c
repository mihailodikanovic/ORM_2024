/*
	AUTORI:     Dikanovic Mihailo RA 84/2021
		        Djurdjevic Aleksa RA 85/2021
		

	KURS:       Osnovi racunarskih mreza I
		
		
	POSLEDNJE   
    IZMENE:		14.1.2024
*/

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include<sys/socket.h>

#include "server_functions.h"

short g_connectedClients;

short g_activeReadersAnalog = 0;
short g_waitingWritersAnalog = 0;
short g_activeReadersDigital = 0;
short g_waitingWritersDigital = 0;

pthread_mutex_t _analogMutex, _digitalMutex, m_connectedClients;
pthread_cond_t _analogRead, _analogWrite, _digitalRead, _digitalWrite;

ANALOG* analogModules[MAX_ANALOG_MODULES];
DIGITAL* digitalModules[MAX_DIGITAL_MODULES];

short totalAnalog;
short totalDigital;

void SendAllModules(char* buffer, int socket)
{
    SendAnalogModules(buffer, socket);
    SendDigitalModules(buffer, socket);
    SendEndFlag(socket);
}

void SendAnalogModules(char* buffer, int socket)
{
    int i;   
    AnalogReadLock();

    for(i = 0; i < totalAnalog; i++)
    {
        FormatAnalog(analogModules[i], buffer);
        CheckSendRecv(send(socket, buffer, DEFAULT_BUFLEN, 0), "Server failed to respond to client's \"LIST ALL ANALOG\" request.\n");
    }
    
    AnalogReadUnlock();
}

void SendDigitalModules(char* buffer, int socket)
{
    int i;
    DigitalReadLock();
    
    for(i = 0; i < totalDigital; i++)
    {
        FormatDigital(digitalModules[i], buffer);
        CheckSendRecv(send(socket, buffer, DEFAULT_BUFLEN, 0), "Server failed to respond to client's \"LIST ALL DIGITAL\" request.\n");
    }
    
    DigitalReadUnlock();
}

void SendModuleByName(char* buffer, int socket)
{
    char receive[B32];
    CheckSendRecv(recv(socket , receive , B32 , 0), "Server failed to receive a module name from a client.\n");
    searchResult result = FindModule(receive);

    if(result.index != -1)
    {   
        if(result.analogOrDigital == 'a')
            FormatAnalog(analogModules[result.index], buffer);
        else
            FormatDigital(digitalModules[result.index], buffer);

        
        CheckSendRecv(send(socket, buffer, DEFAULT_BUFLEN, 0), "Server failed to send search result information to a client.\n");
    }
    else
    {
        char msg[] = "Couldn't find module with that name.\n";
        CheckSendRecv(send(socket, msg, DEFAULT_BUFLEN, 0), "Server failed to inform a client that the search was unsuccessful.\n");
    }
}

void ChangeModuleByName(char* buffer, int socket)
{
    char receive[B32];

    CheckSendRecv(recv(socket , receive , B32 , 0), "Server failed to receive a module name from a client.");
    searchResult result = FindModule(receive);
    bzero(receive, B32);
    
    if(result.index == -1) //server couldn't find specified module
    {
        char message[] = "1\0"; //alerts the client not to wait for data as the module doesn't exist
        CheckSendRecv(send(socket, message, B2, 0), "Server failed to inform a client that the search was unsuccessful.\n");
    }
    else//server found the specified module
    {
        char message[] = "0\0";
        CheckSendRecv(send(socket, message, B2, 0), "Server failed to inform a client that the search was successful.\n");
        CheckSendRecv(recv(socket , receive , B16 , 0), "Server failed to receive a new value from a client.\n");
            
        int newValue = atoi(receive);
        bzero(receive, B32);
    
        if(result.analogOrDigital == 'a')
        {
            AnalogWriteLock();
            analogModules[result.index]->value = newValue;
            AnalogWriteUnlock();
        }
        else
        {
            DigitalWriteLock();
            digitalModules[result.index]->state = newValue > 0 ? true : false;
            DigitalWriteUnlock();
        }                   
    }
}

void SendEndFlag(int socket) //sends terminating flag "END" which alerts the client to stop receiving data
{
    char end[] = "END";
    CheckSendRecv(send(socket, end, DEFAULT_BUFLEN, 0), "Server failed to send the ERROR flag to a client.\n"); 
}

void FreeAnalogMemory(short amountOfModules) 
{
    short i;
    for(i = 0; i < amountOfModules; i++)
    {
        free(analogModules[i]);
    }
}

void FreeDigitalMemory(short amountOfModules)   
{
    short i;
    for(i = 0; i < amountOfModules; i++)
    {
        free(digitalModules[i]);
    }
}

void FormatAnalog(ANALOG* analogModule, char* destinationArray)
{
    char name[DEFAULT_BUFLEN];
    char value[DEFAULT_BUFLEN];
    
    sprintf(value, "%hd", analogModule->value);

    strcpy(name, analogModule->name);

    char displayData[DEFAULT_BUFLEN];
    strcpy(displayData, displayFrame);
    strcat(displayData, "Name: ");
    strcat(displayData, name);
    strcat(displayData, "\nValue: ");
    strcat(displayData, value);
    strcat(displayData, displayFrame);
    strcat(displayData, "\0");

    bzero(destinationArray, DEFAULT_BUFLEN);
    strcpy(destinationArray, displayData);   
}


void FormatDigital(DIGITAL* digitalModule, char* destinationArray) 
{
    char name[DEFAULT_BUFLEN];
    bool state;
    char charState[DEFAULT_BUFLEN];
    
    state = digitalModule->state;
    
    strcpy(charState, state == true ? STATE_1 : STATE_0);

    strcpy(name, digitalModule->name);

    char displayData[DEFAULT_BUFLEN];
    strcpy(displayData, displayFrame);
    strcat(displayData, "Name: ");
    strcat(displayData, name);
    strcat(displayData, "\nState: ");
    strcat(displayData, charState);
    strcat(displayData, displayFrame);
    strcat(displayData, "\0");

    bzero(destinationArray, DEFAULT_BUFLEN);
    strcpy(destinationArray, displayData); 
}

searchResult FindModule(char* buffer) 
{
    int i;
    searchResult result = {'a', -1};

    AnalogReadLock();

    for(i = 0; i < totalAnalog; i++)
    {
        if(CompareStrings(buffer, analogModules[i]->name))
        {
            result.index = i;
            AnalogReadUnlock();

            return result;          
        }       
    }

    AnalogReadUnlock();

    DigitalReadLock();

    for(i = 0; i < totalDigital; i++)
    {
        if(CompareStrings(buffer, digitalModules[i]->name))
        {
            result.analogOrDigital = 'd';
            result.index = i;
            DigitalReadUnlock();

            return result;
        }      
    }

    DigitalReadUnlock();

    return result;
}

bool CompareStrings(char* str1, char* str2) 
{
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    while(len1 > 0 && (str1[len1 - 1] == '\n' || str1[len1 - 1] == '\0'))
    {
        len1--;
    }

    while(len2 > 0 && (str2[len2 - 1] == '\n' || str2[len2 - 1] == '\0'))
    {
        len2--;
    }

    return (len1 == len2) && (strncmp(str1, str2, len1) == 0);
}

void IncrementCounter()
{
    pthread_mutex_lock(&m_connectedClients);
    g_connectedClients++;
    pthread_mutex_unlock(&m_connectedClients);
}

void DecrementCounter()
{
    pthread_mutex_lock(&m_connectedClients);
    g_connectedClients--;
    pthread_mutex_unlock(&m_connectedClients);
}

short ReadCounter()
{
    pthread_mutex_lock(&m_connectedClients);
    short value = g_connectedClients;
    pthread_mutex_unlock(&m_connectedClients);
    return value;
}

void AnalogReadLock()
{
    pthread_mutex_lock(&_analogMutex);
    while (g_waitingWritersAnalog > 0) 
    {
        pthread_cond_wait(&_analogRead, &_analogMutex);
    }
    g_activeReadersAnalog++;
    pthread_mutex_unlock(&_analogMutex);
}
void AnalogReadUnlock()
{
    pthread_mutex_lock(&_analogMutex);
    g_activeReadersAnalog--;
    
    if (g_activeReadersAnalog == 0 && g_waitingWritersAnalog > 0)
    {
        pthread_cond_signal(&_analogWrite);
    }
    pthread_mutex_unlock(&_analogMutex);    
}

void AnalogWriteLock()
{
    pthread_mutex_lock(&_analogMutex);
    g_waitingWritersAnalog++;
    while (g_activeReadersAnalog > 0)
    {
        pthread_cond_wait(&_analogWrite, &_analogMutex);
    }
    g_waitingWritersAnalog--;
}

void AnalogWriteUnlock()
{
    pthread_cond_broadcast(&_analogRead);
    pthread_mutex_unlock(&_analogMutex);
}

void DigitalReadLock()
{
    pthread_mutex_lock(&_digitalMutex);
    while (g_waitingWritersDigital > 0) 
    {
        pthread_cond_wait(&_digitalRead, &_digitalMutex);
    }
    g_activeReadersDigital++;
    pthread_mutex_unlock(&_digitalMutex);
}

void DigitalReadUnlock()
{
    pthread_mutex_lock(&_digitalMutex);
    g_activeReadersDigital--;
    
    if (g_activeReadersDigital == 0 && g_waitingWritersDigital > 0)
    {
        pthread_cond_signal(&_digitalWrite);
    }
    pthread_mutex_unlock(&_digitalMutex);
}

void DigitalWriteLock()
{
    pthread_mutex_lock(&_digitalMutex);
    g_waitingWritersDigital++;

    while (g_activeReadersDigital > 0)
    {
    pthread_cond_wait(&_digitalWrite, &_digitalMutex);
    }

    g_waitingWritersDigital--;
}

void DigitalWriteUnlock()
{
    pthread_cond_broadcast(&_digitalRead);
    pthread_mutex_unlock(&_digitalMutex);
}

void CheckSendRecv(int expression, const char* message)
{
    if(expression < 1)
    {
        perror(message);
        exit(-1);
    }
}