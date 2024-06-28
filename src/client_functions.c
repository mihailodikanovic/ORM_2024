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
#include <sys/socket.h>
#include <limits.h>

#include "client_functions.h"

int IsNumber(const char* string)
{
    char* end;
    strtol(string, &end, 10);

    return (*end == '\0' || *end == '\n');
}

int CheckInput(char* input)
{
    printf("> ");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
        
    if(IsNumber(input))
    {
        int selection = atoi(input);
        if(selection <= 6 && selection >= 1)
        {
            return selection;
        }
    }
    
    printf("Invalid input. Try again.\n");
    fflush(stdout);
    return -1;

}

void CheckSendRecv(int expression, const char* message)
{
    if(expression < 1)
    {
        perror(message);
        exit(-1);
    }
}

void ReceiveAllModules(char* buffer, int sock)
{
    while(1)
    {
        CheckSendRecv(recv(sock , buffer , DEFAULT_BUFLEN , 0), "Client unable to receive data from the server.\n");
    
        if(strncmp(buffer, "END", 3) == 0)
        {
            bzero(buffer, DEFAULT_BUFLEN);
            printf("Press enter to return to menu...\n");
            fflush(stdout);
            getchar();
            break;
        }
        
        printf("%s", buffer);
        fflush(stdout);
        bzero(buffer, DEFAULT_BUFLEN);       
    }
}

void ReceiveAnalogModules(char* buffer, int sock)
{
    while(1)
    {
        CheckSendRecv(recv(sock , buffer , DEFAULT_BUFLEN , 0), "Client unable to receive data from the server.\n");
    
        if(strncmp(buffer, "END", 3) == 0)
        {
            bzero(buffer, DEFAULT_BUFLEN);
            printf("Press enter to return to menu...\n");
            fflush(stdout);
            getchar();
            break;
        }
        
        printf("%s", buffer);
        fflush(stdout);
        bzero(buffer, DEFAULT_BUFLEN);        
    }
}

void ReceiveDigitalModules(char* buffer, int sock)
{
    while(1)
    {
        CheckSendRecv(recv(sock , buffer , DEFAULT_BUFLEN , 0), "Client unable to receive data from the server.\n");
    
        if(strncmp(buffer, "END", 3) == 0)
        {
            bzero(buffer, DEFAULT_BUFLEN);
            printf("Press enter to return to menu...\n");
            fflush(stdout);
            getchar();
            break;
        }
        
        printf("%s", buffer);
        fflush(stdout);
        bzero(buffer, DEFAULT_BUFLEN);       
    }
}

void ReceiveModuleByName(char* buffer, int sock)
{
    char name[B32];
    printf("Type name of the module > ");
    fflush(stdout);
    
    fgets(name, B32, stdin);
    CheckSendRecv(send(sock, name, B32, 0), "Client failed to send search query to the server.\n");
    CheckSendRecv(recv(sock, buffer, DEFAULT_BUFLEN, 0), "Client failed to receive response from the server.\n");  
    printf("%s", buffer);
    bzero(buffer, DEFAULT_BUFLEN);

    fflush(stdout);
    printf("Press enter to return to menu...\n");
    fflush(stdout);
    getchar();
}

void ChangeModuleByName(char* buffer, int sock)
{
    printf("Type the name of the module you wish to modify.\n> ");
    fflush(stdout);
    char nameBuffer[B32];
    fgets(nameBuffer, B32, stdin);
    
    CheckSendRecv(send(sock, nameBuffer, B32, 0), "Client failed to send search query to the server.\n");
    CheckSendRecv(recv(sock , buffer , B2 , 0), "Client failed to receive response from the server.\n");
        
    if(strncmp(buffer, "1", 1) == 0) //"1" flags means it doesn't exist
    {
        bzero(buffer, DEFAULT_BUFLEN);
        printf("Module with specified name not found.\n");
        fflush(stdout);
        printf("Press enter to return to menu...\n");
        fflush(stdout);
        getchar();
        return;
    }
    else if(strncmp(buffer, "0", 1) == 0) //"0" flag means it exists
    {
        bzero(buffer, DEFAULT_BUFLEN);
        char newValue[B16];
        int value;
        while(1)
        {
            printf("Type a valid new value for the selected module.\n> ");
            fflush(stdout);
            fgets(newValue, B16, stdin);
            if(!IsNumber(newValue))
            {
                continue;
            }
            
            value = atoi(newValue);
            if(value > SHRT_MAX || value < SHRT_MIN)
            {
                continue;
            }

            break;
        }

        bzero(newValue, 16);
        sprintf(newValue, "%d", value);
        
        CheckSendRecv(send(sock, newValue, B16, 0), "Client failed to send new value for the specified module.\n");
        printf("Press enter to return to menu...\n");
        fflush(stdout);
        getchar();
        return;                 
    }
    else
    {
        printf("Received undefined acceptance token.\n");
        fflush(stdout);
        return;
    }
}