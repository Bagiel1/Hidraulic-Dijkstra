let network;
let dadosJSON;

const sleep= ms => new Promise(r => setTimeout(r, ms));

async function iniciar(){
    const response= await fetch('saida.json');
    dadosJSON= await response.json();

    const container= document.getElementById('network');
    const data= {
        nodes: new vis.DataSet(dadosJSON.nodes),
        edges: new vis.DataSet(dadosJSON.edges)
    };

    const options= {
        edges: { arrows: 'to', color: { color: '#848484' }, width: 2 },
        physics: { enabled: false}
    };

    network= new vis.Network(container, data, options);
}

function getCaminho(predecessor, destino){
    let caminho= [];
    let atual= destino;

    while(atual != -1 && atual != 0){
        let anterior= predecessor[atual];
        if(anterior == -1) break;
        caminho.push(`${anterior}-${atual}`);
        atual= anterior;
    }
    return caminho.reverse();
}

async function animar(tipo){
    const edges= new vis.DataSet(dadosJSON.edges)
    network.setData({ nodes: new vis.DataSet(dadosJSON.nodes), edges: edges });

    let predecessor= (tipo == 'dijkstra') ? dadosJSON.pred_dijkstra : dadosJSON.pred_bfs;
    let destino= dadosJSON.destino_escolhido;
    let cor= (tipo == 'dijkstra') ? '#FF0000' : '#0000FF';

    let listaArestas= getCaminho(predecessor, destino);

    for(let idAresta of listaArestas){
        let arestaDados= dadosJSON.edges.find(e => e.id == idAresta);
        let valorResistencia= parseFloat(arestaDados.label);
        let tempo= parseFloat(valorResistencia/2) * 1000;

        edges.update({ id: idAresta, color: { color: cor}, width: 5 });

        await sleep(tempo);
    }
    alert("A agua chegou! ("+ tipo.toUpperCase() + ")");

}

function animarDijkstra() { animar('dijkstra'); }
function animarBFS() { animar('bfs'); }

iniciar();