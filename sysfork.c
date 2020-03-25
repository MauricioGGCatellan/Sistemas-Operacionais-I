#include <unistd.h>
#include <sys/syscall.h>

int main(){
	syscall(SYS_fork);
	while(1){

	}	

	return 0;

}

