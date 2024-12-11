#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *alg, *CaminhoFile, line[20], EndTemp[9];
int TamPag, TamMem, numPaginas, operacoes=0, leituras=0, escreve=0, acertos=0, perdas=0, writebacks=0, PaginasUsadas=0;
float erros=0;
FILE *file;
struct Pagina *primeiro, *ultimo;

using namespace std;

struct Pagina {
    char Endereco[9];
    Pagina *prox;
};



void AddNovaPag(char valor[9]){
    Pagina *atual = (Pagina*)malloc(sizeof(Pagina));
    strcpy(atual->Endereco, valor);
    atual->prox = NULL;

    if(PaginasUsadas == 0){
        primeiro = atual;
        ultimo = primeiro;
    }
    else {
        ultimo->prox = atual;
        ultimo = atual;
    }

    if(PaginasUsadas < numPaginas)
        PaginasUsadas++;

    escreve++;
}

void LRU(char valor[9]){
    AddNovaPag(valor);
    if(PaginasUsadas == numPaginas)
        primeiro = primeiro->prox;
}

void FIFO(char valor[9]){
    AddNovaPag(valor);
    if(PaginasUsadas == numPaginas)
        primeiro = primeiro->prox;
}

void Random(char valor[9]){
    escreve++;
    srand(time(NULL));
    int index = rand() % PaginasUsadas;
    Pagina *tmp = primeiro;
    for(int i = 0; i < index; i++){
        tmp = tmp->prox;
    }
    strcpy(tmp->Endereco, valor);
}

bool Achar(char valor[9]){
    Pagina *tmp = primeiro, *ant = NULL;
    while(tmp != NULL){
        if(strcmp(tmp->Endereco, valor)==0){
            if(strcmp(alg, "lru") == 0){
                if(ant != NULL){
                    if(tmp->prox != NULL)
                        ant->prox = tmp->prox;
                }
                else {
                    primeiro = primeiro->prox;
                }
                ultimo->prox = tmp;
                ultimo = tmp;
                tmp->prox = NULL;
            }

            return true;
        }
        ant = tmp;
        tmp = tmp->prox;
    }
    return false;
}

void SubstPagina(char valor[9]){
    if(strcmp(alg, "lru") == 0){
        LRU(valor);
    }
    else if(strcmp(alg, "random") == 0){
        Random(valor);
    }
    else if(strcmp(alg, "fifo") == 0){
        FIFO(valor);
    }
    writebacks++;
}

void EscreveEnd(char valor[9]){
    if(PaginasUsadas < numPaginas){
        AddNovaPag(EndTemp);
    }
    else{
        erros++;
        SubstPagina(EndTemp);
    }
}

void LimpaMemoria(){
    Pagina *tmp = primeiro;
    while(tmp != NULL){
        free(tmp);
        tmp = tmp->prox;
    }
    fclose(file);
}

int main(int argc, char *argv[]){
    alg = argv[1];
    CaminhoFile = argv[2];
    TamPag = atoi(argv[3]);
    TamMem = atoi(argv[4]);

    if(TamPag < 2 || TamPag > 64){
        printf("ERRO: O tamanho de cada pagina deve estar entre 2 e 64.");
        return 0;
    }

    if(TamMem < 128 || TamMem > 16384){
        printf("ERRO: O tamanho da memoria deve estar entre 128 e 16384.");
        return 0;
    }

    if(strcmp(alg, "lru") && strcmp(alg, "fifo") && strcmp(alg,"random")){
        printf("ERRO: O algoritmo deve ser lru, fifo ou random.");
        return 0;
    }

    numPaginas = TamMem/TamPag;

    if(strlen(CaminhoFile) > 0){
        file = fopen(CaminhoFile, "r");
        while(fgets(line, 20, file) != NULL){
            operacoes++;
            strncpy(EndTemp, line, 8);
            EndTemp[8] = '\0';
            if(line[9] == 'W' || line[9] == 'w'){
                EscreveEnd(EndTemp);
            }
            else if(line[9] == 'R' || line[9] == 'r'){
                if(Achar(EndTemp)){
                    acertos++;
                }
                else{
                    perdas++;
                    EscreveEnd(EndTemp);
                }
                leituras++;
            }
            else{
                printf("ERRO: Os dados do arquivo de entrada estao em formato incorreto.");
                return 0;
            }
        }
    }
    else{
        printf("ERRO: Arquivo de entrada inválido.");
        return 0;
    }

    printf("\nExecutando o simulador...\n");
    printf("Tamanho da memoria: %iKB\n", TamMem);
    printf("Tamanho das paginas: %iKB\n", TamPag);
    printf("Tecnica de reposicao: %s\n", alg);
    printf("Numero de paginas: %i\n", numPaginas);
    printf("Operacoes no arquivo de entrada: %i\n", operacoes);
    printf("Operacoes de leitura: %i\n", leituras);
    printf("Operacoes de escrita: %i\n", escreve);
    printf("Pagina acertos: %i\n", acertos);
    printf("Pagina perdas: %i\n", perdas);
    printf("Numero de writebacks: %i\n", writebacks);
    printf("Taxa de Pagina fault: %f%% \n", erros/escreve*100);

    LimpaMemoria();

    return 0;
}
