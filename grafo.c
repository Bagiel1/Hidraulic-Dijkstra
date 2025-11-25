#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include "grafo.h"
#include "pq.h"
#include "fila.h"
#include "pilha.h"
#include "abb.h"

#ifndef max
    #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif // define macro "max" que devolve o maior entre dois valores, se não tiver sido definida antes
// tratado pelo pré-processador

Graph *create_graph(int numnodes){ // aloca e cria Grafo 
    Graph *g= (Graph*)malloc(sizeof(Graph));
    if(g == NULL){
        return NULL; // verificação de segurança
    }

    g->numnodes= numnodes; // guarda número de nós
    g->list_adj= calloc(g->numnodes,sizeof(Cano*));
    g->vertices= calloc(sizeof(Vertice), g->numnodes); // inicializa e aloca listas de vértices e de adjacência
    if(g->list_adj == NULL || g->vertices == NULL){
        free(g->vertices);
        free(g->list_adj);
        free(g);
        return NULL; // verificação de segurança
    }
    
    for(int i=0; i<numnodes; i++){
        g->vertices[i].id= i; // identificando cada vértice
        g->vertices[i].nome= NULL;
    }

    return g;

}


void remove_connect(Graph *g, int from_node, int to_node){ // removendo conexão
    Cano *cano= g->list_adj[from_node]; // aponta para primeiro cano que from_node tem conexão (lista de adjacência do from_node)
    Cano *cano_aux= NULL; // ponteiro auxiliar (nó anterior ao cano analisado)
    while(cano != NULL){
        if(cano->destino == to_node){ 
            if(cano_aux == NULL){ // se for o começo da lista
                g->list_adj[from_node]= cano->proximo;  // cano->proximo vira novo inicio (quebra conexão)
            }else{
                cano_aux->proximo= cano->proximo; // próximo do aux agora é o proximo do cano (pula o cano, removido)
            }
            free(cano); // libera cano
            return;
        }
        cano_aux= cano;
        cano= cano->proximo; // para percorrer a lista
    }
    printf("Não encontrado essa ligacao");
}

void destroy_graph(Graph *g){ // liberar grafo
    if(g == NULL){
        return; // verificação se já não é NULL
    }

    if(g->list_adj != NULL){ // percorre lista de conexões de cada nó e vai liberando uma por uma
        for(int i=0; i<g->numnodes; i++){
            Cano *cano= g->list_adj[i];
            while(cano != NULL){
                Cano *cano_liberar= cano;
                cano= cano->proximo;
                free(cano_liberar); // libera o nó analisado (que o cano conecta) e pula para o próximo
            }
        }
        free(g->list_adj); // libera lista de adjacência
    }

    if(g->vertices != NULL){ // libera cada vértice
        for(int i=0; i<g->numnodes; i++){
            if(g->vertices[i].nome != NULL){
                free(g->vertices[i].nome); // liberando os nomes
            }
        }
        free(g->vertices); // libera lista de vértices
    }
    
    free(g); // libera grafo
}

void print_graph(Graph *g){ // printa situação do grafo
    if(g == NULL) return; // verificação

    printf("digraph {\n");

    for(int i=0; i<g->numnodes; i++){ // printa nome e altura de cada vértice
        char *nome= g->vertices[i].nome ? g->vertices[i].nome : "SemNome";
        printf("%d nome: %s altura: %.1f\n", i, nome, g->vertices[i].altura);
    }

    printf("\n");

    for(int from=0; from<g->numnodes; from++){
        Cano *cano= g->list_adj[from]; // percorre conexões de cada nó, para printar conexões
        while(cano != NULL){
            if(cano->eh_reversa == true){ // conexão reciproca, evita duplicar arestas
                cano= cano->proximo; 
                continue;
            }
            // printa conexão, com nó de inicio e nó de destino
            printf("%d -> %d, resistencia: %.1f, capacidade: %d\n", from, cano->destino, cano->resistencia, cano->capacidade);
            cano= cano->proximo; // proxima aresta
        }
    }
    printf("}\n");
}

