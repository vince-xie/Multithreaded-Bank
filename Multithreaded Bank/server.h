//
//  server.h
//  Multithreaded Bank
//
//  Created by Vince Xie on 11/28/15.
//  Copyright © 2015 Vince Xie. All rights reserved.
//

#ifndef server_h
#define server_h

typedef struct Account {
    int open;
    char name[101];
    double balance;
    int session;
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

void addAccount(int index, char *name);

void creditAccount(int index, int amount);

void debitAccount(int index, int amount);

#endif /* server_h */
