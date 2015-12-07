//
//  server.h
//  Multithreaded Bank
//
//  Created by Vince Xie/Jacob Rizer on 11/28/15.
//  Copyright © 2015 Vince Xie. All rights reserved.
//

#ifndef server_h
#define server_h

typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
	int session;
    int pid;
} connection_t;

typedef struct Account {
    int open;
    char name[101];
    double balance;
    int session;
    pthread_mutex_t lock;
} Account;

typedef struct BankInfo {
    Account accounts[20];
} Bank;

typedef struct _thread_data_t {
    int sockfd;
    int newsockfd;
} thread_data;

void printInfo(int signo);

static void set_timer( int first, int next );

void setUpBank();

int addAccount(char *name);

int startAccount(char *name);

void creditAccount(int index, double amount);

int debitAccount(int index, double amount);

#endif /* server_h */
