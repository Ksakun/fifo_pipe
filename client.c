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
	int fd_stc;
	char buf[ SIZE];

	if(( fd_stc = open( FIFO_STC, O_RDWR)) == -1){
		perror("open FIFO_STC failed");
		exit( -1);
	}

	while( 1){
		if( read( fd_stc, buf, sizeof( buf)) == -1){
			perror("read stc failed");
			break;
		}
		
		printf("\n[SERVER] : %s", buf);

		if( memcmp( buf, "quit", 4) == 0){
			break;
		}
	}

	close( fd_stc);
	return NULL;
}

int main( int argc, char **argv){
	int fd_cts, i;
	char buf[ SIZE];
	pthread_t rd_th;

	if(( fd_cts = open( FIFO_CTS, O_WRONLY)) == -1){
		perror("open FIFO_CTS failed");
		exit( -1);
	}

	if(( pthread_create( &rd_th, NULL, read_data, NULL)) != 0){
		perror( "\nread_data thread CREATE ERROR: ");
		exit( -1);
	}

	printf("< CLIENT START >\n");
	while( 1){
		printf("[CLIENT] ");
		fgets( buf, sizeof( buf), stdin);

		if( memcmp( buf, "quit", 4) == 0){
			break;
		}

		if( write( fd_cts, buf, sizeof( buf)) == -1){
			printf("write cts failed\n");
			break;
		}
		
		printf("[CLIENT] : %s", buf);
	}

	pthread_cancel( rd_th);
	pthread_join( rd_th, NULL);
	close( fd_cts);

	printf("< CLIENT END >\n");
}
