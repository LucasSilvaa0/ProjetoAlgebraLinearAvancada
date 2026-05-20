# Markov Graph API

Esta API conecta o código de `projeto2/markov-graph/src/utils/markov.ts` ao `mfloat.cpp` de `projeto1`.

## Como usar

1. Abra um terminal em `projeto2/markov-graph/api`
2. Execute `npm install`
3. Compile o runner C++ com `npm run build`
4. Inicie a API com `npm start`

A API irá ficar disponível em `http://localhost:3001`.

## Endpoints

- `POST /api/markov/distribution`
  - body: `{ matrix: number[][], t: number, initial?: number | number[] }`
  - retorna: `{ distribution: number[] }`

- `POST /api/markov/steady`
  - body: `{ matrix: number[][], iterations: number }`
  - retorna: `{ distribution: number[] }`

## Observações

- O backend usa `mfloat.cpp` para normalizar e calcular com precisão a distribuição de Markov.
- O frontend React pode consumir esses endpoints com o wrapper `src/utils/mfloatApi.ts`.
