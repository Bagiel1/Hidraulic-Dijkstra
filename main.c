#include "grafo.h"

int main(){

    Graph *g1= create_graph(5);
    
    

    print_graph(g1);

    destroy_graph(g1);


    return 0;
}