/*
	AUTORI:     Dikanovic Mihailo RA 84/2021
		        Djurdjevic Aleksa RA 85/2021
		

	KURS:       Osnovi racunarskih mreza I
		
		
	POSLEDNJE   
    IZMENE:		14.1.2024
*/
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdbool.h>
#include<stdlib.h>
#include<limits.h>

#include "client_functions.h"

void ServerDialogue(int sock);

int main()
{
    int sock;
    struct sockaddr_in server;
    
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        fflush(stdout);
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to the server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Connect failed. Error");
        return 1;
    }

    puts("Connected\n");
    ServerDialogue(sock);

}

void ServerDialogue(int sock)
{
    
    char response[DEFAULT_BUFLEN];
    char charSelection[B8];
    int selection;
    
    while(1)
    {
        bzero(response, DEFAULT_BUFLEN);

        CheckSendRecv(recv(sock , response , strlen(aio) , 0), "Client unable to receive the menu from the server.\n");
        printf("%s", response);
        fflush(stdout);

        bzero(charSelection, B8);
        
        do
        {
            selection = CheckInput(charSelection); //repeats until valid input is given
        }
        while(selection < 0);
        
        CheckSendRecv(send(sock, charSelection, B8, 0), "Client was unable to send its option to the server.\n");
      
        switch(selection) //Clients acts according to the sent option
        {
            case 1:
            {
                ReceiveAllModules(response, sock);
                break;
            }

            case 2:
            {
                ReceiveAnalogModules(response, sock);             
                break;
            }

            case 3:
            {
                ReceiveDigitalModules(response, sock);
                break;
            }
           
            case 4:
            {
                ReceiveModuleByName(response, sock);
                break;
            }

            case 5:
            {
                ChangeModuleByName(response, sock);
                break;
            }

            case 6:
            {
                printf("Terminating...\n");
                fflush(stdout);
                close(sock);
                return;
            }

            default:
            {
                break;
            }
        }
    }
}