

/* =========================================================================
DADOS DO PROBLEMA — O PROBLEMA DA DIETA
6 alimentos (variáveis de decisão) + 4 restrições (variáveis de folga)
= 10 variáveis, atendendo ao mínimo exigido pelo projeto.
========================================================================= */

const foods = [
    { name: "Arroz", unit: "porção (100g)", c: 5 },
    { name: "Feijão", unit: "porção (100g)", c: 4 },
    { name: "Frango", unit: "porção (100g)", c: 3 },
    { name: "Ovo", unit: "unidade", c: 6 },
    { name: "Banana", unit: "unidade", c: 4 },
    { name: "Leite", unit: "copo (200ml)", c: 2 },
];

// Coeficientes das restrições (A) e limites (b)
const constraints = [
    {
        label: "Orçamento diário",
        unit: "R$",
        coefs: [2, 3, 1, 4, 2, 1],
        b: 40,
    },
    {
        label: "Peso total transportável",
        unit: "kg",
        coefs: [1, 1, 2, 1, 3, 2],
        b: 30,
    },
    {
        label: "Tempo total de preparo",
        unit: "min",
        coefs: [3, 1, 1, 2, 1, 4],
        b: 36,
    },
    {
        label: "Variedade máxima/refeição",
        unit: "pts",
        coefs: [1, 2, 3, 1, 2, 1],
        b: 28,
    },
];

const n = foods.length; // 6 variáveis de decisão
const m = constraints.length; // 4 restrições -> 4 folgas
// total de variáveis (sem contar Z) = n + m = 10

/* =========================================================================
MOTOR SIMPLEX GENÉRICO (forma padrão, maximização, todas ≤)
Gera uma lista de "frames" (quadros) para a animação passo-a-passo.
========================================================================= */

function buildInitialTableau() {
    const cols = n + m + 1; // vars + folgas + RHS
    const rows = m + 1; // linha Z + m restrições
    const T = Array.from({ length: rows }, () => Array(cols).fill(0));
    for (let j = 0; j < n; j++) T[0][j] = -foods[j].c;
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) T[i + 1][j] = constraints[i].coefs[j];
        T[i + 1][n + i] = 1;
        T[i + 1][cols - 1] = constraints[i].b;
    }
    const basis = Array.from({ length: m }, (_, i) => n + i);
    return { T, basis };
}

function varName(idx) {
    return idx < n ? "x" + (idx + 1) : "S" + (idx - n + 1);
}

function cloneT(T) {
    return T.map((r) => r.slice());
}

function runSimplex() {
    let { T, basis } = buildInitialTableau();
    const frames = [];
    const cols = n + m + 1;

    frames.push({
        type: "initial",
        T: cloneT(T),
        basis: basis.slice(),
        note: "Tableau inicial: a base contém as 4 variáveis de folga (S₁…S₄). Cada Sᵢ absorve a diferença entre o limite do recurso e o quanto está sendo usado.",
    });

    let guard = 0;
    while (guard++ < 30) {
        const zrow = T[0];
        let entering = -1,
            best = -1e-9;
        for (let j = 0; j < n + m; j++) {
            if (zrow[j] < best) {
                best = zrow[j];
                entering = j;
            }
        }
        if (entering === -1) break; // ótimo

        // teste da razão
        let leaving = -1,
            bestRatio = Infinity;
        const ratios = [];
        for (let i = 1; i <= m; i++) {
            const coef = T[i][entering];
            if (coef > 1e-9) {
                const ratio = T[i][cols - 1] / coef;
                ratios[i] = ratio;
                if (ratio < bestRatio - 1e-9) {
                    bestRatio = ratio;
                    leaving = i;
                }
            } else {
                ratios[i] = null;
            }
        }
        if (leaving === -1) {
            frames.push({
                type: "unbounded",
                T: cloneT(T),
                basis: basis.slice(),
                entering,
            });
            break;
        }

        // frame de "revelação": mostra quem entra/sai ANTES de pivotear
        frames.push({
            type: "reveal",
            T: cloneT(T),
            basis: basis.slice(),
            entering,
            leaving,
            ratios: ratios.slice(),
            note: `A variável <span class="enter-tag">${varName(entering)}</span> entra na base (maior coeficiente negativo na linha Z) e a variável <span class="leave-tag">${varName(basis[leaving - 1])}</span> sai (menor razão não-negativa na coluna de Interseção).`,
        });

        // pivoteamento
        const pivot = T[leaving][entering];
        T[leaving] = T[leaving].map((v) => v / pivot);
        for (let i = 0; i <= m; i++) {
            if (i === leaving) continue;
            const factor = T[i][entering];
            if (Math.abs(factor) > 1e-12) {
                T[i] = T[i].map((v, k) => v - factor * T[leaving][k]);
            }
        }
        basis[leaving - 1] = entering;

        frames.push({
            type: "pivot",
            T: cloneT(T),
            basis: basis.slice(),
            entering,
            leaving,
            note: `Pivoteamento concluído: ${varName(entering)} assume a linha antes ocupada por ${varName(basis[leaving - 1]) === varName(entering) ? "" : ""}${varName(entering)} — a base foi atualizada e um novo tableau equivalente foi obtido.`,
        });
    }

    const zrowFinal = T[0];
    const optimal = zrowFinal.slice(0, n + m).every((v) => v >= -1e-6);
    frames.push({
        type: "optimal",
        T: cloneT(T),
        basis: basis.slice(),
        note: "Nenhum coeficiente negativo restante na linha Z: a solução é ótima. Fim do passo-a-passo.",
    });

    return frames;
}

