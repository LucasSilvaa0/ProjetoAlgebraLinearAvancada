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

export function steadyState(matrix: number[][], iterations = 100) {
  const n = matrix.length;
  let pi = Array(n).fill(1 / n);

  for (let k = 0; k < iterations; k++) {
    const newPi = Array(n).fill(0);

    // pi is treated as a row vector: pi = pi * P
    for (let i = 0; i < n; i++) {
      for (let j = 0; j < n; j++) {
        newPi[j] += pi[i] * matrix[i][j];
      }
    }

    // normalize to avoid numerical drift
    const sum = newPi.reduce((acc, v) => acc + v, 0) || 1;
    for (let i = 0; i < n; i++) newPi[i] /= sum;

    pi = newPi;
  }

  return pi;
}

/**
 * Calcula a distribuição de probabilidade após `t` passos.
 * - `matrix` pode ser uma matriz de pesos (não normalizada) ou já uma matriz de transição.
 * - `t` é o número de passos (iterações).
 * - `initial` pode ser um índice de nó (number) — vetores base e_i — ou um vetor de probabilidades.
 */
export function distributionAfterT(
  matrix: number[][],
  t: number,
  initial?: number | number[]
) {
  const n = matrix.length;

  // normalizar linhas (aceita matriz de pesos)
  const P = Array.from({ length: n }, () => Array(n).fill(0));
  for (let i = 0; i < n; i++) {
    let rowSum = 0;
    for (let j = 0; j < n; j++) rowSum += matrix[i][j];
    for (let j = 0; j < n; j++) P[i][j] = matrix[i][j] / (rowSum || 1);
  }

  // construir vetor inicial
  let pi: number[];
  if (initial === undefined) {
    pi = Array(n).fill(1 / n);
  } else if (typeof initial === "number") {
    pi = Array(n).fill(0);
    if (initial < 0 || initial >= n) throw new Error("initial index out of range");
    pi[initial] = 1;
  } else {
    if (initial.length !== n) throw new Error("initial vector length mismatch");
    const s = initial.reduce((a, b) => a + b, 0) || 1;
    pi = initial.map((v) => v / s);
  }

  if (t <= 0) return pi;

  for (let step = 0; step < t; step++) {
    const newPi = Array(n).fill(0);
    for (let i = 0; i < n; i++) {
      for (let j = 0; j < n; j++) {
        newPi[j] += pi[i] * P[i][j];
      }
    }
    const sum = newPi.reduce((a, b) => a + b, 0) || 1;
    for (let i = 0; i < n; i++) newPi[i] /= sum;
    pi = newPi;
  }

  return pi;
}
