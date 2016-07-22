#define _XOPEN_SOURCE 600

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <syslog.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

void extract_ip_string(struct sockaddr *address, char *dst, size_t len);

int main(int argc, char *argv[])
{
	struct addrinfo *results, *pretreatment, hints = {0};
	int errcode = 0;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	errcode = getaddrinfo("pretreatment", "1111", &hints, &pretreatment);
	if(errcode) {
		fprintf(stderr, "Could not get info of port 1111: %s\n",
				gai_strerror(errcode));
		return -1;
	}

	int out = socket(pretreatment->ai_family, pretreatment->ai_socktype, 0);
	if(out < 0) {
		perror("Could not create socket\n");
		freeaddrinfo(pretreatment);
		return -2;
	}
	hints.ai_socktype = SOCK_STREAM;
	errcode = getaddrinfo("residential", "1111", &hints, &results);
	if(errcode) {
		fprintf(stderr, "Could not get info of port 1111: %s\n",
				gai_strerror(errcode));
		return -1;
	}
	// First one is 127.0.1.1
	results = results->ai_next;

	//Get IP String
	/*
	char string[50] = {'\0'};
	extract_ip_string(results->ai_addr, string, 16);	
	printf("%s\n", string);
	*/
	int in = socket(results->ai_family, results->ai_socktype, 0);
	if(in < 0) {
		perror("failed to create inbound socket.\n");
		return -in;
	}

	errcode = bind(in, results->ai_addr, results->ai_addrlen);
	int incoming = listen(in, 10);
	struct sockaddr_storage remote = {0};
	socklen_t sz = sizeof(remote);
	int conn = 0;
	while(( conn = accept(in, (struct sockaddr *) &remote, &sz))) {
		//printf("Inside While.\n");
		if ( conn < 0 ) {
			//printf("Conn < 0\n");
			perror("Could not accept connection.");
		}
		char *buffer = calloc(1, 65535);
		//printf("Before recvfrom\n");
		ssize_t size = recvfrom(conn, buffer, 65536, 0, (struct sockaddr *)&remote, &sz);
		//extract_ip_string((struct sockaddr *)&remote, ip_as_str,
				//sizeof(ip_as_str));
		//syslog(LOG_INFO, "%s is raining", ip_as_str);

		// 
		//printf("%ld\n", size);
		//printf("Before sendto\n");
		sendto(out, buffer, size, 0, pretreatment->ai_addr, pretreatment->ai_addrlen);
		//printf("End of while.\n");
		free(buffer);
	}
	perror("Just in case.\n");
	close(in);
	close(out);
	return 0;
}

void extract_ip_string(struct sockaddr *address, char *dst, size_t len)
{
	void *addr;

	if(address->sa_family == AF_INET6) {
		// IPv6 Land
		addr = &((struct sockaddr_in6 *)address)->sin6_addr;
	} else {
		// IPv4 Land
		addr = &(((struct sockaddr_in *)address)->sin_addr);
	}
	inet_ntop(address->sa_family, addr, dst, len);
}


