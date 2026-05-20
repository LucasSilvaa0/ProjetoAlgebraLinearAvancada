import { useEffect, useState } from 'react';
import createMFloatModule from './mfloat/mfloat.js';

// --- HELPER FUNCTIONS FOR MEMORY MANAGEMENT ---
export function freeMatrix(matrix: any[][]) {
  for (let i = 0; i < matrix.length; i++) {
    for (let j = 0; j < matrix[i].length; j++) {
      if (matrix[i][j]) matrix[i][j].delete();
    }
  }
}

export function freeVector(vector: any[]) {
  for (let i = 0; i < vector.length; i++) {
    if (vector[i]) vector[i].delete();
  }
}


export function extractLinearSystem(Module: any, matrix: number[][]) {
  const n = matrix.length;
  if (n === 0) return { A: [], b: [] };

  const P = Array.from({ length: n }, () => Array(n).fill(null));

  for (let i = 0; i < n; i++) {
    let rowSum = new Module.MFloat(0);
    for (let j = 0; j < n; j++) {
      const val = new Module.MFloat(matrix[i][j]);
      const nextSum = rowSum.add(val);
      
      rowSum.delete(); // Free previous sum
      val.delete();
      rowSum = nextSum;
    }

    const isZero = rowSum.toDouble() === 0;

    for (let j = 0; j < n; j++) {
      const val = new Module.MFloat(matrix[i][j]);
      if (isZero) {
        P[i][j] = val; // Store directly, no need to delete
      } else {
        P[i][j] = val.div(rowSum);
        val.delete();
      }
    }
    rowSum.delete();
  }

  const A = Array.from({ length: n }, () => Array(n).fill(null));
  const one = new Module.MFloat(1);

  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      if (i === j) {
        A[i][j] = P[j][i].sub(one);
      } else {
        // Create a copy by doing P + 0, so we can safely delete P later
        const zero = new Module.MFloat(0);
        A[i][j] = P[j][i].add(zero); 
        zero.delete();
      }
    }
  }

  one.delete();
  freeMatrix(P); // Free the intermediate P matrix

  const b = Array.from({ length: n }, () => new Module.MFloat(0));
  const lastIndex = n - 1;

  for (let j = 0; j < n; j++) {
    A[lastIndex][j].delete(); // free old value
    A[lastIndex][j] = new Module.MFloat(1);
  }
  
  b[lastIndex].delete();
  b[lastIndex] = new Module.MFloat(1);

  return { A, b };
}

export function solveJacobi(Module: any, A: any[][], b: any[], iterations = 100) {
  const n = A.length;
  // Initialize x array
  let x = Array.from({ length: n }, () => new Module.MFloat(1 / n));

  for (let k = 0; k < iterations; k++) {
    const xNew = Array(n).fill(null);

    for (let i = 0; i < n; i++) {
      let sum = new Module.MFloat(0);
      
      for (let j = 0; j < n; j++) {
        if (i !== j) {
          const prod = A[i][j].mul(x[j]);
          const nextSum = sum.add(prod);
          
          sum.delete();  // cleanup intermediate
          prod.delete(); // cleanup intermediate
          sum = nextSum;
        }
      }

      const bMinusSum = b[i].sub(sum);
      xNew[i] = bMinusSum.div(A[i][i]);
      
      bMinusSum.delete();
      sum.delete();
    }

    freeVector(x); // Free the old 'x' vector before replacing it
    x = xNew;
  }

  return x;
}

export function solveGaussSeidel(Module: any, A: any[][], b: any[], iterations = 100) {
  const n = A.length;
  const x = Array.from({ length: n }, () => new Module.MFloat(1 / n));

  for (let k = 0; k < iterations; k++) {
    for (let i = 0; i < n; i++) {
      let sum = new Module.MFloat(0);
      
      for (let j = 0; j < n; j++) {
        if (i !== j) {
          const prod = A[i][j].mul(x[j]);
          const nextSum = sum.add(prod);
          
          sum.delete();
          prod.delete();
          sum = nextSum;
        }
      }

      const bMinusSum = b[i].sub(sum);
      const newValue = bMinusSum.div(A[i][i]);

      x[i].delete(); // Free the old specific value in x
      x[i] = newValue; // Assign the new calculated value

      bMinusSum.delete();
      sum.delete();
    }
  }

  return x;
}

// --- REACT COMPONENT WRAPPER ---

export default function FloatCalculator() {
  const [resultStr, setResultStr] = useState<string[]>([]);

  useEffect(() => {
    async function runMath() {
      const Module = await createMFloatModule();

      // Dummy transition matrix representing graph edges
      const graphMatrix = [
        [0,   1,   0  ],
        [0.5, 0,   0.5],
        [0,   0.5, 0.5]
      ];

      // 1. Build Linear System (Wasm Arrays)
      const { A, b } = extractLinearSystem(Module, graphMatrix);

      // 2. Solve using Gauss-Seidel (Wasm Arrays)
      const x = solveGaussSeidel(Module, A, b, 50);

      // 3. Read Wasm objects back into React state
      const resultsFormatted = x.map((val, idx) => `Node ${idx + 1}: ${val.toString()}`);
      setResultStr(resultsFormatted);
      console.log("As Doubles:", x.map(val => val.toDouble()));

      // 4. CRITICAL: Clean up all matrices and vectors
      freeMatrix(A);
      freeVector(b);
      freeVector(x);
    }

    runMath();
  }, []);

  return (
    <div style={{ padding: 20 }}>
      <h2>MFloat WebAssembly Steady State:</h2>
      {resultStr.map((res, i) => (
        <div key={i}><code>{res}</code></div>
      ))}
    </div>
  );
}