int set_data(Graph *g, int id, char *nome, float altura){ // altera nome e altura do nó ao passar o identificador
    if(g == NULL || id < 0 || id >= g->numnodes){
        return 0; // verificação de segurança
    }

    Vertice *v= &g->vertices[id]; // ponteiro para endereço da posição do id passado na lista de vértices
    v->altura= altura;

    if(v->nome != NULL){
        free(v->nome);
        v->nome= NULL; // nome do vértice como NULL
    }

    v->nome= (char*)malloc(strlen(nome) + 1); // aloca memória para o nome
    if(v->nome == NULL){
        return 0; // verificação de segurança
    }
    
    strcpy(v->nome, nome); // atribuindo o nome
    return 1;
}

int add_edge(Graph *g, int from_node, int to_node, float resistencia, int capacidade, bool eh_reversa){ // adicionar conexão, função auxiliar

    if(g==NULL || from_node<0 || from_node>=g->numnodes || to_node < 0 || to_node >= g->numnodes){
        return 0; // verificações de segurança
    }
    if(hasEdge(g, from_node, to_node)){
        return 0; // verifica se já tem conexão
    }

    
    Cano *cano_novo= (Cano*)calloc(1, sizeof(Cano)); // inicializa e aloca cano_novo
    cano_novo->destino= to_node; 
    cano_novo->proximo= g->list_adj[from_node];  // o próximo do cano novo aponta para onde a lista começava (antigo primeiro elemento)
    cano_novo->resistencia= resistencia;
    cano_novo->eh_reversa= eh_reversa;
    cano_novo->fluxo= 0;
    cano_novo->capacidade= capacidade; // atribuindo características passadas
    g->list_adj[from_node]= cano_novo; // novo primeiro elemento da lista de adjacência
    
    return 1;
}

int hasEdge(Graph *g, int from_node, int to_node){ // ver se tem conexão
    Cano *cano= g->list_adj[from_node]; // cano é o primeiro elemento da lista de adjacência de "from_node"
    while(cano != NULL){ 
        if(cano->eh_reversa == true){ // se for reciproca
            cano= cano->proximo; // cano pula para a próxima conexão da lista
            continue;
        
        }
        if(cano->destino == to_node){ // se destino do cano atual é igual ao que procuramos
            return 1;
        }
        cano = cano->proximo; // cano é atualizado para a próxima conexão da list_adj  
    }
    return 0;
}

void add_cano_com_altura(Graph *g, int id_A, int id_B, float resistencia, int capacidade){ // add aresta com base na altura dos nós
    float altura_A= g->vertices[id_A].altura;
    float altura_B= g->vertices[id_B].altura; 
    // acessa altura dos vértices com os ids passados por meio do vetor de vértices (onde quero a conexão)
    // vai do nó mais alto para o mais baixo a direção da conexão

    if(altura_A > altura_B){
        float resistencia_final= resistencia;
        if(g->vertices[id_B].tipo == TIPO_RESERVATORIO){ // se o vértice B for um reservatório
            resistencia_final += g->vertices[id_B].dados.reservatorio.capacidade * 0.001; 
            // a capacidade de armazenamento é multiplicada por um fator de 0.001
        }
        add_edge(g, id_A, id_B, resistencia_final, capacidade, false); // adiciona aresta de A a B
        add_edge(g, id_B, id_A, resistencia_final, 0, true); // cria aresta virtual de B a A apenas para o algortimo Ford-Fulkerson
    }else if(altura_B > altura_A){
        float resistencia_final= resistencia;
        if(g->vertices[id_A].tipo == TIPO_RESERVATORIO){ // verifica se o vértice é do tipo reservatório
            resistencia_final += g->vertices[id_A].dados.reservatorio.capacidade * 0.001; 
        }
        add_edge(g, id_B, id_A, resistencia_final, capacidade, false); // adiciona a aresta de B a A
        add_edge(g, id_A, id_B, resistencia_final, 0, true); // aresta virtual para Ford-Fulkerson (com eh_reversa == true)
    }else{ // altura_A == altura_B (plano)
        float resistencia_final1= resistencia;
        float resistencia_final2= resistencia;
        if(g->vertices[id_A].tipo == TIPO_RESERVATORIO){ // faz a mesma verificação e multiplicaçã0 pelo fator
            resistencia_final2 += g->vertices[id_A].dados.reservatorio.capacidade * 0.001;
        }
        if(g->vertices[id_B].tipo == TIPO_RESERVATORIO){ // mesma verificação
            resistencia_final1 += g->vertices[id_B].dados.reservatorio.capacidade * 0.001;
        }
        add_edge(g, id_A, id_B, resistencia_final1, capacidade, false); // add aresta de A a B
        add_edge(g, id_B, id_A, resistencia_final2, capacidade, false); // add aresta de B a A
    }
}

