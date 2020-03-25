#include <stdio.h>
#include <unistd.h>

int main(){
	printf("Estou em um programa...\n");

	char *argc[] = {NULL};
	char *amb[] = {NULL};
	
	execve("./sysexecve2", argc, amb);
	
}
