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

int ** parse_list(char * buf);

int main(void) {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); // host to network short
    inet_aton("127.0.0.1", &(server_addr.sin_addr));
    memset(&(server_addr.sin_zero), 0, 8);

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Server is listening port #%d\n", PORT);
    listen(sockfd, 3);

    int client_sockfd, len;
    struct sockaddr_in client_addr;  
    char buf[1024];
    int start, step;
    int ** digits;

    while(1) {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        printf("New connection with %s on port #%d\n", inet_ntoa(client_addr.sin_addr), PORT);
        send(client_sockfd, "Put int array:\n", 16, 0);
        len = recv(client_sockfd, &buf, sizeof(buf), 0);

        printf("Received: %s", buf);
        digits = parse_list(buf);            

        int ok = 1;
        for (int i = 0; i < SIZE; i++) {
            if (digits[i] != NULL) {
                if(*(digits[i]) < 0) 
                    ok = 0;
            }
            else break;
        }

        memset(buf, '\0', SIZE);
        if (ok) {
            int j = 0;
            for (int i = 0; i < SIZE; i++) {
                if (digits[i] != NULL) {
                    for (int m = 0; m < *(digits[i]); m++) {
                        buf[j++] = '*';
                    }
                    buf[j++] = '\n';
                }
                else break;
            }
            send(client_sockfd, buf, SIZE, 0);
        }
        else send(client_sockfd, "Wrong input\n", 13, 0);  

        memset(buf, '\0', SIZE);       
        for (int i = 0; i < SIZE; i++) {
            if (digits[i]) {
                free(digits[i]);
                digits[i] = NULL;
            }
            else break;
        }  
        free(digits);
        digits = NULL;
   
        printf("Connection with %s on port #%d is closed\n", inet_ntoa(client_addr.sin_addr), PORT);
        close(client_sockfd);
    }
    return 0;
}

int ** parse_list(char * buf) {
    int ** ptr = (int **)malloc(sizeof(int *) * SIZE); // [1024] NULL
    for (int i = 0; i < SIZE; i++) ptr[i] = NULL;

    char digit[SIZE] = {'\0'};
    int pos = 0, idx = 0;

    for (int i = 0; i < SIZE; i++) {
        if (buf[i] == '\0') break;
        if (buf[i] != ' ' && buf[i] != '\n') digit[pos++] = buf[i];
        else {
            ptr[idx] = (int *)malloc(sizeof(int));   
            *(ptr[idx]) = atoi(digit);
            pos = 0;
            idx++;
        }     
    }
    return ptr;
}