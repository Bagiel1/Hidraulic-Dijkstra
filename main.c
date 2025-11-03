#include "grafo.h"

int main(){

    Graph *g1= create_graph(10);

    add_edge(g1, 0, 1);

    print_graph(g1);

    destroy_graph(g1);


    return 0;
}