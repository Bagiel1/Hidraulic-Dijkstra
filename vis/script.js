let network;
let dadosJSON;

// Array para permitir múltiplas águas ao mesmo tempo
let animacoesAtivas = []; 

const sleep = ms => new Promise(r => setTimeout(r, ms));

async function iniciar() {
    try {
        let response;
        try { response = await fetch('saida.json'); } 
        catch (e) { response = await fetch('../saida.json'); }
        
        dadosJSON = await response.json();
        dadosJSON.nodes = dadosJSON.nodes.map(node => {
            let novoLabel = node.label;
            // Deixa a primeira letra maiúscula pra ficar chique
            novoLabel = novoLabel.charAt(0).toUpperCase() + novoLabel.slice(1);
            // Troca o espaço antes do '(' por uma quebra de linha
            novoLabel = novoLabel.replace(' (', '\n('); 
            return { ...node, label: novoLabel };
        });
        const container = document.getElementById('network');
        
        const data = {
            nodes: new vis.DataSet(dadosJSON.nodes),
            edges: new vis.DataSet(dadosJSON.edges)
        };

       const options = {
            nodes: {
                shape: 'circle',
                widthConstraint: { minimum: 80, maximum: 80 }, // Mantém o círculo perfeito
                color: {
                    background: '#ffffff', 
                    border: '#111111',     // Borda preta fina
                    highlight: { background: '#f0f0f0', border: '#000000' },
                    hover: { background: '#ffffff', border: '#000000' }
                },
                font: {
                    multi: true,
                    size: 16,
                    // Truque: Usar fontes nativas deixa muito mais nítido que carregar fonte externa
                    face: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif',
                    color: '#000000',      // Preto absoluto para contraste máximo
                    vadjust: 0,
                    bold: { color: '#000000' } // Garante que negrito também seja preto
                },
                borderWidth: 1.5,          // Borda um pouco mais fina para elegância
                shadow: false              // <--- DESLIGAR SOMBRA (Resolve 90% do borrão)
            },
            edges: {
                arrows: { 
                    to: { enabled: true, scaleFactor: 0.5 } 
                },
                color: { 
                    color: '#dddddd',      // Cinza bem claro para não poluir
                    highlight: '#000000',
                    hover: '#000000',
                    opacity: 1.0
                }, 
                width: 1.5,                // Arestas mais finas
                smooth: { 
                    enabled: true, 
                    type: 'continuous',
                    roundness: 0.4 
                },
                font: {
                    align: 'top',
                    size: 11,
                    face: 'Arial, sans-serif', // Fonte simples para leitura rápida
                    color: '#666666',
                    strokeWidth: 3,        // Borda branca grossa ao redor do texto para "apagar" a linha de baixo
                    strokeColor: '#ffffff',
                    background: 'none'     // Remove o fundo quadrado, usa o stroke para limpar
                },
                shadow: false              // Garante que arestas não tenham sombra
            },
            physics: {
                enabled: true,
                solver: 'forceAtlas2Based',
                forceAtlas2Based: {
                    gravitationalConstant: -120, 
                    centralGravity: 0.005,
                    springLength: 230,     // Mais espaço entre os nós ajuda na leitura
                    springConstant: 0.08
                },
                stabilization: { enabled: true, iterations: 1000 }
            },
            interaction: {
                hover: true,
                zoomView: true,
                dragNodes: true
            }
        };
        network = new vis.Network(container, data, options);

        network.on("click", function (params) {
            if (params.nodes.length > 0) {
                const idClicado = params.nodes[0];
                // Preenche o input
                document.getElementById('input-destino').value = idClicado;
            }
        });

        // --- O PINTOR (CANVAS) ---
        network.on("afterDrawing", function (ctx) {
            animacoesAtivas.forEach(anim => {
                // Pega posições ATUAIS (respeita se você arrastou o nó)
                const posOrigem = network.getPositions([anim.noOrigem])[anim.noOrigem];
                const posDestino = network.getPositions([anim.noDestino])[anim.noDestino];

                if (!posOrigem || !posDestino) return;

                const nodeRadius = 20; 
                const dx = posDestino.x - posOrigem.x;
                const dy = posDestino.y - posOrigem.y;
                const distancia = Math.sqrt(dx*dx + dy*dy);

                const ux = dx / distancia;
                const uy = dy / distancia;

                // Vetor perpendicular para offset lateral
                const perpX = -uy * anim.offsetLateral;
                const perpY = ux * anim.offsetLateral;

                const startX = posOrigem.x + (ux * nodeRadius) + perpX;
                const startY = posOrigem.y + (uy * nodeRadius) + perpY;

                const endX = posDestino.x - (ux * nodeRadius) + perpX;
                const endY = posDestino.y - (uy * nodeRadius) + perpY;

                const pathX = endX - startX;
                const pathY = endY - startY;
                const currentX = startX + (pathX * anim.progresso);
                const currentY = startY + (pathY * anim.progresso);

                ctx.strokeStyle = anim.cor;
                ctx.lineWidth = 3; 
                ctx.lineCap = 'round'; 

                ctx.beginPath();
                ctx.moveTo(startX, startY);
                ctx.lineTo(currentX, currentY);
                ctx.stroke();
            });
        });

    } catch (error) {
        console.error(error);
        alert("Erro ao carregar saida.json.");
    }
}