// o vetor predecessor acessa os ids dos nós de onde os nós que estamos analisando saem 
// por exemplo, predecessor[id_do_vertice_analisado] = id_do_vertice_de_onde_veio

void djisktra(Graph *g, int origem, float *distancias, int *predecessor){ // calcula caminho de menor resistência total entre dois nós
    FilaPrio *pq= create_pq(g->numnodes * g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool)); // inicializa e aloca vetor de nós visitados

    for(int i=0; i<g->numnodes; i++){ // percorre todos os vértices do grafo e inicializa vetores
        distancias[i]= FLT_MAX; // distâncias infinita
        predecessor[i]= -1; // todos sem nó "pai"
        visitados[i]= false; // ninguém visitado
    }
    distancias[origem]= 0.0; // define origem
    insere(pq, origem, 0.0);

    while(!pq_vazia(pq)){
        int atual= remove_min(pq); // nó de menor distância acumulada atual (resistência total)

        if(atual == -1) break; // verificação de segurança

        if(visitados[atual]) continue; // se já visitamos o nó, continua

        visitados[atual]= true; // visita nó atual

        Cano *cano= g->list_adj[atual];
        while(cano != NULL){ // percorre lista de adjacência do nó atual
            if(cano->eh_reversa == true){
                cano= cano->proximo; // ignora arestas virtuais e passa para a próxima conexão
                continue;
            }
            int vizinho= cano->destino;
            float resistencia_cano= cano->resistencia;
            float nova_dist= distancias[atual] + resistencia_cano; // custo acumulado

            if(nova_dist < distancias[vizinho]){ // caminho mais curto que o conhecido
                distancias[vizinho]= nova_dist;
                predecessor[vizinho]= atual;
                insere(pq, vizinho, nova_dist); // atualiza distância, nó "pai" e adiciona na fila de prioridades para ser o novo nó atual
            }
            cano= cano->proximo; // atualiza conexão
        }
    }
    destroy_pq(pq); // libera fila de prioridades
    free(visitados); // libera vetor de visitados
}

void BFS(Graph *g, int origem, int *predecessor){ // calcula caminho com menor número de arestas (independente do custo)
    Fila *f= criarFila(g->numnodes); 
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool)); // inicializa e aloca vetor booleano que guarda nós visitados

    for(int i=0; i<g->numnodes; i++){ // percorre todos os vértices do grafo e inicializa vetor "predecessor"
        predecessor[i]= -1;
    }

    visitados[origem]= true; // visita nó origem
    enfileirar(f, origem);

    while(!filaVazia(f)){
        int atual= desenfileirar(f); // primeiro da fila
        
        if(atual == -1) break; // verificação de segurança

        Cano *cano= g->list_adj[atual]; // percorre todos os nós vizinhos (conexões com o nó atual)
        while(cano != NULL){
            if(cano->eh_reversa == true){ // ignora aresta virtual
                cano= cano->proximo; // atualiza conexão para a próxima associada ao nó atual
                continue;
            }
            int vizinho= cano->destino;

            if(visitados[vizinho] == false){ 
                visitados[vizinho] = true;
                predecessor[vizinho] = atual; // se não foi visitado, visita e salva predecessor (quem "descobriu" vizinho)

                enfileirar(f, vizinho); // enfileira vizinho para explorar "filhos" depois
            }
            cano= cano->proximo; // atualiza cano
        }
    }
    encerrarFila(f); // libera fila
    free(visitados); // libera vetor de nós visitados
}

