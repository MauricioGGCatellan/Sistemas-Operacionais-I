#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

int main(){
	size_t tampage = getpagesize();
	char *mspace = (char *) mmap( (void*) (tampage*20000000), tampage, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, 0, 0);
	
	printf("Tamanho da pagina: %d\n",(int) tampage); 

	mspace[0] = 'o'; mspace[1]= 'i'; mspace[2] = '!'; mspace[3] = '\n'; mspace[4] = '\0';
	printf("Conteudo do espaco mapeado: %s\n", mspace);

	return 0;
	
}
