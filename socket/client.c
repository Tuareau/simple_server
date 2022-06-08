#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 9000
#define SIZE 1024

int main(void) {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Couldn't create socket\n");
        exit(-1);
    }

    struct sockaddr_in server_addr;    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // host to network short
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int c = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (c < 0) {
        printf("Couldn't not connect to server\n");
        exit(-1);
    }
    else {
        printf("Client connected to server\n");
        printf("Waiting...\n");
    }

    int len;
    char buf[1024] = {'\0'};

    len = recv(sockfd, &buf, sizeof(buf), 0);
    printf("Received: %s", buf);
    fgets(buf, SIZE, stdin);
    send(sockfd, buf, SIZE, 0);

    memset(buf, '\0', sizeof(buf));
    len = recv(sockfd, &buf, sizeof(buf), 0);
    for (int i = 0; i < SIZE; i++) {
        fputc(buf[i], stdout);
    }

    close(sockfd);

    return 0;
}