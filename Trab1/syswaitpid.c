#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int *status;
	pid_t child;
	child = fork();
	if(child == 0){
		sleep(10);
		return 0;
	}
	waitpid(child, status, 0);
	
	
	return 0;

}