bool *alcancaveis(Graph *g, int origem){ // retorna vetor com os nós que foram visitados (true) ou não (false)
    // vê quem recebe água, mesma lógica de BFS
    Fila *f= criarFila(g->numnodes);

    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool)); // inicializa e aloca vetor booleano
    visitados[origem]= true; // visita origem
    enfileirar(f, origem); // coloca na fila

    while(!filaVazia(f)){
        int atual= desenfileirar(f); // primeiro da fila
        Cano *cano= g->list_adj[atual]; 

        while(cano != NULL){ // percorre lista de adjacência do nó atual
            if(cano->eh_reversa == true){
                cano= cano->proximo; // ignora aresta virtual e atualiza conexão
                continue;
            }
            int vizinho= cano->destino;
            if(visitados[vizinho] == false){
                visitados[vizinho] = true;
                enfileirar(f, vizinho); // se vizinho não foi visitado, visita e enfileira para explorar "filhos" depois
            }
            cano= cano->proximo; // atualiza cano
        } 
    }
    encerrarFila(f); // libera fila
    return visitados; // retorna vetor de nós visitados (abastecidos ou não)
}


void imprimir_caminho_pilha(Graph *g, int destino, int *predecessor){  
    // faz uma pilha para imprimir caminho, já que o vetor "predecessor" guarda o caminho do fim para o começo
    Pilha *p= create_pilha(g->numnodes);
    push(p, destino); // empilho vértice destino

    int proximo= predecessor[destino]; // auxiliar para percorrer todos os nós, acessa de onde o vértice destino veio
    while(proximo != -1){
        push(p, proximo); // vai empilhando os vértices
        proximo= predecessor[proximo]; // atualiza valor
    }

    while(!pilha_vazia(p)){
        printf("%d -> ", pop(p)); 
        // enquanto a pilha não tiver vazia, vai retirando os valores e imprimindo do topo ao fundo
        // mostra o caminho até o vértice destino
    }
    
    encerrarPilha(p); // libera pilha
}

void imprimir_arestas_prim(Graph *g, int *predecessor){ // apenas para visualizar arestas do algoritmo de Prim
    
    printf("Arestas da Arvore Geradora Minima (Prim):\n");

    for(int i = 0; i < g->numnodes; i++) { // percorre todos os vértices do grafo
        if(predecessor[i] != -1) {
            printf("%d -> %d\n", predecessor[i], i); // imprime a conexão entre dois vértices enquanto existir o predecessor
        }
    }
}

float get_resistencia(Graph *g, int from_node, int to_node){ // função de busca auxiliar para descobrir resistência do cano que conecta dois nós
    Cano *cano= g->list_adj[from_node]; // acessamos list_adj de from_node (primeira conexão de from_node)

    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo; // atualiza cano e ignora as arestas virtuais
            continue;
        }
        if(cano->destino == to_node){ // se nó para onde cano atual vai for o que estamos procurando, retorna a resistência
            return cano->resistencia;
        }
        cano= cano->proximo; // atualiza cano para a próxima conexão de from_node
    }
    return -1.0; // erro ou não há conexão
}

void dfs_recursive(Graph *g, int origem, int destino, bool *visitados, Pilha *p){  // imprime caminhos
    if(origem == destino){ 
        push(p, origem); // empilha
        print_pilhaContrario(p); // imprime caminho
        printf("\n");
        pop(p); // desempilha para explorar outros nós
        return;
    }
    push(p, origem); // add nó atual na pilha
    visitados[origem]= true; // visita nó atual
    Cano *cano= g->list_adj[origem];
    while(cano != NULL){ // percorre lista de adjacência do nó origem
        if(cano->eh_reversa == true){
            cano= cano->proximo; // ignora aresta virtual e pula para próxima conexão
            continue;
        }
        if(visitados[cano->destino] == false){ // se ainda não foi visitado, explora ele usando recursão
            dfs_recursive(g, cano->destino, destino, visitados, p);
        }
        cano= cano->proximo; // atualiza conexão
    }
    visitados[origem]= false; // desmarca nó visitado para outros que passem por ele, se necessário
    pop(p); // desempilha nó atual

    return;
}

