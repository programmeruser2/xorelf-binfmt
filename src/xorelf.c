#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "xorkey.h"

typedef enum {
	encrypt, 
	decrypt,
	interpret
} Action;

void convert(char* in, char* out, Action action) {
	FILE* f = fopen(in, "r");
	if (f == NULL) {
		perror("fopen");
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	long length = ftell(f);
	rewind(f);
	char* buf = malloc(sizeof(char)*length);
	fread(buf, length, 1, f);
	fclose(f);
	if (action == encrypt) {
		buf[0] = 'X';
	} else {
		buf[0] = 0x7f;
	}
	for (long i = 4; i < length; ++i) {
		buf[i] ^= XORKEY;
	}
	FILE* output;
	int fd;
	if (action == interpret){
		fd = memfd_create(in, O_RDWR);
		output = fdopen(fd, "w");
	} else {
		output = fopen(out, "w");
	}	
	if (output == NULL) {
		perror("fopen");
		exit(1);
	}
	fwrite(buf, length, 1, output);
	if (action != interpret) fclose(output);
	free(buf);
	if (action == interpret) {
		char* argv[] = {in, NULL};
		char* envp[] = {NULL};
		int res = fexecve(fd, argv, envp);
		if (res < 0) {
			perror("fexecve");
			exit(1);
		}
	}	
}


int main(int argc, char* argv[]) {
	// TODO: don't hardcode xor key?
	if (argc < 3 || ((!strcmp(argv[1], "encrypt") || !strcmp(argv[1], "decrypt")) && argc < 4)) {
		puts("usage: xorelf [encrypt|decrypt] <input file> <output file> or xorelf interpret <file>");
		return 1; 
	}
	if (!strcmp(argv[1], "encrypt")) {
		convert(argv[2], argv[3], encrypt);
	} else if (!strcmp(argv[1], "decrypt")) {
		convert(argv[2], argv[3], decrypt);
	} else if (!strcmp(argv[1], "interpret")) {
		convert(argv[2], NULL, interpret);
	} else {
		printf("Unknown action %s", argv[1]);
		return 1;
	}
	return 0;
}
