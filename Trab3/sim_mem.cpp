#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <list>
#include <climits>
#include <ctime>

#define PMEM_SIZE 5             //Tamanho da memória principal, em páginas.
#define SMEM_SIZE 150           //Tamanho da memória secundária, em páginas.
#define PAGE_SIZE 1024          //Tamanho das páginas e quadros de página.
#define LOGICAL_ADDRESS_SIZE 20 //Tamanho em bits do endereço lógico.

using namespace std;

//Struct para as entradas das tabela de páginas.
typedef struct pt_entry {
    int page;
    time_t referenced;
    bool present;       // present/absent
    int pf;             // page frame
} pt_entry;

class Process {

    private:

        int size;
        list<pt_entry> page_table;

    public: 

        // Constructor da classe, recebe como argumentos dois vetores de posições livres na
        //      memória para saber para onde mapear as páginas.

        Process(int size, bool available_prim[PMEM_SIZE], bool available_sec[SMEM_SIZE]){

            this -> size = size;

            // Cria a tabela de páginas.
            for(int i = 0; i < size; i++){

                pt_entry temp;

                temp.present = false;
                temp.page = i;
                temp.pf = -1;
                temp.referenced = 0;

                // Primeiro tenta colocar as páginas na memória principal.
                for (int j = 0; j < PMEM_SIZE; j++){
                    if (available_prim[j] == true){
                        available_prim[j] = false;
                        temp.present = true;
                        temp.pf = j;
                        break;
                    }
                }

                // Se a memória principal estiver cheia, tenta colocar na memória secundária
                if (temp.present == false)
                    for (int j = 0; j < SMEM_SIZE; j++){
                        if (available_sec[j] == true){
                            available_sec[j] = false;
                            temp.pf = j;
                            break;
                        }
                    }

                // Se a memória secundária também estiver cheia, apaga as páginas já colocadas e lança um erro.
                if (temp.pf == -1){
                    cout << "Out of memory. freeing previous pages." << endl;

                    while (!page_table.empty()){
                        pt_entry last = page_table.back();
                        if (last.present)
                            available_prim[last.pf] = true;
                        else available_sec[last.pf] = true;
                        page_table.pop_back();

                    }
                    throw "Out of memory";
                }
                
                // Coloca na tabela e imprime na tela. 
                page_table.push_back(temp);
                cout << "Página " << temp.page << " mapeada para o quadro de página " << temp.pf << " na memória " << (temp.present? "primária" : "secundária")  << endl;
            }
        }

        // função que serve tanto para leitura quanto para escrita na memória. 
        void access(int address, unordered_map<string, Process>* processes, char type){

            //Calcula a página e o offset a partir do endereço lógico, baseado
            int page = (int) address / PAGE_SIZE;
            int offset =  address % PAGE_SIZE;

            cout << "Procurando a página " << page << " offset " << offset << endl;
            if (page > size){
                cerr << "Argumento inválido";
                return;
            }

            list<pt_entry>::iterator it = page_table.begin();
            advance(it, page);
            if (it->present){
                cout << (type == 'W'?"Escrevendo na ": "Lendo ")<< " página " << page << " (page frame " << it->pf << ") offset " << offset << "." << endl;
                it->referenced = time(NULL);
            }
            else {cout << "Page fault." << endl;            
                LRUPageReplacement(processes, &(*it));
                this->access(address, processes, type);
            }

        }

        int getSize(){
            return size;
        }

        // Retorna a entrada da tabela de páginas que foi acessada a mais tempo.
        pt_entry* get_lru(){
            pt_entry* lru = NULL;
            int lowest_referenced = INT_MAX;
            for (auto it = page_table.begin(); it != page_table.end(); ++it){
                if (it -> present && it -> referenced < lowest_referenced){
                    lru = &(*it);
                    lowest_referenced = it -> referenced;
                }
            }
            return lru;
        }
        
