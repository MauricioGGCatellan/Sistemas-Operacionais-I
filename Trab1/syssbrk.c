#include <stdio.h>
#include <unistd.h>

int main(){
	char *mspace = (char *) sbrk(0);
	
	printf("Usando sbrk!\n");
	
	mspace = sbrk(10);
	printf("%p\n", mspace);

	mspace = sbrk(0);	


	printf("%p\n", mspace);

	return 0; 

}
