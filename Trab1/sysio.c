#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>


int main(){
	int fd;
	char buf[30];
        strcpy(buf, "conteudo do arquivo!");
	
	fd = creat("arq_exemplo", O_WRONLY);
	
	if(fd <0){		//erro
		return 1;
	}
	
	if(write(fd, buf, 30) < 0){
		return 1;
	
	}
	
	close(fd);

	return 0;

}
