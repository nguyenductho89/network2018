#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>


int main()
{
    int sockfd, clientfd;
    socklen_t clen;
    struct sockaddr_in saddr, caddr;
    //Using port
    unsigned short port = 8784;
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
        return 0;
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    if ((bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)) {
        printf("Error binding\n");
        return 0;
    }
    if (listen(sockfd, 5) < 0) {
        printf("Error listening\n");
        return 0;
    }
    clen=sizeof(caddr);
    if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
        printf("Error accepting connection\n");
        return 0;
    }
    ssize_t messageSize;
    char message[256];
    while(clientfd > 0){
        //Receive message
        messageSize = recv(clientfd, message, sizeof(message), 0);
        if (messageSize < 0){
            printf("ERROR reading from socket");
        }else{
            printf("@@@@@@@@@@@@ Client: %s\n",message);
        }
        //Send message
        printf("...\n");
        scanf("%s",message) ;
        messageSize = send(clientfd, message, strlen(message), 0);
        if (messageSize < 0){
            printf("ERROR writing to socket");
        }else{
            printf("############# Server: %s\n",message);
        }
    }
    //Close port before exit program
    shutdown(sockfd, SHUT_RDWR);
    return 0;
}