        void LRUPageReplacement(unordered_map<string, Process>* processes, pt_entry* new_page){
            int lowest = INT_MAX;
            pt_entry* lru; 
            string key;


            for (auto it = (*processes).begin(); it != (*processes).end(); ++it){
                pt_entry* temp = (*it).second.get_lru();
                if (temp != NULL && temp -> referenced < lowest){
                    lowest = temp -> referenced;
                    lru = temp;
                    key = (*it).first;
                }
            }

            cout << "Pondo página no quadro de página " << lru -> pf << endl << endl;
            int pf_swap = lru -> pf;
            lru -> pf = new_page -> pf;
            new_page -> pf =  pf_swap ;
            new_page -> present = true;
            lru -> present = false;
        }

        void print(){

            cout << " Virtual Page \t|\t Present/Absent \t|\t Page Frame " << endl << "=================================================================================" << endl;

            for (auto it = page_table.begin(); it != page_table.end(); ++it){
                cout << "\t" << it -> page << "\t|\t\t" << it -> present << "\t\t|\t\t" << ((it -> present)? to_string(it -> pf) : "X") << endl << "---------------------------------------------------------------------------------" << endl;


            }



        }

};

int main(int argc, char **argv){

    unordered_map<string, Process> active_processes;
    // lista todos os espaços disponíveis na memória principal.
    bool available_prim[PMEM_SIZE];
    for (int i = 0; i< PMEM_SIZE; i++){
        available_prim[i] = true;
    }
    // lista todos os espaços disponíveis na memória secundária.
    bool available_sec[SMEM_SIZE];
    for (int i = 0; i< SMEM_SIZE; i++){
        available_sec[i] = true;
    }

    // O programa deve ser rodado como './programa arquivo_de_entrada'
    if (argc < 2){
        cout << "Usage: ./program_name input_file" << endl;
        return 1;
    }

    string file_in = argv[1];

    fstream input;
    input.open(file_in, ios::in);

    cout << "Input: " << file_in << endl;

    string pid, arg;
    char type;

    // Lê as linhas do arquivo de entrada em sequencia.
    for (int i = 0; input >> pid >> type >> arg; i++){

        cout <<endl<< "Line " << i << ": " << pid << " " << type << " " << arg << endl << endl;

        if (active_processes.count(pid) == 0 && type != 'C'){
            cout << "Process \"" <<  pid << "\" does not exist!" << endl;
            continue;
        }

        //Remove o "()2" da string
        int arg_i = 0;
        if(arg.find("(") < arg.length())
            arg_i = stoi(arg.substr(1, arg.find(")")));

        switch(type){
            case 'R':
            case 'W':
                if (arg_i > pow(2,LOGICAL_ADDRESS_SIZE)){
                    cout << "Erro: O endereço lógico deve ser de " << LOGICAL_ADDRESS_SIZE << " bits." << endl;
                        } 
                active_processes.at(pid).access(arg_i, &active_processes, type);
                break;
            case 'P':
            case 'I':
                // Se 'P' imprime que a instrução é pela CPU, se não (se for 'I') imprime que é de I/O
                cout << "Processo " << pid << " executou a instrução " << arg << ((type == 'P')? " pela CPU": " de I/O") << endl;
                break;
            case 'C': //Cria um novo processo
                if (active_processes.count(pid) != 0){
                    cout << "O processo \"" <<  pid << "\" já existe!" << endl;
                    break;
                }
                try{
                    Process newProcess(stoi(arg), available_prim, available_sec);
                    pair<string, Process> newPair(pid, newProcess);
                    active_processes.insert(newPair);
                    cout << "Criado o processo " << pid << " de tamanho " << active_processes.at(pid).getSize() << " paginas." << endl;
                }catch(const char* msg){
                    cerr << msg << endl;
                    break;
                }
        }

    }

    for (auto it = active_processes.begin(); it != active_processes.end(); ++it){
        cout << (*it).first << endl;
        (*it).second.print();
    }

    input.close();
    return 1;
}
