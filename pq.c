#include <stdio.h>
#include <stdlib.h>
#include "pq.h"


struct item{
    int id; // identificador e prioridade do item
    float prioridade;
};


FilaPrio *create_pq(int tam_max){
    FilaPrio *pq= (FilaPrio*)malloc(sizeof(FilaPrio)); // aloca memória com um ponteiro
    if(pq == NULL){
        return NULL;
    } // verificação de segurança
    pq->heap= (Item*)calloc(tam_max, sizeof(Item));  // faz um vetor de 0's com tam_max espaços
    if(pq->heap == NULL){
        free(pq);
        return NULL; // verificação de segurança
    }
    pq->tam_atual= 0; // começa com tamanho 0
    pq->tam_max= tam_max;

    return pq;
}

void destroy_pq(FilaPrio *pq){
    free(pq->heap);
    free(pq); // libera Fila de prioridade e o vetor com os itens
}

int pq_vazia(FilaPrio *pq){
    return pq->tam_atual == 0;  // verifica se está vazia
}

void trocar(FilaPrio *pq, int i, int j){ // troca dois elementos no vetor heap
    Item temp; // para guardar elemento
    temp= pq->heap[i];
    pq->heap[i]= pq->heap[j];
    pq->heap[j]= temp; // copia temp para o outro
}

void subir(FilaPrio *pq, int i){ // sobe o elemento com menor prioridade no heap, que é tipo uma árvore
    int pai= (i-1)/2;
    while(i > 0 && pq->heap[i].prioridade < pq->heap[pai].prioridade){
        trocar(pq, i, pai); // se filho tem prioridade menor que pai, troca
        i=pai;
        pai= (i-1)/2;
    }
}

void descer(FilaPrio *pq, int i){ // desce o elemento na fila de prioridade para equilibrar
    int filho1, filho2, menor;
    while((i*2)+1 < pq->tam_atual){ // enquanto tiver filho

        filho1= (i*2)+1;
        filho2= (i*2)+2;

        menor= filho1;

        if(filho2 < pq->tam_atual && pq->heap[filho2].prioridade < pq->heap[filho1].prioridade){ 
            // define os filhos e vê qual tem menor prioridade
            menor= filho2;
        }

        if(pq->heap[i].prioridade > pq->heap[menor].prioridade){ // compara pai com menor filho e troca se for maior
            trocar(pq, i, menor);
            i= menor;
        }else{
            break;
        }
    }
}

void insere(FilaPrio *pq, int id, float prioridade){ // insere novo elemento no heap
    if(pq->tam_atual == pq->tam_max){
        return;
    }
    int i= pq->tam_atual;
    pq->heap[i].id= id; // coloca no fim
    pq->heap[i].prioridade= prioridade; 
    pq->tam_atual++; // aumenta o tamanho em 1
    subir(pq, i); // para colocar no lugar certo
}

int remove_min(FilaPrio *pq){ // remove elemento com menor prioridade
    if(pq_vazia(pq)) return -1; // verifica se está vazia

    int id_removido= pq->heap[0].id; // raiz
    int id_ultimo= pq->tam_atual-1;
    pq->heap[0]= pq->heap[id_ultimo]; // move último para raiz
    pq->tam_atual--; // diminui o tamanho da lista
    descer(pq, 0); // restaura heap
    
    return id_removido;
}


