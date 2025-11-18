#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "abb.h"
#include "pilha.h"

struct no{          //Struct dos nós da arvore
    float custo;
    No *direita, *esquerda;
    int *caminho;
    int tamanho_caminho;
};

struct arvore{      //Struct da árvore em si
    No *raiz;
};


Arvore* criarABB(){        //Aloca a árvore e começa com raiz nula
    Arvore *arvore= (Arvore*)malloc(sizeof(Arvore));
    if(arvore == NULL){
        return NULL;
    }
    arvore->raiz = NULL;
    return arvore;
}

int adicionar(Arvore *arvore, float custo, int *caminho, int tamanho){     //Faz uma tipo de busca binária, vendo se o código é maior ou menor
    No *p= (No*)malloc(sizeof(No));                                      //que o ponteiro auxiliar está e atualiza o aux a partir disso
    if(p == NULL || caminho == NULL){      //Aloca um novo nó
        return 1;
    }

    p->caminho= caminho;
    p->custo= custo;
    p->tamanho_caminho= tamanho;
    p->direita= NULL;
    p->esquerda= NULL;

    if(arvore->raiz == NULL){       //Se a árvore estiver vazia o nó adicionado vai ser a raíz e pronto
        arvore->raiz= p;
        return 0;    
    }
    No *aux= arvore->raiz;          //Cria ponteiro auxiliar
    while(1){                       //Faz a "busca binária" até achar o ponto onde o novo nó tem que ser adicionado
        if(aux->custo < custo){
            if(aux->direita == NULL){
                aux->direita = p;
                return 0;
            }
            aux= aux->direita;

        }else if(aux->custo > custo){
            if(aux->esquerda == NULL){
                aux->esquerda = p;
                return 0;
            }
            aux= aux->esquerda;

        }else if(aux->custo == custo){        //Não adiciona codigos repetidos
            printf("Custo já existe!\n");
            free(p->caminho);
            free(p);
            return 1;
        }
    }
}


No *buscar(Arvore *arvore, float custo){         //Busca o código no nós da arvore, mesmo estilo de "busca binária"
    No *aux= arvore->raiz;                      //Ponteiro auxiliar
    while(aux != NULL){
        if(aux->custo < custo){
            aux= aux->direita;
        }else if(aux->custo > custo){
            aux= aux->esquerda;
        }else{
            return aux;
        }
    }
    printf("Custo %f nao encontrado.\n", custo);         //Se sair do while não encontrou o produto
    return NULL;
}

void caminho(No *no){
    for(int i= 0; i<=no->tamanho_caminho; i++){
        printf("%d");
    }
}


void encerrarNos(No *no){       //Função auxiliar do encerrarArvore que da free em todos os nós individualmente com o método de posordem
    if(no == NULL){
        return;
    }
    encerrarNos(no->esquerda);
    encerrarNos(no->direita);
    free(no->caminho);
    free(no);
}

void encerrarArvore(Arvore *arvore){        //Encerra a arvore
    if(arvore == NULL){
        return;
    }

    encerrarNos(arvore->raiz);
    free(arvore);
}

int *get_caminho_do_no(No *no){
    return no->caminho;
}

int get_tamanho_do_no(No *no){
    return no->tamanho_caminho;
}