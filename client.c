//
//  client.c
//  Multithreaded Bank
//
//  Created by Vince Xie/Jacob Rizer on 11/27/15.
//  Copyright © 2015 Vince Xie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define NUMTHREADS 2
int sockfd, n;
struct sockaddr_in serv_addr;
struct hostent *server;

void *read2(void *arg){
    char buffer[256];
    while(1){
        bzero(buffer,256);
        n = read(sockfd,buffer,255); //read from server using socket
        if (n <= 0){
            printf("\n");
            exit(0);
        }
        printf("%s\n",buffer); //print out the server message
    }
}

void *write2(void *arg){
    char buffer[256];
    printf("Enter your command. (Enter \"exit\" to close): ");
    while(1){
        bzero(buffer,256);
        fgets(buffer,255,stdin); //read from stdin
        n = write(sockfd,buffer,strlen(buffer)); //writes to server
        if (n < 0){
            printf("Error writing to socket\n");
            exit(0);
        }
        if(strncasecmp(buffer, "Exit", 4) == 0){ //if "exit" is inputted, closes client
            close(sockfd);
            return 0;
        }
        sleep(2);
        printf("Enter your command. (Enter \"exit\" to close): ");
    }
}

int main(int argc, char *argv[])
{
    int port = 6799;
    pthread_t thr[NUMTHREADS]; //array of two threads, one for reading and one for writing
    if (argc < 2) {
        printf("Error: No hostname inputted\n");
        exit(0);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //opens socket
    if (sockfd < 0){
        printf("Error opening socket.\n");
        exit(0);
    }
    server = gethostbyname(argv[1]); //get host name
    if (server == NULL) {
        fprintf(stderr,"Error: no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    while(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ //connect to server
        printf("Server not found. Waiting 3 seconds...\n");
        sleep(3);      //wait 3 seconds to connect again
        //exit(0);
    }
    
    printf("Connected to server %s\n\n", argv[1]);
    
    //creates threads for reading and writing
    int rc = pthread_create(&thr[1], NULL, write2, NULL);
    int rc2 = pthread_create(&thr[0], NULL, read2, NULL);
    if(rc != 0 || rc2 != 0){
        printf("Error creating threads");
        return 1;
    }
    int i;
    for (i = 0; i < NUMTHREADS; i++) {
        pthread_join(thr[i], NULL);
    }
}
