/*
 * tcp server
 * Ref: http://www.thegeekstuff.com/2011/12/c-socket-programming/
 * Added getsockopt for SO_PEERCRED
 * Oct 4, 2015
 * root@davejingtian.org
 * http://davejingtian.org
 */
#define _GNU_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <sys/time.h>

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    struct ucred cred;
    int len;

    char sendBuff[1025];
    time_t ticks; 

    struct timeval start_tv, end_tv;

    /* daveti: init cred */
    memset(&cred, 0x0, sizeof(cred));
    len = sizeof(cred);

    /* daveti: display our pid */
    printf("TCP server pid [%u]\n", getpid());

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

	/* daveti: retrieve the peer cred */
	gettimeofday(&start_tv, NULL);
	if (getsockopt(connfd, SOL_SOCKET, SO_PEERCRED, &cred, &len))
		printf("Error: getsockopt failed with error %s\n",
			strerror(errno));
	else
		printf("Client pid/uid/gid [%u/%u/%u]\n",
			cred.pid, cred.uid, cred.gid);
	gettimeofday(&end_tv, NULL);
	printf("socket2pid query time [%lu] us\n",
		((end_tv.tv_sec-start_tv.tv_sec)*1000000 +
		(end_tv.tv_usec-start_tv.tv_usec)));

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
}