void dfs_abb(Graph *g, int origem, int destino, bool *visitados, Pilha *p, Arvore *abb, float custo_total){ 
    // salva caminhos na abb, a partir do custo total (mesma lógica do dfs_recursive)
    if(origem == destino){
        push(p, origem);
        int tamanho= pilha_get_tamanho(p); // tamanho da pilha
        if(tamanho <= 0){ // verificação
            printf("Caminho inválido (tamanho 0).\n");
            pop(p);
            return;
        }
        int *caminho= (int*)calloc(tamanho, sizeof(int)); // aloca e inicializa vetor do caminho (com ids dos vértices)
        if(caminho == NULL){
            printf("Nao conseguiu alocar memória para o caminho.\n");
            pop(p);
            return; // verificação de segurança
        }
        adicionar_caminhos(p, caminho); // salva o vetor do caminho (cópia pilha)
        adicionar(abb, custo_total, caminho, tamanho); // insere na abb, custo_total é a chave
        pop(p);
        return;
    }
    push(p, origem);
    visitados[origem]= true; // visita nó atual
    Cano *cano= g->list_adj[origem];
    while(cano != NULL){ // percorre a lista de adjacência do nó atual
        if(cano->eh_reversa == true){
            cano= cano->proximo; // ignora aresta virtual e passa para a próxima conexão
            continue;
        }
        if(visitados[cano->destino] == false){ // se ainda não foi visitado, define novo custo acumulado e chama por recursão, para explorar 
            float novo_custo= custo_total + cano->resistencia;
            dfs_abb(g, cano->destino, destino, visitados, p, abb, novo_custo);
        }
        cano= cano->proximo; // atualiza a conexão
    }
    visitados[origem]= false; // desmarca nó visitado para que outros passem por ele, se necessário
    pop(p); // desempilha nó atual

    return;
}

void prim(Graph *g, int origem, float *distancias, int *predecessor){  
    // conecta todos os nós com o menor custo total (calcula árvore geradora mínima), lógica parecida com o dijkstra
    FilaPrio *pq= create_pq(g->numnodes * g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool)); // aloca e inicializa um vetor booleano de nós visitados

    for(int i=0; i<g->numnodes; i++){ // percorre todos os vértices do grafo e inicializa vetores
        distancias[i]= FLT_MAX; // distâncias infinita
        predecessor[i]= -1; // todos sem nó "pai"
        visitados[i]= false; // ninguém visitado
    }
    distancias[origem]= 0.0; // define origem
    insere(pq, origem, 0.0); //

    while(!pq_vazia(pq)){
        int atual= remove_min(pq); // nó de menor custo com a árvore atual

        if(atual == -1) break; // verificação de segurança

        if(visitados[atual]) continue; // se já visitamos o nó, continua

        visitados[atual]= true; // visita nó atual

        Cano *cano= g->list_adj[atual];
        while(cano != NULL){ // percorre lista de adjacência do nó atual
            if(cano->eh_reversa == true){
                cano= cano->proximo; // ignora arestas virtuais e passa para a próxima conexão
                continue;
            }
            int vizinho= cano->destino;
            float resistencia_cano= cano->resistencia;

            if(!visitados[vizinho] && resistencia_cano < distancias[vizinho]){ 
                // se o vizinho não tiver sido visitado e se o cano ter menor custo do que a conexão que tinhamos antes para este vizinho 
                distancias[vizinho]= resistencia_cano; // atualiza custo
                predecessor[vizinho]= atual; // nó da onde vizinho veio
                insere(pq, vizinho, resistencia_cano);
            }
            cano= cano->proximo; // atualiza conexão
        }
    }

    destroy_pq(pq); // libera fila de prioridades
    free(visitados); // libera vetor de visitados
}