let frames = runSimplex();

// cópia dos valores padrão, para o botão "restaurar padrão"
const DEFAULT_FOODS = foods.map((f) => ({ ...f }));
const DEFAULT_CONSTRAINTS = constraints.map((c) => ({
    ...c,
    coefs: c.coefs.slice(),
}));

/* =========================================================================
RENDERIZAÇÃO
========================================================================= */

function fmt(v) {
    if (Math.abs(v) < 1e-9) return "0";
    const r = Math.round(v * 1000) / 1000;
    if (Number.isInteger(r)) return String(r).replace("-", "−");
    return r.toFixed(2).replace(".", ",").replace("-", "−");
}

// Constrói a tabela de edição: uma linha por alimento, uma coluna "Valor
// nutricional" (coeficiente na função-objetivo) + uma coluna por restrição
// (quanto aquele alimento consome daquele recurso).
function buildEditTable() {
    const ft = document.getElementById("foodsTable");
    let head = `<tr><th>Alimento</th><th class="num">Var.</th><th class="num">Valor<br>nutricional</th>`;
    constraints.forEach(
        (c, k) =>
            (head += `<th class="num">${c.label.split(" ")[0]}<br><span style="font-weight:400">(${c.unit})</span></th>`),
    );
    head += `</tr>`;

    let rows = "";
    foods.forEach((f, i) => {
        rows += `<tr><td class="foodname">${f.name}<span style="display:block;color:#9a9d8d;font-size:10.5px">${f.unit}</span></td>`;
        rows += `<td class="num">x${i + 1}</td>`;
        rows += `<td class="num"><input type="number" step="any" min="0" class="nutri-input" data-role="c" data-i="${i}" value="${f.c}"></td>`;
        constraints.forEach((c, k) => {
            rows += `<td class="num"><input type="number" step="any" min="0" data-role="coef" data-i="${i}" data-k="${k}" value="${c.coefs[i]}"></td>`;
        });
        rows += `</tr>`;
    });
    ft.innerHTML = head + rows;

    // limites (b) de cada restrição
    const lg = document.getElementById("limitsGrid");
    lg.innerHTML = constraints
        .map(
            (c, k) =>
                `<span class="llabel">${c.label} (S${k + 1}) ≤</span><input type="number" step="any" min="0" data-role="b" data-k="${k}" value="${c.b}">`,
        )
        .join("");
}

function renderFormulas() {
    document.getElementById("objectiveBox").innerHTML =
        "max Z = " + foods.map((f, i) => `${f.c}x${i + 1}`).join(" + ");

    const cl = document.getElementById("constraintList");
    cl.innerHTML = constraints
        .map((c, i) => {
            const expr = c.coefs.map((v, j) => `${v}x${j + 1}`).join(" + ");
            return `<li><span class="cnum">${i + 1}</span><span><b>${c.label}</b><span class="label">${expr} ≤ ${c.b} ${c.unit} → +S${i + 1}</span></span></li>`;
        })
        .join("");
}

function renderSidebar() {
    buildEditTable();
    renderFormulas();
}

