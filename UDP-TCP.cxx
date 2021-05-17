/*
 * Assignment 5
 * 
 * 	Erind HYsa
 *  Simeon Lico
 * 	      
 */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <openssl/rc4.h>
#include <string.h>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <cstdlib>
using namespace std;
int main() {
	// Create the UDP socket
	int sock, rc,n;
	unsigned int addrlen;
	char buffer[1024];	
	// Create the socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}	
	// Construct the target sockaddr_in structure
	struct sockaddr_in target; // structure for address of target	
	memset(&target, 0, sizeof(target)); /* Clear struct */
	target.sin_family = AF_INET;			/* Internet/IP */
	target.sin_addr.s_addr = inet_addr("10.158.56.43"); /* IP address */	
	// set socket to timeout mode
	struct timeval read_timeout;
	read_timeout.tv_sec = 0;
	read_timeout.tv_usec = 200000;	// varies depending of network connectivity
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

	int udpTime = 0;
	struct timespec start, stop;
	srand (time(NULL));	
	clock_gettime(CLOCK_REALTIME, &start);	
	// loop to probe for responses
	for (int port=9000; port <= 9100; port++) {
		target.sin_port = htons(port); /* target port */

		// Send the packet to the target
		rc = sendto(sock, "group 3", 7, 0, (struct sockaddr *) &target, sizeof(target));
		if (rc < 0)
		{
			perror("sendto");
			exit(EXIT_FAILURE);
		}
		// Receive the message back from the server
		addrlen = sizeof(target);
		n = recvfrom(sock, (char *)buffer, 2048, 0, (struct sockaddr *)&target, &addrlen);
		if (n < 0)
		{		
			if (errno == EWOULDBLOCK) {
				cout << "." << flush;
				continue;
			} else {
				perror("sendto");
				exit(EXIT_FAILURE);
			}
		}		
		cout << " Target port " << ntohs(target.sin_port) << " sent: " << n << " bytes\n";		
		break;
	}
	clock_gettime(CLOCK_REALTIME, &stop);
	long start_time = start.tv_sec ;
	long stop_time = stop.tv_sec;
	udpTime += stop_time - start_time;	
	cout << "Time needed to connect to UDP was: " << udpTime << " seconds\n";
	// Create the TCP socket
	int sockfd, rc1;
	char buffer1[1024];
	// Create the socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}
	// Construct the target sockaddr_in structure
	struct sockaddr_in targ; // structure for address of target	
	memset(&targ, 0, sizeof(targ)); /* Clear struct */
	targ.sin_family = AF_INET;			/* Internet/IP */
	targ.sin_addr.s_addr = inet_addr("10.158.56.43"); /* IP address */
	
	int tcpTime = 0;
	struct timespec startTcp, stopTcp;
	srand (time(NULL));	
	clock_gettime(CLOCK_REALTIME, &startTcp);
	// loop to probe for responses
	for (int port=9000; port <= 9100; port++) {			
		targ.sin_port = htons(port); /* target port */	
		// Connect to target
		rc1 = connect(sockfd, (struct sockaddr *) &targ, sizeof(targ));
		if (rc1 < 0) {
			if (errno == ECONNREFUSED) {
				cout << "." << flush;
				continue;
			} else {
				perror("connect");
				exit(EXIT_FAILURE);
			}
		}
		// Send the message to the server 
		if (write(sockfd, "group 3", 7) < 0) {
			perror("write");
			exit(EXIT_FAILURE);
		}
		// Receive the message back from the server 
		rc1 = read(sockfd, buffer1, sizeof(buffer1));
		if (rc1 < 0) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		cout << " Target port " << dec<<ntohs(targ.sin_port) << " sent: " << rc1 << " bytes\n";
		break;
	}
	clock_gettime(CLOCK_REALTIME, &stopTcp);
	long start_timeTcp = startTcp.tv_sec;
	long stop_timeTcp = stopTcp.tv_sec ;
	tcpTime += stop_timeTcp - start_timeTcp;	
	cout << "Time needed to connect to TCP was: " << tcpTime << " seconds\n";
	close(sockfd);	
	char* KEY = buffer;
	int KEY_LEN = 16;
	RC4_KEY key;
	int len = rc1;
	unsigned char *obuf = (unsigned char*)malloc(len+1);
	memset(obuf, 0, len+1);
	RC4_set_key(&key, KEY_LEN, (const unsigned char*)KEY);
	RC4(&key, len, (const unsigned char*)buffer1, obuf);
	printf("DEcoded message\n %s \n",obuf);
	return 0;
}
