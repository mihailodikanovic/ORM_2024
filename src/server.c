/*
	AUTORI:     Dikanovic Mihailo RA 84/2021
		        Djurdjevic Aleksa RA 85/2021
		

	KURS:       Osnovi racunarskih mreza I
		
		
	POSLEDNJE   
    IZMENE:		14.1.2024
*/
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<time.h>
#include<limits.h> //SHRT_MAX
#include<stdbool.h>
#include<pthread.h>

#include "server_functions.h"

//The function threads run to communicate with a client.
void* ClientDialogue(void* sock);

int main(int argc , char *argv[])
{   
    //Input handle
    if(argc != 3)
    {
        printf("Invalid arguments.\n");
        fflush(stdout);
        return -1;
    }

    srand((unsigned int)time(NULL));    
    short i;
    g_connectedClients = 0;

    totalAnalog = atoi(argv[1]);
    totalDigital = atoi(argv[2]);
    
    if(totalAnalog > MAX_ANALOG_MODULES || totalAnalog < 0 || totalDigital > MAX_DIGITAL_MODULES || totalDigital < 0 || (totalAnalog == 0 && totalDigital == 0) || argc > 3)
    {
        printf("Invalid input. Program terminated.\n");
        fflush(stdout);
        exit(-1);
    }
    
    //Analog module generation
    for(i = 0; i < totalAnalog; i++)
    {       
        ANALOG* newAnalog = (ANALOG*)malloc(sizeof(ANALOG));

        if(newAnalog == NULL)
        {
            printf("Error. Cannot allocate memory for ANALOG structure instance!\n");
            fflush(stdout);
            FreeAnalogMemory(i);
            exit(-11);
        }

        char customName[] = "analogModule";
        char customNum[8];
        sprintf(customNum, "%d", i);
        strcat(customName, customNum);
        
        strcpy((*newAnalog).name, customName);
        (*newAnalog).value = rand() % SHRT_MAX;

        analogModules[i] = newAnalog;
    }

    //Digital module generation
    for(i = 0; i < totalDigital; i++)
    {       
        DIGITAL* newDigital = (DIGITAL*)malloc(sizeof(DIGITAL));

        if(newDigital == NULL)
        {
            printf("Error. Cannot allocate memory for DIGITAL structure instance!\n");
            fflush(stdout);
            FreeDigitalMemory(i);
            exit(-12);
        }

        char customName[] = "digitalModule";
        char customNum[8];
        sprintf(customNum, "%d", i);
        strcat(customName, customNum);

        strcpy((*newDigital).name, customName);
        (*newDigital).state = rand() % 2;

        digitalModules[i] = newDigital;
    } 

    int socketDesc , clientSock;
    socklen_t c;
    struct sockaddr_in server , client;

    //Create socket
    socketDesc = socket(AF_INET , SOCK_STREAM , 0);
    if (socketDesc == -1)
    {
        printf("Could not create socket");
        fflush(stdout);
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if(bind(socketDesc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }

    //Listen
    listen(socketDesc , MAX_CLIENTS);
    c = sizeof(struct sockaddr_in);
    puts("Waiting for connections...");

    while (1)
    {
        // Accept connection from an incoming client
        clientSock = accept(socketDesc, (struct sockaddr*)&client, &c);
        if (clientSock < 0)
        {
            perror("Accept failed.\n");
            exit(-21);
        }

        IncrementCounter();
        printf("A client has connected.\n");
        fflush(stdout);

        int* sock = (int*)malloc(sizeof(int));
        *sock = clientSock;
        
        pthread_t communicationThread;

        if (pthread_create(&communicationThread, NULL, ClientDialogue, (void*)sock) != 0)
        {
            perror("Failed to create communication thread.\n");
            return 1;
        }

        pthread_detach(communicationThread);
    }

    close(socketDesc);
    FreeAnalogMemory(totalAnalog);
    FreeDigitalMemory(totalDigital);
    return 0;
}

void* ClientDialogue(void* sock)
{
    int clientSock = *((int*)sock);
    free(sock);

    char dataReceived[B8];
    char dataToSend[DEFAULT_BUFLEN];

    while(1)
    {
        CheckSendRecv(send(clientSock, aio, strlen(aio), 0), "Server failed to send the menu to a client.\n");
        CheckSendRecv(recv(clientSock , dataReceived , B8 , 0), "Server failed to receive an option from a client.\n");

        int selection = atoi(dataReceived);
        bzero(dataToSend, DEFAULT_BUFLEN);

        switch(selection) //responds based on received number
        {
            case 1:
            {
                SendAllModules(dataToSend, clientSock);
                break;
            }

            case 2:
            {
                SendAnalogModules(dataToSend, clientSock);
                SendEndFlag(clientSock);
                break;                
            }

            case 3:
            {
                SendDigitalModules(dataToSend, clientSock);
                SendEndFlag(clientSock);
                break;
            }

            case 4:
            {
                SendModuleByName(dataToSend, clientSock);
                break;
            }

            case 5:
            {
                ChangeModuleByName(dataToSend, clientSock);
                break;
            }

            case 6:
            {
                printf("A client has disconnected.\n");
                fflush(stdout);
                DecrementCounter();
                switch(ReadCounter())
                {
                    case 0:
                    {
                        printf("There are no currently active clients.\n");
                        break;
                    }
                    case 1:
                    {
                        printf("There is 1 currently active client.\n");
                        break;
                    }
                    default:
                    {
                        printf("There are %d currently active clients.\n", ReadCounter());
                        break;
                    }
                }
                close(clientSock);
                pthread_exit(NULL);       
            }

            default:
            {
                break;
            }        
        }
    }
    close(clientSock);
    pthread_exit(NULL);
}