void DFS(Graph *g, int origem, int destino, int qual, Arvore *abb){ // encontra todos os caminhos possíveis entre dois pontos
    // agora, utilizando as funções já preparadas
    Pilha *p= create_pilha(g->numnodes);
    bool *visitados= (bool*)calloc(g->numnodes, sizeof(bool)); // inicializa e aloca vetor de visitados
    if(qual == 1){ // recursivo, para imprimir caminhos
        dfs_recursive(g, origem, destino, visitados, p);
    }else if(qual == 2){ // para a abb, salva caminhos a partir do custo total
        dfs_abb(g, origem, destino, visitados, p, abb, 0);
    }
    encerrarPilha(p); // libera pilha
    free(visitados); // libera vetor de visitados
    printf("\n");
}

int busca_caminho_ford_fukerson(Graph *g, int origem, int destino, int *predecessor, FilaPrio *pq){
    // busca caminho do algoritmo Ford-Fulkerson, função auxiliar para achar os caminhos entre um nó e outro, que calcula o fluxo máximo
    for(int i=0; i<g->numnodes; i++){
        predecessor[i]= -1; // inicialização do vetor predecessor
    }
    int *fluxo_max_ate_no= (int*)calloc(g->numnodes, sizeof(int)); // inicializa e aloca vetor de fluxo maximo ate o nó
    if(fluxo_max_ate_no == NULL) return 0; // verificação 

    fluxo_max_ate_no[origem]= -INT_MAX; // inicializa origem com capacidade infinita
    insere(pq, origem, -INT_MAX); // usamos "-" para ser um Heap Máximo (já que a fila de prioridades que implementamos, na verdade, usa Heap Mínimo)
    // considerar as magnitudes para interpretação
    while(!pq_vazia(pq)){
        int removido= remove_min(pq); // remove nó com maior capacidade
        Cano *cano= g->list_adj[removido];
        while(cano != NULL){ // percorre lista de adjacência do nó removido
            int vizinho= cano->destino; // nó para qual o cano de conexão vai (a partir do removido)
            int capacidade_total= fluxo_max_ate_no[removido]; // até nó atual
            int cap_residual= -cano->capacidade + cano->fluxo; // capacidade livre do cano ainda
            int gargalo= max(cap_residual, capacidade_total);
            // gargalo é o valor mínimo entre as duas capacidades (colocamos max por estar representando de outra forma, com o "-")
            // valor do fluxo de água que dá para passar nos canos

            if(cap_residual < 0 && gargalo < fluxo_max_ate_no[vizinho]){ // se o cano ainda tem capacidade e o nó vizinho tem um caminho com maior fluxo
                fluxo_max_ate_no[vizinho]= gargalo; // melhor capacidade para chegar no vizinho
                predecessor[vizinho]= removido; // da onde o vizinho veio
                insere(pq, vizinho, fluxo_max_ate_no[vizinho]);
            }
            cano= cano->proximo; // atualiza conexão
        }
    }
    free(fluxo_max_ate_no); // libera vetor com fluxos máximos para cada nó
    return (predecessor[destino] != -1); // true se chegar no destino
}

