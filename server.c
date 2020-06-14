#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 128
#define FIFO_STC "stc" // Server to Client
#define FIFO_CTS "cts" // Client to Server

void* read_data( void *data){
	int fd_cts;
	char buf[ SIZE];

	if(( fd_cts = open( FIFO_CTS, O_RDWR)) == -1){
		perror("open FIFO_CTS failed");
		exit( -1);
	}

	while( 1){
		if( read( fd_cts, buf, sizeof( buf)) == -1){
			perror("read cts failed");
			break;
		}
		
		printf("\n[CLIENT] : %s", buf);

		if( memcmp( buf, "quit", 4) == 0){
			break;
		}
	}

	close( fd_cts);
	return NULL;
}

int main( int argc, char **argv){
	int fd_stc, i;
	char buf[ SIZE];
	pthread_t rd_th;

	if( mkfifo( FIFO_STC, 0666) == -1){
		perror("mkfifo FIFO_STC failed");
		exit( -1);
	}

	if( mkfifo( FIFO_CTS, 0666) == -1){
		perror("mkfifo FIFO_CTS failed");
		exit( -1);
	}


	if(( pthread_create( &rd_th, NULL, read_data, NULL)) != 0){
		printf( "\nread_data thread CREATE ERROR\n");
		exit( -1);
	}

	printf("< SERVER START >\n");

	if(( fd_stc = open( FIFO_STC, O_RDWR)) == -1){
		perror("open FIFO_STC failed");
		exit( -1);
	}


	while( 1){
		printf("[SERVER] ");
		fgets( buf, sizeof( buf), stdin);

		if( memcmp( buf, "quit", 4) == 0){
			break;
		}

		if( write( fd_stc, buf, sizeof( buf)) == -1){
			printf("write stc failed\n");
			break;
		}
		
		printf("[SERVER] : %s", buf);

	}

	pthread_cancel( rd_th);
	pthread_join( rd_th, NULL);
	close( fd_stc);

	printf("< SERVER END >\n");
}
