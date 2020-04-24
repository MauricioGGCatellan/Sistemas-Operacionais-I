#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define BUF_LIM 15


void* consumo();		//Declaração das funções executadas nas threads.
void* producao();		

int* buffer = NULL; 		//Declaração das variáveis compartilhadas pelas threads.

int producao_indice = 0;

int consumo_indice = - 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;


void print_buffer(){		//Função auxiliar de print.

        printf("Buffer: ");

        for(int i = 0; i < BUF_LIM; i++){
                printf("%d ", buffer[i]);

        }

        printf("\n\n");
}




int main(){

	buffer = calloc(BUF_LIM, sizeof(int));	//Inicialização do buffer. Para este programa, os itens consumidos/produzidos foram representados por inteiros.



        pthread_t thread_consumidor, thread_produtor;		//Declaração e criação das threads.
       
        pthread_create(&thread_consumidor, NULL, consumo, NULL);

        pthread_create(&thread_produtor, NULL, producao, NULL);


        pthread_join(thread_consumidor, NULL);	//Espere as threads terminarem!

        pthread_join(thread_produtor, NULL);


        return 0;


}


void* consumo(){
        int last_consumo;


        while(1){				//A thread executa infinitamente!

                pthread_mutex_lock(&mutex);	//Uso de semáforo mutex: apenas uma thread executa por vez!
                

		if(consumo_indice < 0){		//Se não há itens para consumir, espere que haja produção --> uso de pthread_cond
                        pthread_cond_wait(&cond_var, &mutex);	


                }

                last_consumo = buffer[consumo_indice];		//Consumo: remocao de um item do buffer, e atualização dos índices.
                buffer[consumo_indice] = 0;
                consumo_indice--;
                producao_indice--;

                printf("Consumido do indice %d o valor %d\n", consumo_indice +1, last_consumo);
                print_buffer();

                if(producao_indice == BUF_LIM - 1){		//Se houve consumo após o buffer estar cheio, sinalizar para que a produção continue.		
                        pthread_cond_signal( &cond_var );

                }



                pthread_mutex_unlock(&mutex);
	        
		//sleep(1);			//O sleep pode ser utilizado para simular diferenças nas velocidades de execução de cada thread.
	}


}



void* producao(){
        int last_producao;



        while(1){
                pthread_mutex_lock(&mutex);
                if(producao_indice >= BUF_LIM){			//Caso o buffer esteja cheio: produção fica em espera.	
                        pthread_cond_wait(&cond_var, &mutex);

                }

                last_producao = (rand() % 100) + 1; //O "+1" garante que o valor produzido nunca é zero.
                buffer[producao_indice] = last_producao;		//Produção: gera um valor aleatório entre 1 e 100, e o coloca no buffer. Além disso, atualiza os índices.
                consumo_indice++;
                producao_indice++;

                printf("Produzido no indice %d o valor %d\n", producao_indice - 1, last_producao);
                print_buffer();

                if(consumo_indice == 0){		//Caso surja um item para ser consumido após o buffer estar vazio, sinalize para que o consumo retorne.
                        pthread_cond_signal(&cond_var);

                }

                pthread_mutex_unlock(&mutex);
        

		//sleep(1);	
	}


}

