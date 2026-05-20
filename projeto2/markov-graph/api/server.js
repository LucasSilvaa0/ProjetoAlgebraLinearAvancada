const express = require("express");
const { spawn } = require("child_process");
const path = require("path");

const app = express();
app.use(express.json());
app.use((req, res, next) => {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Content-Type");
  res.header("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
  if (req.method === "OPTIONS") return res.sendStatus(200);
  next();
});

const runnerPath = path.join(__dirname, process.platform === "win32" ? "mfloat_runner.exe" : "mfloat_runner");

function runMFloat(operation, matrix, t, initial) {
  return new Promise((resolve, reject) => {
    const child = spawn(runnerPath, [], { stdio: ["pipe", "pipe", "pipe"] });
    let stdout = "";
    let stderr = "";

    child.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });

    child.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    child.on("close", (code) => {
      if (code !== 0) {
        return reject(new Error(`mfloat_runner exited with code ${code}: ${stderr}`));
      }
      try {
        return resolve(JSON.parse(stdout));
      } catch (error) {
        return reject(new Error(`Failed to parse mfloat_runner output: ${error.message}\n${stdout}`));
      }
    });

    const initialMode = initial === undefined ? "none" : Array.isArray(initial) ? "vector" : "index";
    const initialPart = initialMode === "none"
      ? ""
      : initialMode === "index"
      ? `${initial}\n`
      : `${initial.join(" ")}\n`;

    const matrixFlat = matrix.flat().join(" ");
    const payload = `${operation}\n${matrix.length} ${t}\n${initialMode}\n${initialPart}${matrixFlat}\n`;
    child.stdin.write(payload);
    child.stdin.end();
  });
}

function validateMatrix(matrix) {
  if (!Array.isArray(matrix) || matrix.length === 0) return false;
  const n = matrix.length;
  return matrix.every(
    (row) => Array.isArray(row) && row.length === n && row.every((v) => typeof v === "number")
  );
}

app.post("/api/markov/distribution", async (req, res) => {
  try {
    const { matrix, t, initial } = req.body;
    if (!validateMatrix(matrix)) return res.status(400).json({ error: "matrix must be a square numeric array" });
    if (typeof t !== "number" || t < 0) return res.status(400).json({ error: "t must be a non-negative number" });

    const result = await runMFloat("distribution", matrix, t, initial);
    res.json(result);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.post("/api/markov/steady", async (req, res) => {
  try {
    const { matrix, iterations } = req.body;
    if (!validateMatrix(matrix)) return res.status(400).json({ error: "matrix must be a square numeric array" });
    if (typeof iterations !== "number" || iterations < 1) return res.status(400).json({ error: "iterations must be a positive number" });

    const result = await runMFloat("steady", matrix, iterations, undefined);
    res.json(result);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

const port = process.env.PORT || 3001;
app.listen(port, () => {
  console.log(`Markov-MFloat API listening on http://localhost:${port}`);
});
