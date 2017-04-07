// Program: TCPClient
// Subject: CMPE 207
// Project - Distributed Learning Management System
// Created by Team 8.


/*****CLIENT PROGRAM*****/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <string>
using namespace std;
/*
 **********************************************************************************************************
 Function Name : error
 Purpose	   : Function to display the error message from the program to standard error.
 Parameter     : character pointer which points to the starting of the message
 Return Value  : The program exits with the condition 1
 **********************************************************************************************************
 */


void error(const char *msg)
{
	perror(msg);
	printf ("%s", strerror(errno));
	exit(1);
}


void userInfo(char* storageBuffer){

	char tempBuf[10];
	char* ID, uniqueID;
	string name;
	int UserType, newOrReturningUser, bufferWriteSuccess;
	char *ptr; // for strtol. not used anywhere
	memset(storageBuffer, '\0',BUFSIZ);


	printf("What kind of a user are you? Please choose an option\n"
			"1. System Administrator\n"
			"2. Instructor\n"
			"3. Student\n");
	fgets(tempBuf, sizeof tempBuf, stdin);
	tempBuf[strlen(tempBuf) - 1] = '\0';
	UserType = strtol(tempBuf, &ptr, 10);
	if(UserType > 3){
		printf("Not a valid option");
		userInfo(storageBuffer);
	}

	printf("Are you a new user or returning user? Please choose an option\n"
			"1. New user\n"
			"2. Returning user\n");
	fgets(tempBuf, sizeof tempBuf, stdin);
	tempBuf[strlen(tempBuf) - 1] = '\0';
	newOrReturningUser = strtol(tempBuf, &ptr, 10);

	if(newOrReturningUser == 1){
		printf("Please enter your name\n");
		fgets(tempBuf, sizeof tempBuf, stdin);
		tempBuf[strlen(tempBuf) - 1] = '\0';
		name = tempBuf;

		printf("Enter your ID\n");
		fgets(tempBuf, sizeof tempBuf, stdin);
		tempBuf[strlen(tempBuf) - 1] = 0;
		ID = tempBuf;
		bufferWriteSuccess = snprintf(storageBuffer, BUFSIZ, "%d %d %s %s", UserType, newOrReturningUser,name.c_str(), ID );
	}

	else if(newOrReturningUser == 2){
		printf("Please enter the unique ID\n");
		fgets(tempBuf, sizeof tempBuf, stdin);
		tempBuf[strlen(tempBuf) - 1] = 0;
		uniqueID = strtol(tempBuf, &ptr, 10);
		bufferWriteSuccess = snprintf(storageBuffer, BUFSIZ, "%d %d %c", UserType, newOrReturningUser,uniqueID );

	}

	else{
		printf("Not a valid option\n");
		userInfo(storageBuffer);
	}
}

int createSocket(){
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	return sockfd;

}

int createConnection(int portno, int sockfd, struct sockaddr_in serv_addr){
	int connectSuccess;
	char storageBuffer[BUFSIZ];
	socklen_t server_length;

	connectSuccess = connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	return connectSuccess;
}


int sendToServer(char* storageBuffer, int sockfd, struct sockaddr_in serv_addr){
	int rwSuccess;
	rwSuccess = sendto(sockfd, storageBuffer, strlen(storageBuffer), 0,
			(struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));

	printf("The sent message is %s and number of bytes sent is %d\n", storageBuffer, rwSuccess);
	if (rwSuccess < 0)
		error("ERROR writing to socket");
	return 1;
}

/*
 **********************************************************************************************************
 Function Name : Main
 Purpose	   : Program starts here
 Parameter     : None
 Return Value  : Integer
 *********************************************************************************************************
 */

int main(int argc, char *argv[])
{
	int rwSuccess, reponse_value, connectSuccess, sockfd, sendSuccess;
	const char* host = "localhost";
	char storageBuffer[BUFSIZ];
	struct hostent *server;
	struct sockaddr_in serv_addr;

	if(argc < 2){
		error("Invalid arguments");
		printf("Usage: ./client <Server port number>");
		exit(0);
	}

	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	int portno = atoi(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	sockfd = createSocket();

	connectSuccess= createConnection(portno, sockfd, serv_addr);
	if(connectSuccess == 0){
		userInfo(storageBuffer);
	}
	else{
		printf("Error connecting\n");
	}

	sendSuccess = sendToServer(storageBuffer,sockfd, serv_addr);
	printf("The buffer contains: %s\n", storageBuffer); //just for debugging, need to remove
	printf("The send success is %d\n", sendSuccess);


}
