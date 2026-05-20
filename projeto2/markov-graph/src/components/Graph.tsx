import React, { useCallback, useState } from 'react';
import ReactFlow, {
  addEdge,
  Edge,
  Connection,
  MiniMap,
  Controls,
  Background,
  useNodesState,
  useEdgesState,
  Node,
} from 'reactflow';
import { buildTransitionMatrix, steadyState, distributionAfterT } from '../utils/markov';

import 'reactflow/dist/style.css';

const initialNodes: Node[] = [
  { id: '1', position: { x: 100, y: 100 }, data: { label: '1' } },
  { id: '2', position: { x: 300, y: 200 }, data: { label: '2' } },
];

const initialEdges: Edge[] = [];

export default function Graph() {
  const [nodes, setNodes, onNodesChange] = useNodesState(initialNodes);
  const [edges, setEdges, onEdgesChange] = useEdgesState(initialEdges);
  const [isDialogOpen, setIsDialogOpen] = useState(false);
  const [matrizAtual, setMatrizAtual] = useState<number[][]>([]);
  const [initialNodeIndex, setInitialNodeIndex] = useState<number>(0);
  const [steps, setSteps] = useState<number>(1);
  const [distribution, setDistribution] = useState<number[] | null>(null);

  const onEdgeClick = (event: any, edge: Edge) => {
    const newWeight = prompt('Digite o novo peso:', edge.data?.weight || '1');

    if (newWeight === null) return;

    const parsed = parseFloat(newWeight);

    if (isNaN(parsed) || parsed < 0) {
        alert('Peso inválido!');
        return;
    }

    setEdges((eds) =>
        eds.map((e) =>
        e.source === edge.source && e.target == edge.target
            ? {
                ...e,
                data: { ...e.data, weight: parsed },
                label: parsed.toString(),
            }
            : e
        )
    );
    
    setEdges((eds) =>
        eds.map((e) =>
        e.target === edge.source && e.source == edge.target
            ? {
                ...e,
                data: { ...e.data, weight: parsed },
                label: parsed.toString(),
            }
            : e
        )
    );

    let matriz = buildTransitionMatrix(nodes, edges);
    setMatrizAtual(matriz);
  };

  // 🔁 conectar nós (com aresta dupla automática)
  const onConnect = useCallback((params: Connection) => {
    setEdges((eds) => {
      const forward: Edge = {
        id: `${params.source}-${params.target}`,
        source: params.source!,
        target: params.target!,
        data: { weight: 1 },
        label: '1',
      };

      const backward: Edge = {
        id: `${params.target}-${params.source}`,
        source: params.target!,
        target: params.source!,
        data: { weight: 1 },
        label: '1',
      };

      return addEdge(backward, addEdge(forward, eds));
    });
  }, [setEdges]);

  // ➕ adicionar nó com laço automático
  const addNode = () => {
    const id = (nodes.length + 1).toString();

    const newNode: Node = {
      id,
      position: {
        x: Math.random() * 400,
        y: Math.random() * 400,
      },
      data: { label: id },
    };

    setNodes((nds) => [...nds, newNode]);
  };

  const openCalculateBox = () => {
    let matriz = buildTransitionMatrix(nodes, edges);
    setMatrizAtual(matriz);
    // reset inputs
    setInitialNodeIndex(nodes.length > 0 ? 0 : 0);
    setSteps(1);
    setDistribution(null);
    setIsDialogOpen(true);
  };

  const computeDistribution = () => {
    const matriz = matrizAtual.length > 0 ? matrizAtual : buildTransitionMatrix(nodes, edges);
    const t = Math.max(0, Math.floor(Number(steps) || 0));

    if (t === 0) {
      // return initial distribution
      const init = Array(nodes.length).fill(0);
      if (initialNodeIndex >= 0 && initialNodeIndex < nodes.length) init[initialNodeIndex] = 1;
      setDistribution(init);
      return;
    }

    const result = distributionAfterT(matriz, t, initialNodeIndex);
    setDistribution(result);
  };

  return (
    <div style={{ width: '100%', height: '100vh' }}>
      {isDialogOpen && (
        <div
          style={{
            position: 'fixed',
            top: 0,
            left: 0,
            width: '100%',
            height: '100%',
            backgroundColor: 'rgba(0, 0, 0, 0.5)',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            zIndex: 1000,
          }}
        >
          <div
            style={{
              backgroundColor: 'white',
              padding: '30px',
              borderRadius: '8px',
              boxShadow: '0 4px 6px rgba(0, 0, 0, 0.1)',
              maxWidth: '500px',
              height: 'auto',
              width: '90%',
            }}
          >
            <h2 style={{color:"black"}}>Calcular Distribuição</h2>

            <div style={{ display: 'flex', gap: '12px', marginBottom: '12px', alignItems: 'center' }}>
              <label style={{ display: 'flex', flexDirection: 'column' }}>
                Nó inicial
                <select
                  value={initialNodeIndex}
                  onChange={(e) => setInitialNodeIndex(Number(e.target.value))}
                  style={{ marginTop: 6 }}
                >
                  {nodes.map((n, i) => (
                    <option key={n.id} value={i}>{`${n.data?.label ?? n.id} (idx ${i})`}</option>
                  ))}
                </select>
              </label>

              <label style={{ display: 'flex', flexDirection: 'column' }}>
                Passos (t)
                <input
                  type="number"
                  min={0}
                  value={steps}
                  onChange={(e) => setSteps(Number(e.target.value))}
                  style={{ width: 100, marginTop: 6 }}
                />
              </label>

              <button
                onClick={computeDistribution}
                style={{ padding: '8px 12px', marginTop: 18 }}
              >
                Calcular
              </button>
            </div>

            <div style={{
              maxHeight: '140px',
              overflow: 'auto',
              background: '#f7f7f7',
              padding: '10px',
              borderRadius: '4px',
              marginBottom: '8px'
            }}>
              <strong>Matriz de transição:</strong>
              {matrizAtual.length === 0 ? (
                <div>Sem matriz</div>
              ) : (
                matrizAtual.map((row, i) => (
                  <div key={i}>{row.map((v) => Number(v).toFixed(3)).join(', ')}</div>
                ))
              )}
            </div>

            <div style={{ marginBottom: '12px' }}>
              <strong>Probabilidade de estado após {steps} passo(s):</strong>
              {distribution ? (
                <div>
                  {distribution.map((v, i) => (
                    <div key={i}>{`nó ${i+1}: ${Number(v).toFixed(3)}`}</div>
                  ))}
                </div>
              ) : (
                <div>Nenhuma distribuição calculada</div>
              )}
            </div>

            <button
              onClick={() => setIsDialogOpen(false)}
              style={{
                padding: '10px 20px',
                backgroundColor: '#007bff',
                color: 'white',
                border: 'none',
                borderRadius: '4px',
                cursor: 'pointer',
              }}
            >
              Fechar
            </button>
          </div>
        </div>
      )}

      <button
        onClick={addNode}
        style={{
          position: 'absolute',
          left: 10,
          top: 10,
          zIndex: 4,
        }}
      >
        Adicionar vértice
      </button>

      <button
        onClick={openCalculateBox}
        style={{
          position: 'absolute',
          right: 10,
          top: 10,
          zIndex: 4,
        }}
      >
        Calcular estado estacionário
      </button>

      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        onEdgeClick={onEdgeClick}
      >
        <MiniMap />
        <Controls />
        <Background />
      </ReactFlow>
    </div>
  );
}