int ford_fukerson(Graph *g, int origem, int destino, int contar){ // Calcular fluxo máximo entre dois nós
    int fluxo_total= 0; // inicializando o acumulador
    int iteracao= 1;

    int *predecessor= (int*)malloc(g->numnodes * sizeof(int)); // alocando o vetor dos nós "pais" (de onde os outros vem)
    FilaPrio *pq= create_pq(g->numnodes * g->numnodes);

    zerar_fluxos(g); // inicializa cálculo

    while(busca_caminho_ford_fukerson(g, origem, destino, predecessor, pq)){ // enquanto encontrarmos caminho 
        if(contar == 1){
            printf("\nCaminho de Aumento #%d: ", iteracao++);
            Pilha *p = create_pilha(g->numnodes);
            int rastro = destino;
            while(rastro != -1){ // vai percorrendo o caminho e se a orgiem for igual ao rastro, encontramos o que queremos
                push(p, rastro);
                if(rastro == origem) break;
                rastro = predecessor[rastro]; // atualiza para valor da onde veio (do destino a origem, para printar da origem ao destino)
            }
            while(!pilha_vazia(p)){
                printf("%d", pop(p)); // printa pilha do topo ao fundo
                if(!pilha_vazia(p)){
                    printf(" -> ");
                }
            }
            encerrarPilha(p); // libera pilha
        }

        int atual= destino; // do destino para a origem
        int gargalo_caminho= INT_MAX;

        while(atual != origem){
            int anterior= predecessor[atual]; // da onde o nó atual veio
            Cano *cano= g->list_adj[anterior]; 
            while(cano != NULL){ // percorre lista de adjacência do nó anterior, para acessar propriedades
                if(cano->destino == atual){
                    int cap_res= cano->capacidade - cano->fluxo; // capacidade livre
                    if(cap_res < gargalo_caminho){ 
                        gargalo_caminho = cap_res; // se for menor, atualiza o gargalo
                    }
                    break;
                }
                cano= cano->proximo; // atualiza conexão
            }
            atual= anterior; // atualiza para nó anterior
        }
        if(gargalo_caminho <= 0 || gargalo_caminho == INT_MAX) break; // verificação de segurança
        if(contar == 1) printf(" [Gargalo: %d]\n", gargalo_caminho);

        atual= destino; // começa no destino
        while(atual != origem){ // percorre caminho para calculo fluxo
            int anterior= predecessor[atual]; // da onde o nó atual veio
            Cano *cano= g->list_adj[anterior];
            while(cano != NULL){ // percorrendo a lista de adjacencia do nó anterior
                if(cano->destino == atual){ // cano que vai do anterior para o atual
                    cano->fluxo += gargalo_caminho; // "enche" mais o cano
                    break;
                }
                cano= cano->proximo; // atualiza conexão
            }
            Cano *cano_rev= g->list_adj[atual];
            while(cano_rev != NULL){ // lista de adjacência do nó atual
                if(cano_rev->destino == anterior){ // cano que vai do atual para o interior (reverso de antes)
                    cano_rev->fluxo -= gargalo_caminho; // negativa o fluxo na aresta reversa
                    break;
                }
                cano_rev= cano_rev->proximo; // atualiza conexão
            }
            atual= anterior; // atual "desce" um
        }
        fluxo_total += gargalo_caminho; // gargalo do caminho somado ao fluxo total 
    }
    if(contar == 1){
        printf("Fluxo Maximo Calculado: %d\n", fluxo_total);
    }

    free(predecessor); // liberando vetor predecessor e liberando fila de prioridades
    destroy_pq(pq);

    return fluxo_total;
}

void zerar_fluxos(Graph *g){ // inicializa fluxos
    for(int i=0; i<g->numnodes; i++){ // percorre lista de adjacência de cada vértice
        Cano *cano= g->list_adj[i];
        while(cano != NULL){ // zera o fluxo de todas as conexões entre os nós
            cano->fluxo= 0;
            cano= cano->proximo;
        }
    }
}

void analisar_corte_agua(Graph *g, int origem, int cano_from, int cano_to){ 
    // função para ver quais vértices não são abastecidos ao cortar uma conexão
    
    float resistencia_cano= get_resistencia(g, cano_from, cano_to); // busca resistência do cano entre dois nós
    if(resistencia_cano == -1.0){
        return; // verificação de conexão ou erro
    }

    printf("\n--- Analisando Corte de Agua no Cano (%d -> %d) ---\n", cano_from, cano_to);

    bool *alcancaveis_antes= alcancaveis(g, origem); // vetor de nós visitados (abastecidos ou não) antes

    printf("Removendo o cano %d -> %d...\n", cano_from, cano_to);
    Cano *cano= g->list_adj[cano_from]; // percorre lista de adjacência do cano_from
    while(cano != NULL){
        if(cano->eh_reversa == true){
            cano= cano->proximo; // ignora aresta virtual e passa para próxima conexão
            continue;
        }
        if(cano->destino == cano_to) break; // se econtrar cano que quero, sai o while para salvar informações
        cano= cano->proximo; // atualiza conexão
    }
    int capacidade= cano->capacidade;
    bool eh_reversa= cano->eh_reversa;
    remove_connect(g, cano_from, cano_to); // remove conexão do grafo

    bool* alcancaveis_depois = alcancaveis(g, origem); // vetor de nós visitados (abastecidos ou não) após quebra de conexão
    printf("Relatorio de Abastecimento:\n");
    bool algum_corte = false;
    for(int i=0; i<g->numnodes; i++){ // percorre os vértices e vê quais deixaram de ser abastecidos
        if(alcancaveis_antes[i] == true && alcancaveis_depois[i] == false){
            printf("O No %d (%s) ficou sem agua!\n", i, g->vertices[i].nome);
            algum_corte= true;
        }
    }
    if(!algum_corte){
        printf("Nenhum no perdeu o abastecimento (provavelmente existe uma rota alternativa).\n");
    }
    
    add_edge(g, cano_from, cano_to, resistencia_cano, capacidade, eh_reversa); // adiciona aresta novamente entre dois nós depois de remover

    free(alcancaveis_antes);
    free(alcancaveis_depois); // libera vetores boolenaos
}


