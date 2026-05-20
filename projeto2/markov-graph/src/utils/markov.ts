import { distributionAfterTWithMFloat, steadyStateWithMFloat } from "./mfloatApi";

export function buildTransitionMatrix(nodes: any[], edges: any[]) {
  const n = nodes.length;
  const indexMap: any = {};

  nodes.forEach((node, i) => {
    indexMap[node.id] = i;
  });

  const matrix = Array.from({ length: n }, () =>
    Array(n).fill(0)
  );

  edges.forEach((edge) => {
    const from = indexMap[edge.source];
    const to = indexMap[edge.target];
    const weight = edge.data.weight;

    matrix[from][to] += weight;
  });

  // normalizar linhas
  for (let i = 0; i < n; i++) {
    let sum = 0;
    for (let j = 0; j < n; j++) sum += matrix[i][j];

    for (let j = 0; j < n; j++) {
      matrix[i][j] /= sum || 1;
    }
  }

  return matrix;
}

export { distributionAfterTWithMFloat, steadyStateWithMFloat };

/**
 * Calcula a distribuição de probabilidade após `t` passos usando o backend C++.
 * - `matrix` pode ser uma matriz de pesos (não normalizada) ou já uma matriz de transição.
 * - `t` é o número de passos (iterações).
 * - `initial` pode ser um índice de nó (number) — vetores base e_i — ou um vetor de probabilidades.
 */
export async function distributionAfterT(
  matrix: number[][],
  t: number,
  initial?: number | number[],
) {
  return distributionAfterTWithMFloat(matrix, t, initial);
}

export async function steadyState(matrix: number[][], iterations = 100) {
  return steadyStateWithMFloat(matrix, iterations);
}
