let network;
let dadosJSON;

let animacoesAtivas = []; 

const sleep = ms => new Promise(r => setTimeout(r, ms));

async function iniciar() {
    try {
        let response;
        try { response = await fetch('saida.json'); } 
        catch (e) { response = await fetch('../saida.json'); }
        
        dadosJSON = await response.json();

        let valorFluxo= dadosJSON.max_flow;

        if (valorFluxo !== undefined) {
            document.getElementById('display-max-flow').innerText = valorFluxo;
            document.getElementById('status-panel').style.display = "block"; 
        }

        dadosJSON.nodes = dadosJSON.nodes.map(node => {
            let novoLabel = node.label;
            novoLabel = novoLabel.charAt(0).toUpperCase() + novoLabel.slice(1);
            novoLabel = novoLabel.replace(' (', '\n('); 
            return { ...node, label: novoLabel };
        });

        dadosJSON.edges= dadosJSON.edges.map(edge => {
            const estiloFinal= {
                color: edge.color ? edge.color.color : "#848484",
                width: edge.width || 1
            };
            return{
                ...edge,
                hiddenColor: estiloFinal.color,
                hiddenWidth: estiloFinal.width,
                color: { color: '#cccccc'},
                width: 2
            };
        })

        const container = document.getElementById('network');
        
        const data = {
            nodes: new vis.DataSet(dadosJSON.nodes),
            edges: new vis.DataSet(dadosJSON.edges)
        };

       const options = {
            nodes: {
                shape: 'circle',
                widthConstraint: { minimum: 80, maximum: 80 }, 
                color: {
                    background: '#ffffff', 
                    border: '#111111',     
                    highlight: { background: '#f0f0f0', border: '#000000' },
                    hover: { background: '#ffffff', border: '#000000' }
                },
                font: {
                    multi: true,
                    size: 16,
                    face: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif',
                    color: '#000000',      
                    vadjust: 0,
                    bold: { color: '#000000' } 
                },
                borderWidth: 1.5,          
                shadow: false              
            },
            edges: {
                arrows: { 
                    to: { enabled: true, scaleFactor: 0.5 } 
                },
                color: { 
                    color: '#dddddd',     
                    highlight: '#000000',
                    hover: '#000000',
                    opacity: 1.0
                }, 
                width: 1.5,              
                smooth: { 
                    enabled: true, 
                    type: 'continuous',
                    roundness: 0.4 
                },
                font: {
                    align: 'top',
                    size: 11,
                    face: 'Arial, sans-serif',
                    color: '#666666',
                    strokeWidth: 3,       
                    strokeColor: '#ffffff',
                    background: 'none'     
                },
                shadow: false              
            },
            physics: {
                enabled: true,
                solver: 'forceAtlas2Based',
                forceAtlas2Based: {
                    gravitationalConstant: -120, 
                    centralGravity: 0.005,
                    springLength: 230,     
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
                document.getElementById('input-destino').value = idClicado;
            }
        });

        network.on("afterDrawing", function (ctx) {
            animacoesAtivas.forEach(anim => {
                const posOrigem = network.getPositions([anim.noOrigem])[anim.noOrigem];
                const posDestino = network.getPositions([anim.noDestino])[anim.noDestino];

                if(!posOrigem || !posDestino) return;

                const nodeRadius = 20; 
                const dx = posDestino.x - posOrigem.x;
                const dy = posDestino.y - posOrigem.y;
                const distancia = Math.sqrt(dx*dx + dy*dy);

                const ux = dx / distancia;
                const uy = dy / distancia;

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
        let aresta = dadosJSON.edges.find(e => 
            (e.from == passo.de && e.to == passo.para) || 
            (e.from == passo.para && e.to == passo.de)
        );

        if (!aresta) continue;

        let texto = String(aresta.label);
        
        let numeros = texto.match(/(\d+(\.\d+)?)/); 
        
        if (numeros) {
            valorResistencia = parseFloat(numeros[0]);
        } else {
            console.warn("Não achei número na label:", texto);
        }
        
        console.log("Resistência lida:", valorResistencia); 

        let duracaoMs = (valorResistencia / 4) * 1000;

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
        
        edgesDataSet.update({
            id: aresta.id,
            color: { color: cor }, 
            width: 4,
            from: passo.de,   
            to: passo.para,
            arrows: { to: { enabled: true, scaleFactor: 0.5 } } 
        });
        network.redraw();
    }
}

function getDestinoUsuario() {
    const inputVal = document.getElementById('input-destino').value;
    
    if (inputVal === "") {
        return dadosJSON.destino_escolhido;
    }

    const id = parseInt(inputVal);

    const existe = dadosJSON.nodes.some(n => n.id == id);
    if (!existe) {
        alert("O nó " + id + " não existe no grafo!");
        return null;
    }
    
    return id;
}

function resetarMapa() {
    animacoesAtivas = []; 
    
    const edgesDataSet = network.body.data.edges;
    
    const updates = edgesDataSet.get().map(edge => ({
        id: edge.id,
        color: { color: '#cccccc' }, 
        width: 2,                    
        dashes: false
    }));

    edgesDataSet.update(updates);
    network.redraw();
}

async function animarDijkstra() {
    if (!network) return;
    
    const destino = getDestinoUsuario(); 
    if (destino === null) return;

    resetarMapa();
    let caminho = getCaminho(dadosJSON.pred_dijkstra, destino);
    
    if (caminho.length === 0 && destino != 0) { 
        alert("Não há caminho para este destino!");
        return;
    }

    await executarCaminho(caminho, '#e11d48', 0); 
}

async function animarBFS() {
    if (!network) return;

    const destino = getDestinoUsuario(); 
    if (destino === null) return;

    resetarMapa();
    let caminho = getCaminho(dadosJSON.pred_bfs, destino);

    if (caminho.length === 0 && destino != 0) {
        alert("Não há caminho para este destino!");
        return;
    }

    await executarCaminho(caminho, '#2563eb', 0); 
}

async function comparar() {
    if (!network) return;

    const destino = getDestinoUsuario();
    if (destino === null) return;

    resetarMapa();

    let caminhoDijkstra = getCaminho(dadosJSON.pred_dijkstra, destino);
    let caminhoBFS = getCaminho(dadosJSON.pred_bfs, destino);

    await Promise.all([
        executarCaminho(caminhoDijkstra, '#e11d48', -4), 
        executarCaminho(caminhoBFS, '#2563eb', 4)
    ]);

}

async function animarFluxo() {
    if (!network) return;
    
    let inputDestino = document.getElementById('input-destino').value;

    if (inputDestino !== "") {
        let idDigitado = parseInt(inputDestino);
        let idCalculadoNoC = dadosJSON.destino_escolhido;

        if (idDigitado !== idCalculadoNoC) {
            alert(`A simulação atual foi calculada no C para o Destino ${idCalculadoNoC}.\n\nPara ver o fluxo para o nó ${idDigitado}, você precisa rodar o comando 'vis' ou 'fuk' no terminal C novamente escolhendo este nó.`);
            document.getElementById('input-destino').value = idCalculadoNoC;
        }
    }

    resetarMapa(); 

    const edgesDataSet = network.body.data.edges;
    const allEdges = edgesDataSet.get();
    
    const updates = allEdges.map(edge => {
        return {
            id: edge.id,
            color: { color: edge.hiddenColor },
            width: edge.hiddenWidth
        };
    });
    edgesDataSet.update(updates);

    const fps = 60;
    const duracaoLoop = 2000; 
    
    const arestasComFluxo = allEdges.filter(e => e.hiddenWidth > 1);

    if (arestasComFluxo.length === 0) {
        alert("Nenhum fluxo detectado na rede!");
        return;
    }

    const numParticulas = 3; 
    
    for (let onda = 0; onda < numParticulas; onda++) {
        
        arestasComFluxo.forEach(edge => {
            animacoesAtivas.push({
                id: Math.random(),
                noOrigem: edge.from,
                noDestino: edge.to,
                progresso: 0,
                cor: '#33CCFF', 
                offsetLateral: 0
            });
        });

        let frames = 60; 
        for (let i = 1; i <= frames; i++) {
            animacoesAtivas.forEach(anim => {
                if (anim.progresso < 1) {
                    anim.progresso += 1/frames;
                }
            });
            network.redraw();
            await sleep(1000 / fps);
        }
        animacoesAtivas = animacoesAtivas.filter(a => a.progresso < 1);
    }

    animacoesAtivas = [];
    network.redraw();
    
}

iniciar();