void exportar_json(Graph *g, int* pred_dijkstra, int* pred_bfs, int destino, int max_flow){ // exporta para arquivo json na web (para visualização)
    FILE *f = fopen("vis/saida.json", "w");
    if (f == NULL) return; // verificação para abertura do arquivo

    fprintf(f, "{\n");
    
    fprintf(f, "  \"nodes\": [\n");
    for (int i = 0; i < g->numnodes; i++){ // exporta cada vértice, cor com base no tipo e imprime nome e altura do nó
        char *color = (g->vertices[i].tipo == TIPO_RESERVATORIO) ? "#97C2FC" : "#E0E0E0";
        fprintf(f, "    {\"id\": %d, \"label\": \"%s\\n(Alt: %.1f)\", \"color\": \"%s\"}%s\n", 
                i, g->vertices[i].nome, g->vertices[i].altura, color, 
                (i == g->numnodes - 1) ? "" : ",");
    }
    fprintf(f, "  ],\n");

    
    fprintf(f, "  \"edges\": [\n"); // agora para exportar as arestas
    int primeiro = 1;
    for (int i = 0; i < g->numnodes; i++){
        Cano *cano = g->list_adj[i]; // percorrendo listas de adjacência dos i's
        while (cano != NULL){
            if(cano->eh_reversa == true){ // ignorando arestas virtuais
                cano= cano->proximo;
                continue;
            }
            if (!primeiro) fprintf(f, ",\n");

            char *cor_aresta= (cano->fluxo > 0) ? "#0044cc" : "#848484"; // se tem fluxo é azul

            int espessura= (cano->fluxo > 0) ? (cano->fluxo / 2) + 3 : 1; // linha proporcional ao fluxo
            if(espessura > 10) espessura = 10; // limite 

            // imprime arestas e seus dados
            fprintf(f, "    {\"from\": %d, \"to\": %d, \"resistance_val\": %.2f, \"flow_val\": %d, \"capacity_val\": %d, \"label\": \"R: %.1f\\nF: %d/%d\", \"color\": {\"color\":\"%s\"}, \"width\": %d, \"id\": \"%d-%d\"}", 
                i, cano->destino, 
                cano->resistencia, 
                cano->fluxo,       
                cano->capacidade,  
                cano->resistencia,             
                cano->fluxo, cano->capacidade, 
                cor_aresta, 
                espessura, 
                i, cano->destino);
                
            primeiro = 0;
            cano = cano->proximo;
        }
    }
    fprintf(f, "\n  ],\n");

    // exporta caminhos de dijkstra e bfs pelos predecessores

    fprintf(f, "  \"pred_dijkstra\": [");
    for(int i=0; i < g->numnodes; i++) fprintf(f, "%d%s", pred_dijkstra[i], (i<g->numnodes-1)?",":"");
    fprintf(f, "],\n");

    fprintf(f, "  \"pred_bfs\": [");
    for(int i=0; i < g->numnodes; i++) fprintf(f, "%d%s", pred_bfs[i], (i<g->numnodes-1)?",":"");
    fprintf(f, "],\n");

    // dados da simulação
    fprintf(f, "  \"destino_escolhido\": %d,\n", destino);

    fprintf(f, "  \"max_flow\": %d\n", max_flow);

    fprintf(f, "}\n");
    fclose(f); // fecha
    printf("Arquivo 'saida.json' gerado com sucesso!\n");
}
