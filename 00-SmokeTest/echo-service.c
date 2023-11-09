#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 5555
#define BUFSIZE 2048

int main(int argc, char *argv[]) {

	char buf[BUFSIZE];
	int sd, cs, slen, status;
	int option_value = 1;
	struct sockaddr_in serv;
	struct sockaddr_in conn;
	socklen_t len;
	pid_t pid;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sd, SOL_SOCKET,SO_REUSEADDR , &option_value,sizeof(option_value));

	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = INADDR_ANY;

	status = bind(sd, (struct sockaddr *)&serv, sizeof(struct sockaddr));
	if(status<0) {
		perror("Couldn't bind");
		exit(1);
	}

	status = listen(sd, 4);
	if(status<0) {
		perror("Couldn't listen");
		exit(1);
	}

	while(1) {

		printf("Waiting to accept\n");
		cs = accept(sd, (struct sockaddr *)&conn, &len);
		printf("Accept complete\n");

		if(cs<0) {
			perror("Couldn't accept");
			exit(1);
		}

		printf("Forking in %d\n", getpid());
		pid = fork();
		if(pid == 0) {
			printf("Created new process %d\n", getpid());
			memset(buf, 0, BUFSIZE);

			while(recv(cs, buf, BUFSIZE, 0) != 0) {
				printf("Got from client: %s\n", buf);
				slen = strlen(buf);
				send(cs, buf, slen, 0);
				memset(buf, 0, BUFSIZE);
			}
			printf("Client closed connection\n");

			close(cs);
			exit(0);
		}
		close(cs);

	}
	close(sd);

}