function getCaminho(predecessor, destino) {
    let caminho = [];
    let atual = destino;
    while (atual != -1) {
        let anterior = predecessor[atual];
        if (anterior === undefined || anterior === null || anterior === -1) break;
        caminho.push({ de: anterior, para: atual });
        atual = anterior;
    }
    return caminho.reverse();
}

async function executarCaminho(caminho, cor, offsetLateral) {
    const fps = 60; 
    const edgesDataSet = network.body.data.edges; 

    for (let passo of caminho) {
        // Encontra a aresta original (independente da direção)
        let arestaDadosOriginal = dadosJSON.edges.find(e => 
            (e.from == passo.de && e.to == passo.para) || 
            (e.from == passo.para && e.to == passo.de)
        );

        if (!arestaDadosOriginal) continue;

        let valorResistencia = parseFloat(arestaDadosOriginal.label.split(',')[0]);
        let duracaoMs = (valorResistencia / 2) * 1000;
        if (duracaoMs < 500) duracaoMs = 500; 

        let animacaoObj = {
            id: Math.random(), 
            noOrigem: passo.de,
            noDestino: passo.para,
            progresso: 0,
            cor: cor,
            offsetLateral: offsetLateral
        };

        animacoesAtivas.push(animacaoObj);

        let framesTotais = (duracaoMs / 1000) * fps;
        
        for (let i = 1; i <= framesTotais; i++) {
            animacaoObj.progresso = i / framesTotais;
            network.redraw(); 
            await sleep(1000 / fps);
        }

        animacoesAtivas = animacoesAtivas.filter(a => a.id !== animacaoObj.id);
        
        // --- CORREÇÃO DA SETA INVERTIDA AQUI ---
        edgesDataSet.update({
            id: arestaDadosOriginal.id,
            color: { color: cor }, 
            width: 4,
            // FORÇA A DIREÇÃO DA ARESTA PARA SEGUIR O FLUXO DA ÁGUA
            from: passo.de,   
            to: passo.para,
            arrows: { to: { enabled: true, scaleFactor: 0.5 } } // Garante que a ponta apareça no final
        });
        network.redraw();
    }
}
function getDestinoUsuario() {
    const inputVal = document.getElementById('input-destino').value;
    
    // Se o usuário não digitou nada, usa o padrão do JSON
    if (inputVal === "") {
        return dadosJSON.destino_escolhido;
    }

    const id = parseInt(inputVal);

    // Verifica se o nó existe no grafo
    const existe = dadosJSON.nodes.some(n => n.id == id);
    if (!existe) {
        alert("O nó " + id + " não existe no grafo!");
        return null;
    }
    
    return id;
}

// --- CORREÇÃO AQUI ---
function resetarMapa() {
    animacoesAtivas = []; // Para animações correntes
    
    // Em vez de network.setData(...), que reseta tudo (inclusive posição dos nós),
    // nós vamos pegar as arestas EXISTENTES e resetar só a cor delas.
    const edgesDataSet = network.body.data.edges;
    
    // Cria uma lista de atualizações para todas as arestas
    const updates = edgesDataSet.get().map(edge => ({
        id: edge.id,
        color: { color: '#cccccc' }, // Volta pra cinza
        width: 2,                    // Volta pra fino
        dashes: false
    }));

    // Aplica as atualizações em massa (sem mexer nos nós!)
    edgesDataSet.update(updates);
    network.redraw();
}

async function animarDijkstra() {
    if (!network) return;
    
    const destino = getDestinoUsuario(); // <--- MUDOU AQUI
    if (destino === null) return;

    resetarMapa();
    let caminho = getCaminho(dadosJSON.pred_dijkstra, destino);
    
    if (caminho.length === 0 && destino != 0) { // Assume 0 como origem
        alert("Não há caminho para este destino!");
        return;
    }

    await executarCaminho(caminho, '#e11d48', 0); 
    alert("Dijkstra chegou!"); 
}

async function animarBFS() {
    if (!network) return;

    const destino = getDestinoUsuario(); // <--- MUDOU AQUI
    if (destino === null) return;

    resetarMapa();
    let caminho = getCaminho(dadosJSON.pred_bfs, destino);

    if (caminho.length === 0 && destino != 0) {
        alert("Não há caminho para este destino!");
        return;
    }

    await executarCaminho(caminho, '#2563eb', 0); 
    alert("BFS chegou!"); 
}

async function comparar() {
    if (!network) return;

    const destino = getDestinoUsuario(); // <--- MUDOU AQUI
    if (destino === null) return;

    resetarMapa();

    let caminhoDijkstra = getCaminho(dadosJSON.pred_dijkstra, destino);
    let caminhoBFS = getCaminho(dadosJSON.pred_bfs, destino);

    await Promise.all([
        executarCaminho(caminhoDijkstra, '#e11d48', -4), 
        executarCaminho(caminhoBFS, '#2563eb', 4)
    ]);

        alert("Corrida Terminou!");

}

iniciar();