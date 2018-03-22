#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

void connectToServer(char *hostname) {
    struct hostent* h;
    h = gethostbyname(hostname);
    if (h == NULL) {
        printf("Unknown host\n");
        return;
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
            return;
        }
        printf("Connect successfully!\n");
        
    }
}


int main( int argc, char *argv[] ){
    char hostName[100];
    if ( argc == 1 ) {
        printf("Nhap hostname\n");
        scanf("%s",hostName);
        connectToServer(hostName);
    } else {
        printf( "IPAddress of :%s",argv[1] );
        connectToServer(argv[1]);
    }
    return 0;
}