// Lê os valores digitados nos inputs de volta para os arrays foods/constraints
function readEditorValues() {
    const msg = document.getElementById("editMsg");
    const inputs = document.querySelectorAll(
        "#foodsTable input, #limitsGrid input",
    );
    for (const inp of inputs) {
        const v = parseFloat(inp.value.replace(",", "."));
        if (inp.value.trim() === "" || isNaN(v) || v < 0) {
            msg.textContent =
                "Preencha todos os campos com números válidos (≥ 0).";
            msg.className = "edit-msg";
            inp.focus();
            return false;
        }
    }
    inputs.forEach((inp) => {
        const role = inp.dataset.role;
        const v = parseFloat(inp.value.replace(",", "."));
        if (role === "c") {
            foods[Number(inp.dataset.i)].c = v;
        } else if (role === "coef") {
            constraints[Number(inp.dataset.k)].coefs[Number(inp.dataset.i)] = v;
        } else if (role === "b") {
            constraints[Number(inp.dataset.k)].b = v;
        }
    });
    msg.textContent = "Valores atualizados — Simplex recalculado.";
    msg.className = "edit-msg ok";
    return true;
}

function recalcAndRerun() {
    if (!readEditorValues()) return;
    stopPlay();
    frames = runSimplex();
    current = 0;
    renderFormulas();
    render();
}

function resetToDefault() {
    stopPlay();
    DEFAULT_FOODS.forEach((f, i) => (foods[i].c = f.c));
    DEFAULT_CONSTRAINTS.forEach((c, k) => {
        constraints[k].b = c.b;
        c.coefs.forEach((v, i) => (constraints[k].coefs[i] = v));
    });
    buildEditTable();
    frames = runSimplex();
    current = 0;
    renderFormulas();
    render();
    const msg = document.getElementById("editMsg");
    msg.textContent = "Valores padrão restaurados.";
    msg.className = "edit-msg ok";
}

function renderBreadcrumb(current) {
    const bc = document.getElementById("breadcrumb");
    bc.innerHTML = "";
    frames.forEach((f, i) => {
        const b = document.createElement("div");
        b.className =
            "crumb" + (i === current ? " active" : i < current ? " past" : "");
        b.textContent = i + 1;
        b.title = f.type;
        b.addEventListener("click", () => goTo(i));
        bc.appendChild(b);
    });
}

function renderTableau(frame) {
    const table = document.getElementById("tableauTable");
    const cols = n + m + 1;
    let thead = `<thead><tr><th class="rowlabel">Básicas</th><th>Z</th>`;
    for (let j = 0; j < n; j++)
        thead += `<th class="${frame.entering === j ? "col-enter" : ""}">x${j + 1}</th>`;
    for (let j = 0; j < m; j++)
        thead += `<th class="${frame.entering === n + j ? "col-enter" : ""}">S${j + 1}</th>`;
    thead += `<th>Solução</th><th>Interseção</th></tr></thead>`;

    let tbody = "<tbody>";
    // Z row
    {
        const row = frame.T[0];
        tbody += `<tr class="zrow"><td class="rowlabel">Z</td><td>1</td>`;
        for (let j = 0; j < n + m; j++) {
            tbody += `<td class="${frame.entering === j ? "col-enter" : ""}">${fmt(row[j])}</td>`;
        }
        tbody += `<td>${fmt(row[cols - 1])}</td><td class="dim">—</td></tr>`;
    }
    // constraint rows
    for (let i = 1; i <= m; i++) {
        const row = frame.T[i];
        const isLeave = frame.leaving === i;
        tbody += `<tr class="${isLeave ? "row-leave" : ""}"><td class="rowlabel">${varName(frame.basis[i - 1])}</td><td>0</td>`;
        for (let j = 0; j < n + m; j++) {
            const isPivot = isLeave && frame.entering === j;
            const classes = [
                frame.entering === j ? "col-enter" : "",
                isPivot ? "pivot-cell" : "",
            ]
                .filter(Boolean)
                .join(" ");
            tbody += `<td class="${classes}">${fmt(row[j])}</td>`;
        }
        tbody += `<td>${fmt(row[cols - 1])}</td>`;
        // intersection column
        let cell = `<td class="dim">—</td>`;
        if (
            frame.type === "reveal" &&
            frame.ratios &&
            frame.ratios[i] != null
        ) {
            const isBest = frame.leaving === i;
            cell = `<td class="intersect ${isBest ? "best" : ""}">${fmt(frame.ratios[i])}</td>`;
        }
        tbody += cell + `</tr>`;
    }
    tbody += "</tbody>";
    table.innerHTML = thead + tbody;
}

