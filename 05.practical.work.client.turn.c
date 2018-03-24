#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int connectToServer(char *hostname) {
    struct hostent* h;
    h = gethostbyname(hostname);
    if (h == NULL) {
        printf("Unknown host\n");
        return 0;
    }
    printf( "IPAddress of :%s",hostname );
    if (h->h_addr_list[0] != NULL) {
        printf( "\n %s\n ", inet_ntoa( *( struct in_addr*)( h -> h_addr_list[0])));
        struct sockaddr_in saddr;
        int sockfd;
        unsigned short port = 8784;
        if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("Error creating socket\n");
        }
        memset(&saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
        saddr.sin_port = htons(port);
        if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
            printf("Cannot connect\n");
            return 0;
        }
        printf("Connect successfully!\n");
        return sockfd;
    }else {return 0;}
}
void startConversation(int sockfd) {
    ssize_t messageSize;
    char message[256];
    //Send message
    printf("...\n");
    scanf("%s",message) ;
    messageSize = send(sockfd, message, strlen(message), 0);
    if (messageSize < 0){
        printf("ERROR writing to socket");
    }else{
        printf("@@@@@@@@@@@@ Client: %s\n",message);
    }
    
    //Receive message
    messageSize = recv(sockfd, message, sizeof(message), 0);
    if (messageSize < 0){
        printf("ERROR reading from socket");
    }else{
        printf("############# Server: %s\n",message);
    }
}

int main( int argc, char *argv[] ){
    char hostName[100];
    printf("Nhap hostname\n");
    scanf("%s",hostName);
    int sockfd = connectToServer(hostName);
    while (1) {
        startConversation(sockfd);
    }
    return 0;
}



