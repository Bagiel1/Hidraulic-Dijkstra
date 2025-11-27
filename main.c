#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "grafo.h" 


void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Menu visual para facilitar o uso
void exibir_menu() {
    printf("\n================ SISTEMA HIDRAULICO ================\n");
    printf("[1]  INICIAR GRAFO (Obrigatorio)\n");
    printf("[2]  Imprimir Grafo (Debug)\n");
    printf("[3]  Adicionar Cano\n");
    printf("[4]  Remover Conexao\n");
    printf("[5]  Verificar se Tem Conexao\n");
    printf("[6]  Menor Caminho (Dijkstra - Custo)\n");
    printf("[7]  Menor Caminho (BFS - Saltos)\n");
    printf("[8]  Todos os Caminhos (DFS)\n");
    printf("[9]  Arvore Geradora Minima (Prim)\n");
    printf("[10] Buscar por Intervalo de Resistencia\n");
    printf("[11] Analisar Corte de Agua (Simulacao)\n");
    printf("[12] Fluxo Maximo (Ford-Fulkerson)\n");
    printf("[13] Exportar Visualizacao (JSON)\n");
    printf("[0]  SAIR\n");
    printf("====================================================\n");
    printf("Escolha uma opcao: ");
}

int main(){
    int numnodes = 0;
    int from_node, to_node, destino, tipo, capacidade;
    char nome[50]; 
    float altura, resistencia, min_res, max_res;
    int capacidade_reserva;

    Graph *g = NULL;
    float *distancias = NULL;
    int *predecessorDji = NULL;
    int *predecessorBFS = NULL;

    bool grafo_ativo = false;
    int opcao;

    while(1){

        #ifdef _WIN32
            system("cls");   
        #else
            system("clear");
        #endif

        exibir_menu();
        
        if (scanf("%d", &opcao) != 1) {
            printf("\n[ERRO] Entrada invalida! Digite um numero.\n");
            limpar_buffer();
            continue;
        }
        limpar_buffer(); 

        if (opcao > 1 && grafo_ativo == false) {
            printf("\nVoce precisa criar o grafo primeiro (Opcao 1)!\n");
            printf("Pressione Enter para continuar...");
            getchar();
            continue;
        }

        switch(opcao){
            case 0:
                printf("Encerrando sistema e liberando memoria...\n");
                if (grafo_ativo) {
                    free(distancias);
                    free(predecessorBFS);
                    free(predecessorDji);
                    destroy_graph(g);
                }
                return 0;

            case 1: // INICIAR GRAFO
                if(grafo_ativo){
                    printf("\n[AVISO] O grafo ja existe! Reinicie o programa para criar um novo.\n");
                } else {
                    printf("Quantos vertices (nos) tera o sistema? ");
                    scanf("%d", &numnodes);
                    limpar_buffer();

                    g = create_graph(numnodes);
                    distancias = (float*)calloc(numnodes, sizeof(float));
                    predecessorDji = (int*)calloc(numnodes, sizeof(int));
                    predecessorBFS = (int*)calloc(numnodes, sizeof(int));

                    printf("\n Configuracao dos Nos \n"); //Verificacoes de input
                    for(int i=0; i<numnodes; i++){
                        while(true){
                            printf("No ID %d -> Digite: Nome Altura Tipo(1=Juncao, 2=Reservatorio): ", i);
                            int leu= scanf("%49s %f %d", nome, &altura, &tipo);
                            limpar_buffer();

                            if(leu != 3){
                                printf("   Formato invalido! Digite: Nome(texto) Altura(numero) Tipo(1 ou 2)\n");
                                continue;
                            }
                            if(tipo != 1 && tipo != 2){
                                printf("   Tipo invalido! Digite 1 para Juncao ou 2 para Reservatorio.\n");
                                continue;
                            }
                            break;
                        }
                        
                        set_data(g, i, nome, altura);
                        
                        // Configuração específica do tipo
                        if(tipo == 1){
                            g->vertices[i].tipo = TIPO_JUNÇÃO;
                        } else if(tipo == 2){
                            g->vertices[i].tipo = TIPO_RESERVATORIO;
                            while(true){
                                printf("   >>> Capacidade do Reservatorio (inteiro) %d: ", i);
                                int leu_cap = scanf("%d", &capacidade_reserva);
                                limpar_buffer();
                                if(leu_cap == 1 && capacidade_reserva >= 0){
                                    break; 
                                }
                                printf("   Capacidade deve ser um numero inteiro positivo.\n");
                            }
                            g->vertices[i].dados.reservatorio.capacidade = (float)capacidade_reserva;
                        }
                    }
                    grafo_ativo = true;
                    printf("\nGrafo criado com sucesso!\n");
                }
                break;

            case 2: // Print
                print_graph(g);
                break;

            case 3: // Adicionar Cano
                printf("Formato: Origem Destino Resistencia Capacidade\nDigite: ");
                scanf("%d %d %f %d", &from_node, &to_node, &resistencia, &capacidade);
                limpar_buffer();
                add_cano_com_altura(g, from_node, to_node, resistencia, capacidade);
                break;

            case 4: // Remover
                printf("Digite: Origem Destino: ");
                if(scanf("%d %d", &from_node, &to_node) != 2){
                    limpar_buffer();
                    printf("Entrada invalida.\n");
                    break;
                };
                limpar_buffer();
                if(remove_connect(g, from_node, to_node)){
                    printf("Conexao %d %d removida com sucesso.\n", from_node ,to_node);
                }
                break;

            case 5: // Tem Conexão
                printf("Digite: Origem Destino: ");
                if(scanf("%d %d", &from_node, &to_node) != 2){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                if(hasEdge(g, from_node, to_node)){
                    printf("\n>> Existe conexao direta entre %d e %d.\n", from_node, to_node);
                } else {
                    printf("\n>> NAO existe conexao direta entre %d e %d.\n", from_node, to_node);
                }
                break;

            case 6: // Dijkstra
                printf("Escolha o ID do Destino (partindo do 0): ");
                if(scanf("%d", &destino) != 1){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                printf("\n Caminho de Menor Resistencia \n");
                djisktra(g, 0, distancias, predecessorDji);
                imprimir_caminho_pilha(g, destino, predecessorDji);
                printf("\n");
                break;

            case 7: // BFS
                printf("Escolha o ID do Destino (partindo do 0): ");
                if(scanf("%d", &destino) != 1){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                printf("\n Caminho com Menos Conexoes \n");
                BFS(g, 0, predecessorBFS);
                imprimir_caminho_pilha(g, destino, predecessorBFS);
                printf("\n");
                break;

            case 8: // DFS (Todos os caminhos)
                printf("Destino: ");
                if(scanf("%d", &destino) != 1){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                printf("\n Todos os Caminhos Possiveis \n");
                Arvore *abb_dfs = criarABB();
                DFS(g, 0, destino, 1, abb_dfs); // Modo 1 = Imprimir recursivo
                encerrarArvore(abb_dfs);
                break;

            case 9: // Prim
                printf("\n Arvore Geradora Minima \n");
                prim(g, 0, distancias, predecessorDji);
                imprimir_arestas_prim(g, predecessorDji);
                break;

            case 10: // Buscar Intervalo
                printf("Digite: Origem Destino Min_Res Max_Res: ");
                if(scanf("%d %d %f %f", &from_node, &destino, &min_res, &max_res) != 4){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                if(min_res > max_res){
                    printf("O valor minimo nao pode ser maior que o maximo.\n");
                    break;
                }
                Arvore *abb_busca = criarABB();
                DFS(g, from_node, destino, 2, abb_busca); // Modo 2 = Salvar na ABB
                buscarIntervalo(abb_busca, min_res, max_res);
                encerrarArvore(abb_busca);
                break;

            case 11: // Analisar Corte
                printf("Digite: No_Corte_Inicio No_Corte_Fim: ");
                if(scanf("%d %d", &from_node, &to_node) != 2){ // Reutilizando variaveis
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                analisar_corte_agua(g, 0, from_node, to_node);
                break;
            
            case 12: // Ford-Fulkerson
                printf("Calculo de Fluxo Maximo (Origem fixa no no 0)\n");
                printf("Escolha o destino do fluxo: ");
                if(scanf("%d", &destino) != 1){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();
                zerar_fluxos(g);
                ford_fukerson(g, 0, destino, 1); // 1 = Com prints de debug
                break;

            case 13: // Exportar JSON
                printf("Escolha o destino final para simulacao completa: ");
                if(scanf("%d", &destino) != 1){
                    limpar_buffer();
                    printf("Entrada Invalida.\n");
                    break;
                }
                limpar_buffer();

                if(destino < 0 || destino >= g->numnodes){
                    printf("Destino invalido.\n");
                    break;
                }
                
                // Recalcula tudo para garantir dados do JSON
                zerar_fluxos(g);
                int fluxo_total = ford_fukerson(g, 0, destino, 0); // 0 = Sem prints
                djisktra(g, 0, distancias, predecessorDji);
                BFS(g, 0, predecessorBFS);
                
                exportar_json(g, predecessorDji, predecessorBFS, destino, fluxo_total);
                break;

            default:
                printf("Opcao invalida!\n");
        }

        printf("\n[Pressione Enter para voltar ao menu...]");
        getchar();
    }
    return 0;
}