function renderStatus(frame) {
    const pill = document.getElementById("statusPill");
    if (frame.type === "reveal") {
        pill.textContent = "destacando pivô";
        pill.className = "status-pill reveal";
    } else if (frame.type === "pivot") {
        pill.textContent = "após pivoteamento";
        pill.className = "status-pill pivot";
    } else if (frame.type === "optimal") {
        pill.textContent = "solução ótima";
        pill.className = "status-pill done";
    } else if (frame.type === "unbounded") {
        pill.textContent = "ilimitado";
        pill.className = "status-pill done";
    } else {
        pill.textContent = "tableau inicial";
        pill.className = "status-pill reveal";
    }
}

function renderResult(frame) {
    const panel = document.getElementById("resultPanel");
    if (frame.type !== "optimal") {
        panel.classList.remove("show");
        return;
    }
    panel.classList.add("show");
    const cols = n + m + 1;
    const sol = Array(n + m).fill(0);
    for (let i = 0; i < m; i++)
        sol[frame.basis[i]] = frame.T[i + 1][cols - 1];
    const Z = frame.T[0][cols - 1];

    document.getElementById("resultText").textContent =
        `A combinação de porções abaixo maximiza o índice de valor nutricional respeitando orçamento, peso, tempo de preparo e variedade máxima — sem violar nenhuma das 4 restrições.`;

    const grid = document.getElementById("resultGrid");
    let html = `<div class="result-tile"><div class="k">Z ótimo</div><div class="v">${fmt(Z)}</div></div>`;
    foods.forEach((f, i) => {
        html += `<div class="result-tile"><div class="k">${f.name}</div><div class="v">${fmt(sol[i])}</div></div>`;
    });
    grid.innerHTML = html;
}

/* =========================================================================
MÁQUINA DE ESTADOS / CONTROLES
========================================================================= */

let current = 0;
let playing = false;
let playTimer = null;

function goTo(i) {
    current = Math.max(0, Math.min(frames.length - 1, i));
    render();
}

function render() {
    const frame = frames[current];
    document.getElementById("progressLabel").textContent =
        `Passo ${current + 1} de ${frames.length}`;
    document.getElementById("stepNote").innerHTML = frame.note || "";
    renderStatus(frame);
    renderTableau(frame);
    renderBreadcrumb(current);
    renderResult(frame);

    document.getElementById("btnFirst").disabled = current === 0;
    document.getElementById("btnPrev").disabled = current === 0;
    document.getElementById("btnNext").disabled =
        current === frames.length - 1;
    document.getElementById("btnLast").disabled =
        current === frames.length - 1;
    if (current === frames.length - 1) stopPlay();
}

function next() {
    if (current < frames.length - 1) {
        current++;
        render();
    } else stopPlay();
}
function prev() {
    if (current > 0) {
        current--;
        render();
    }
}

function startPlay() {
    playing = true;
    document.getElementById("btnPlay").textContent = "⏸ pausar";
    const speed = 6 - Number(document.getElementById("speedRange").value); // 1..5 -> maior=mais rápido
    playTimer = setInterval(() => {
        if (current >= frames.length - 1) {
            stopPlay();
            return;
        }
        next();
    }, speed * 700);
}
function stopPlay() {
    playing = false;
    document.getElementById("btnPlay").textContent = "▶ reproduzir";
    if (playTimer) clearInterval(playTimer);
    playTimer = null;
}
function togglePlay() {
    playing ? stopPlay() : startPlay();
}

document.getElementById("btnNext").addEventListener("click", () => {
    stopPlay();
    next();
});
document.getElementById("btnPrev").addEventListener("click", () => {
    stopPlay();
    prev();
});
document.getElementById("btnFirst").addEventListener("click", () => {
    stopPlay();
    goTo(0);
});
document.getElementById("btnLast").addEventListener("click", () => {
    stopPlay();
    goTo(frames.length - 1);
});
document.getElementById("btnPlay").addEventListener("click", togglePlay);
document.getElementById("speedRange").addEventListener("input", () => {
    if (playing) {
        stopPlay();
        startPlay();
    }
});

window.addEventListener("keydown", (e) => {
    if (e.key === "ArrowRight") {
        e.preventDefault();
        stopPlay();
        next();
    } else if (e.key === "ArrowLeft") {
        e.preventDefault();
        stopPlay();
        prev();
    } else if (e.code === "Space") {
        e.preventDefault();
        togglePlay();
    }
});

document
    .getElementById("btnRecalc")
    .addEventListener("click", recalcAndRerun);
document
    .getElementById("btnResetData")
    .addEventListener("click", resetToDefault);

renderSidebar();
render();