//
//  myServer.c
//  Server
//
//  Created by Nguyễn Đức Thọ on 4/5/18.
//  Copyright © 2018 nguyen.duc.tho. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/mman.h>
#define ServerPort          8989
#define MessageSize         256
#define MaxClient 4
#define BUF_SIZE 1024

void *sendMessageFromKeyboard(void *fds)
{
    int *clientfds = (int *)fds;
    char sendingMessage[MessageSize];
    while (1)
    {
        fgets(sendingMessage,MessageSize-1,stdin);
        for (int i = 0; i < MaxClient; i++)
        {
            if (clientfds[i] > 0 && sendingMessage != "\n")
            {
                send(clientfds[i], sendingMessage, strlen(sendingMessage), 0);
                printf("Me chat with %d: %s\n",i,sendingMessage);
            }
        }
    }
}
void *sendMessageFromKeyboardToFD(void *fd)
{
    int *clientfd = (int *)fd;
    char sendingMessage[MessageSize];
    while (1)
    {
        fgets(sendingMessage,MessageSize-1,stdin);
        send(*clientfd, sendingMessage, strlen(sendingMessage), 0);
        printf("Me: %s\n",sendingMessage);
    }
}
/* Handle receiving message */
void handleReceivingMessage(void *handleClient, int clientIndex){
    int *client = (int *)handleClient;
    char message[MessageSize];
    if (recv(*client, message, sizeof(message),0) > 0)
    {
        printf("Client %d: %s\n", clientIndex, message);
    } else {
        handleClient = 0;
    }
    memset(&message, 0, sizeof(message));
    
}
/* Open socket at port */
int createListeningSocketAt(int port) {
    int socketFileDescriptor;
    /* Create tcp stream socket */
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if ( socketFileDescriptor == -1 ) {
        perror("Init soket fail");
        return -1;
    }
    return socketFileDescriptor;
}
/* Address reuseable and non-blocking socket*/
void configSocketMultiplex(void *clientSocketFileDescriptor) {
    int fd = *((int *)clientSocketFileDescriptor);
    //Address reusable
    setsockopt(fd, SOL_SOCKET,
               SO_REUSEADDR, &(int){ 1 },
               sizeof(int));
    //Socket non blocking
    int clientSocketFlag = fcntl(fd, F_GETFL, 0);
    clientSocketFlag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, clientSocketFlag);
}
/* Server address setup */
struct sockaddr_in createServerSocketAdress(int port){
    struct sockaddr_in serverSocketAddress;
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverSocketAddress.sin_port = htons(port);
    return serverSocketAddress;
}
void addAConnectedClientToListeningClientList(void *clientSocketFileDescriptor,fd_set *tmpset,void *clientList) {
    int fd = *((int *)clientSocketFileDescriptor);
    int *clients = (int *)clientList;
    fd_set set = *tmpset;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    int maxfd = fd;
    for (int i = 0; i < MaxClient; i++)
    {
        if (clients[i] > 0)
        {
            FD_SET(clients[i], &set);
        }
        if (clients[i] > maxfd)
        {
            maxfd = clients[i];
        }
    }
    select(maxfd + 1, &set, NULL, NULL, NULL);
}
void addAListeningClientToAcceptedClientList(void *clientSocketFileDescriptor,void *serverSocketsAddress,void *clientList) {
    int fd = *((int *)clientSocketFileDescriptor);
    int *clients = (int *)clientList;
    struct sockaddr serverAddress = *(struct sockaddr *)serverSocketsAddress;
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int clientfd = accept(fd, (struct sockaddr *)&serverAddress, &clen);
    for (int i = 0; i < MaxClient; i++)
    {
        if (clients[i] == 0)
        {
            clients[i] = clientfd;
            break;
        }
    }
}
int main(int argc, char **argv)
{
    /* 1. Create client socket */
    int listenningFD = createListeningSocketAt(ServerPort);
    if (listenningFD == -1) return -1;
    /* 2. Create server socket */
    struct sockaddr_in serverAddress = createServerSocketAdress(ServerPort);
    /* 3. Config multiplex socket */
    configSocketMultiplex(&listenningFD);
    /* 4. Binding */
    if (bind(listenningFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Error binding\n");
        return -1;
    }
    /* 5. Listening for client */
    if (listen(listenningFD, MaxClient) < 0)
    {
        perror("Error listening\n");
        return -1;
    }
    
    printf("%s\n","Server Ready!");
    
    
    int clientCount = 0;
    int pipes_count = 0;
    int pipes[MaxClient];  // Changed type!
    int new_pipefd[2];
    for (;;){
        socklen_t clientSockLen;
        struct sockaddr_in clientAddress;
        clientSockLen = sizeof(clientAddress);
        int acceptedFD = accept (listenningFD, (struct sockaddr *) &clientAddress, &clientSockLen);
        if (acceptedFD > 0 && clientCount < MaxClient) {
            clientCount++;
            pipe(new_pipefd);
            pipes[pipes_count++] = new_pipefd[0];
            int childPID ;
            if ( (childPID = fork ()) == 0 ) {
                //New process for accepted client
                close (listenningFD);
                close(new_pipefd[0]);
                for(;;) {
                    char message[MessageSize];
                    if (recv(acceptedFD, message, sizeof(message),0) > 0)
                    {
                        printf("Client %d: %s\n",clientCount,message);
                        write(new_pipefd[1], message, strlen(message));
                        close(new_pipefd[1]);
                    }
                }
 
            }else if (childPID < 0){
                perror("fork() fail!");
                exit(-1);
            }else {
                //Main process
                close(new_pipefd[1]);
                for (int i = 0; i < pipes_count; i++) {
                    char message[MessageSize];
                    int pipe_bytes = read(pipes[i], message, sizeof(message));
                    for (int i = 0; i < pipes_count; i++){
                        if (pipe_bytes != 0)
                            printf("Testing: %.*s\n", pipe_bytes, message); // Safe!
                        
                    }
                    
                    close(pipes[i]);
                }
            }
            close(acceptedFD);
        }
        

    }

    return 0;
}

