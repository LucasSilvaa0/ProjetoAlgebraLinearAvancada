const API_BASE = ((import.meta as any).env?.VITE_MFLOAT_API_URL as string) ?? "";

type DistributionResponse = {
  distribution: number[];
};

type DistributionRequest = {
  matrix: number[][];
  t: number;
  initial?: number | number[];
};

type SteadyStateRequest = {
  matrix: number[][];
  iterations: number;
};

async function postJson<T>(path: string, body: object): Promise<T> {
  const url = `${API_BASE}${path}`;
  const response = await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });

  const responseText = await response.clone().text();
  console.log("[mfloatApi] response.body:", responseText);

  if (!response.ok) {
    throw new Error(`API request failed (${response.status}): ${responseText}`);
  }

  return response.json();
}

export async function distributionAfterTWithMFloat(
  matrix: number[][],
  t: number,
  initial?: number | number[],
): Promise<number[]> {
  const result = await postJson<DistributionResponse>("/api/markov/distribution", {
    matrix,
    t,
    initial,
  });
  return result.distribution;
}

export async function steadyStateWithMFloat(
  matrix: number[][],
  iterations = 100,
): Promise<number[]> {
  const result = await postJson<DistributionResponse>("/api/markov/steady", {
    matrix,
    iterations,
  });
  return result.distribution;
}
