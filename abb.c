#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "abb.h"
#include "pilha.h"

struct caminho_item{
    int *caminho; // vetor com caminho (nós visitados)
    int tamanho; // quantidade de vértices
    CaminhoItem *proximo; // próximo item
};

struct no{          //Struct dos nós da arvore
    float custo; // peso, sua chave
    No *direita, *esquerda; // filhos
    CaminhoItem *lista_caminhos; // lista dos caminhos 
};

struct arvore{      //Struct da árvore em si, para o nó da raiz
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

CaminhoItem *criar_caminho_item(int *caminho, int tamanho){ // aloca CaminhoItem (nó lista), auxilia na função adicionar
    CaminhoItem *novo= (CaminhoItem*)malloc(sizeof(CaminhoItem));
    novo->caminho= caminho;
    novo->tamanho= tamanho;
    novo->proximo= NULL;
    return novo;
}

int adicionar(Arvore *arvore, float custo, int *caminho, int tamanho){     //Faz uma tipo de busca binária, vendo se o código é maior ou menor
    CaminhoItem *novoItem= criar_caminho_item(caminho, tamanho);

    if(arvore->raiz == NULL){ // se a árvore tiver vazia
        No *p= (No*)malloc(sizeof(No));       
        p->custo= custo;
        p->lista_caminhos= novoItem;
        p->esquerda= NULL;
        p->direita= NULL;
        arvore->raiz= p;
        return 0;
    }

    No *aux= arvore->raiz;          //Cria ponteiro auxiliar
    while(1){                       //Faz a "busca binária" até achar o ponto onde o novo nó tem que ser adicionado
        if(custo < aux->custo){
            if(aux->esquerda == NULL){
                No *p= (No*)malloc(sizeof(No));
                p->custo= custo;
                p->lista_caminhos= novoItem;
                p->esquerda= NULL;
                p->direita= NULL;
                aux->esquerda= p;
                return 0;
            }
            aux= aux->esquerda;

        }else if(custo > aux->custo){
            if(aux->direita == NULL){
                No *p= (No*)malloc(sizeof(No));
                p->custo= custo;
                p->lista_caminhos= novoItem;
                p->esquerda= NULL;
                p->direita= NULL;
                aux->direita= p;
                return 0;
            }
            aux= aux->direita;

        }else if(aux->custo == custo){        //Não adiciona codigos repetidos
            novoItem->proximo= aux->lista_caminhos;
            aux->lista_caminhos= novoItem;
            return 0;
        }
    }
}


No *buscar(Arvore *arvore, float custo){         //Busca o código no nós da arvore, mesmo estilo de "busca binária"
    No *aux= arvore->raiz;                      //Ponteiro auxiliar
    while(aux != NULL){
        if(aux->custo < custo - 0.0001){
            aux= aux->direita;
        }else if(aux->custo > custo + 0.0001){
            aux= aux->esquerda;
        }else{
            return aux;
        }
    }
    printf("Custo %f nao encontrado.\n", custo);         //Se sair do while não encontrou o produto
    return NULL;
}

void imprimir_caminhos_do_no(No *no){ // imprimir caminhos armazenados no nó
    if(no == NULL) return; // verificação

    CaminhoItem *item = no->lista_caminhos;  // primeira lista de caminhos do nó
    int contador = 1;

    while(item != NULL){
        printf("Opcao %d: ", contador++); 
        for(int i = 0; i < item->tamanho; i++){ // imprime ids que compõe caminho
            printf("%d", item->caminho[i]);
            if(i < item->tamanho - 1) printf(" -> ");
        }
        printf("\n");
        
        item = item->proximo; // avança para próximo caminho da lista
    }
}

void encerrarNos(No *no){       //Função auxiliar do encerrarArvore que da free em todos os nós individualmente com o método de posordem
    
    if(no == NULL) return;

    encerrarNos(no->esquerda);
    encerrarNos(no->direita);

    CaminhoItem *item= no->lista_caminhos;
    while(item != NULL){
        CaminhoItem *temp= item;
        item= item->proximo;
        free(temp->caminho);
        free(temp);
    }
    free(no);
}

void encerrarArvore(Arvore *arvore){        //Encerra a arvore
    if(arvore == NULL){
        return;
    }

    encerrarNos(arvore->raiz);
    free(arvore);
}

void buscar_intervalo_recursivo(No *no, float min, float max, int *contador){ // busca nós da abb dentro dum intervalo
    // contador é ponteiro para um inteiro para contar nós encontrados, indo alterando o valor
    if(no == NULL){
        return; // verificação de segurança
    }
    if(no->custo > (min-0.001)){
        buscar_intervalo_recursivo(no->esquerda, min, max, contador); // busca a esquerda
    }
    if(no->custo >= (min-0.001) && no->custo <= (max+0.001)){
        printf("\n Resistencia encontrada: %.2f \n", no->custo);
        imprimir_caminhos_do_no(no); 
        (*contador)++; // se o nó tá dentro do intervalo, imprime e conta
    }
    if(no->custo < (max+0.001)){
        buscar_intervalo_recursivo(no->direita, min, max, contador); // busca a direita
    }

}

void buscarIntervalo(Arvore *arvore, float min, float max){
    if(arvore == NULL || arvore->raiz == NULL){
        printf("Nenhum caminho encontrado na arvore.\n");
        return; // auto-explicativo, verificação
    }
    int encontrados= 0; // número de caminhos encontrados
    printf("Buscando caminhos com resistencia entre %.2f e %.2f...\n", min, max);

    buscar_intervalo_recursivo(arvore->raiz, min, max, &encontrados); // chama função recursiva

    if(encontrados == 0){
        printf("Nenhum caminho encontrado no intervalo!\n");
    }else{
        printf("\nTotal de caminhos unicos encontrados no intervalo: %d\n", encontrados);
    }

}