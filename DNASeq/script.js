/**
 * ============================================================================
 * Project: DNASeq — DNA Sequence Comparison Using Dynamic Programming
 * File:    script.js
 * Purpose: Vanilla JavaScript frontend interaction and data visualization.
 * Note:    Does NOT calculate or recompute LCS in JS. Relies exclusively
 *          on the compiled C binary response served by the Flask backend.
 * ============================================================================
 */

document.addEventListener("DOMContentLoaded", () => {
    // DOM Elements
    const form = document.getElementById("comparison-form");
    const seq1Input = document.getElementById("seq1");
    const seq2Input = document.getElementById("seq2");
    const seq1Count = document.getElementById("seq1-count");
    const seq2Count = document.getElementById("seq2-count");
    const seq1Feedback = document.getElementById("seq1-feedback");
    const seq2Feedback = document.getElementById("seq2-feedback");
    const allowGeneralToggle = document.getElementById("allow-general-toggle");
    const compareBtn = document.getElementById("compare-btn");
    const compareSpinner = document.getElementById("compare-spinner");
    const loadExampleBtn = document.getElementById("load-example-btn");
    const clearBtn = document.getElementById("clear-btn");
    const generalAlert = document.getElementById("general-alert");

    // Example Chips
    const exStandardBtn = document.getElementById("example-standard-btn");
    const exRepeatedBtn = document.getElementById("example-repeated-btn");
    const exTextbookBtn = document.getElementById("example-textbook-btn");
    const exDifferentBtn = document.getElementById("example-different-btn");

    // Results Dashboard Sections
    const resultsDashboard = document.getElementById("results-dashboard");
    const resLcsText = document.getElementById("res-lcs-text");
    const resLcsLen = document.getElementById("res-lcs-len");
    const resLen1 = document.getElementById("res-len1");
    const resLen2 = document.getElementById("res-len2");
    const resOverlapScore = document.getElementById("res-overlap-score");
    const resOverlapProgress = document.getElementById("res-overlap-progress");
    const resTimeComp = document.getElementById("res-time-comp");
    const resSpaceComp = document.getElementById("res-space-comp");

    // Visualization Section
    const visualizationSection = document.getElementById("visualization");
    const visSeq1Meta = document.getElementById("vis-seq1-meta");
    const visSeq2Meta = document.getElementById("vis-seq2-meta");
    const seq1Ribbon = document.getElementById("seq1-ribbon");
    const seq2Ribbon = document.getElementById("seq2-ribbon");

    // DP Matrix Section
    const dpMatrixSection = document.getElementById("dp-matrix-section");
    const matrixWrapper = document.getElementById("matrix-wrapper");

    const MAX_LENGTH = 500;
    const DNA_REGEX = /^[ATGC]*$/;
    const GENERAL_REGEX = /^[A-Z]*$/;

    /**
     * Validates an individual sequence input element.
     * Returns true if valid, false otherwise.
     */
    function validateInput(inputEl, feedbackEl) {
        // Uppercase conversion in place
        const originalVal = inputEl.value;
        const upperVal = originalVal.toUpperCase();
        if (originalVal !== upperVal) {
            const start = inputEl.selectionStart;
            const end = inputEl.selectionEnd;
            inputEl.value = upperVal;
            inputEl.setSelectionRange(start, end);
        }

        const value = inputEl.value.trim();
        const allowGeneral = allowGeneralToggle.checked;
        const regex = allowGeneral ? GENERAL_REGEX : DNA_REGEX;

        if (value.length > MAX_LENGTH) {
            feedbackEl.textContent = `Sequence exceeds maximum limit (${value.length}/${MAX_LENGTH} bp).`;
            feedbackEl.className = "input-feedback error";
            return false;
        }

        if (!regex.test(value)) {
            // Find offending character
            let badChar = "";
            for (let i = 0; i < value.length; i++) {
                if (allowGeneral) {
                    if (value[i] < "A" || value[i] > "Z") {
                        badChar = value[i];
                        break;
                    }
                } else {
                    if (!["A", "T", "G", "C"].includes(value[i])) {
                        badChar = value[i];
                        break;
                    }
                }
            }
            if (allowGeneral) {
                feedbackEl.textContent = `Invalid character '${badChar}'. Only uppercase letters A-Z are allowed.`;
            } else {
                feedbackEl.textContent = `Invalid character '${badChar}'. Only DNA nucleotides A, T, G, and C are allowed.`;
            }
            feedbackEl.className = "input-feedback error";
            return false;
        }

        if (value.length === 0) {
            feedbackEl.textContent = "Sequence is empty (safe; LCS length will be 0).";
            feedbackEl.className = "input-feedback valid";
        } else {
            feedbackEl.textContent = "Valid sequence.";
            feedbackEl.className = "input-feedback valid";
        }
        return true;
    }

    /**
     * Updates live nucleotide counter badges.
     */
    function updateCounters() {
        const len1 = seq1Input.value.trim().length;
        const len2 = seq2Input.value.trim().length;
        seq1Count.textContent = `${len1} / ${MAX_LENGTH} bp`;
        seq2Count.textContent = `${len2} / ${MAX_LENGTH} bp`;
    }

    /**
     * Input event listeners for real-time validation and counter updates.
     */
    seq1Input.addEventListener("input", () => {
        validateInput(seq1Input, seq1Feedback);
        updateCounters();
        hideAlert();
    });

    seq2Input.addEventListener("input", () => {
        validateInput(seq2Input, seq2Feedback);
        updateCounters();
        hideAlert();
    });

    allowGeneralToggle.addEventListener("change", () => {
        validateInput(seq1Input, seq1Feedback);
        validateInput(seq2Input, seq2Feedback);
    });

    /**
     * Displays a dismissible alert box.
     */
    function showAlert(message) {
        generalAlert.textContent = message;
        generalAlert.className = "alert-box alert-error";
        generalAlert.style.display = "block";
    }

    function hideAlert() {
        generalAlert.style.display = "none";
        generalAlert.textContent = "";
    }

    /**
     * Sets button state while calling backend.
     */
    function setLoading(isLoading) {
        if (isLoading) {
            compareBtn.disabled = true;
            loadExampleBtn.disabled = true;
            clearBtn.disabled = true;
            compareSpinner.style.display = "inline-block";
            compareBtn.querySelector(".btn-text").textContent = "Comparing with C Binary...";
        } else {
            compareBtn.disabled = false;
            loadExampleBtn.disabled = false;
            clearBtn.disabled = false;
            compareSpinner.style.display = "none";
            compareBtn.querySelector(".btn-text").textContent = "Compare Sequences";
        }
    }

    /**
     * Populates form inputs with given example sequences.
     */
    function applyExample(s1, s2, isGeneral = false) {
        allowGeneralToggle.checked = isGeneral;
        seq1Input.value = s1;
        seq2Input.value = s2;
        validateInput(seq1Input, seq1Feedback);
        validateInput(seq2Input, seq2Feedback);
        updateCounters();
        hideAlert();
    }

    // Example Presets
    exStandardBtn.addEventListener("click", () => {
        applyExample("ATGCCGTA", "ATGCGTA", false);
    });

    exRepeatedBtn.addEventListener("click", () => {
        applyExample("AAAA", "AA", false);
    });

    exTextbookBtn.addEventListener("click", () => {
        applyExample("AGGTAB", "GXTXAYB", true);
    });

    exDifferentBtn.addEventListener("click", () => {
        applyExample("AAAA", "TTTT", false);
    });

    loadExampleBtn.addEventListener("click", () => {
        applyExample("ATGCCGTA", "ATGCGTA", false);
    });

    clearBtn.addEventListener("click", () => {
        seq1Input.value = "";
        seq2Input.value = "";
        seq1Feedback.textContent = "";
        seq2Feedback.textContent = "";
        updateCounters();
        hideAlert();
        resultsDashboard.style.display = "none";
        visualizationSection.style.display = "none";
        dpMatrixSection.style.display = "none";
    });

    /**
     * Renders nucleotide tracks with matched positions highlighted based on C backend results.
     */
    function renderVisualization(seq1, seq2, matchIdx1, matchIdx2) {
        // Clear previous ribbons safely
        seq1Ribbon.innerHTML = "";
        seq2Ribbon.innerHTML = "";

        const matchSet1 = new Set(matchIdx1 || []);
        const matchSet2 = new Set(matchIdx2 || []);

        visSeq1Meta.textContent = `${seq1.length} nucleotides (${matchSet1.size} matched)`;
        visSeq2Meta.textContent = `${seq2.length} nucleotides (${matchSet2.size} matched)`;

        // Build Track 1
        if (seq1.length === 0) {
            const emptyNotice = document.createElement("span");
            emptyNotice.className = "stat-subtext";
            emptyNotice.textContent = "(Empty Sequence)";
            seq1Ribbon.appendChild(emptyNotice);
        } else {
            for (let i = 0; i < seq1.length; i++) {
                const charBox = document.createElement("div");
                const isMatched = matchSet1.has(i);
                charBox.className = `nucleotide-box ${isMatched ? "matched" : "unmatched"}`;
                
                const charLetter = document.createElement("span");
                charLetter.textContent = seq1[i];
                
                const posLabel = document.createElement("span");
                posLabel.className = "nucleotide-pos";
                posLabel.textContent = (i + 1).toString(); // 1-indexed for biological display

                charBox.appendChild(charLetter);
                charBox.appendChild(posLabel);
                seq1Ribbon.appendChild(charBox);
            }
        }

        // Build Track 2
        if (seq2.length === 0) {
            const emptyNotice = document.createElement("span");
            emptyNotice.className = "stat-subtext";
            emptyNotice.textContent = "(Empty Sequence)";
            seq2Ribbon.appendChild(emptyNotice);
        } else {
            for (let j = 0; j < seq2.length; j++) {
                const charBox = document.createElement("div");
                const isMatched = matchSet2.has(j);
                charBox.className = `nucleotide-box ${isMatched ? "matched" : "unmatched"}`;
                
                const charLetter = document.createElement("span");
                charLetter.textContent = seq2[j];
                
                const posLabel = document.createElement("span");
                posLabel.className = "nucleotide-pos";
                posLabel.textContent = (j + 1).toString();

                charBox.appendChild(charLetter);
                charBox.appendChild(posLabel);
                seq2Ribbon.appendChild(charBox);
            }
        }

        visualizationSection.style.display = "block";
    }

    /**
     * Renders the Dynamic Programming matrix received from the C program.
     */
    function renderDPMatrix(seq1, seq2, matrix, canRender) {
        matrixWrapper.innerHTML = "";

        const m = seq1.length;
        const n = seq2.length;

        if (!canRender && (m > 35 || n > 35)) {
            const warningCard = document.createElement("div");
            warningCard.style.padding = "1.5rem";
            warningCard.style.textAlign = "center";

            const p = document.createElement("p");
            p.style.marginBottom = "1rem";
            p.style.fontSize = "0.95rem";
            p.style.color = "var(--text-on-light)";
            p.textContent = `The input sequences are ${m} × ${n} nucleotides (${(m + 1) * (n + 1)} table cells). Visual rendering is collapsed to ensure high browser performance.`;

            const showBtn = document.createElement("button");
            showBtn.type = "button";
            showBtn.className = "btn btn-secondary";
            showBtn.textContent = `Render Full Table (${(m + 1) * (n + 1)} cells)`;
            showBtn.addEventListener("click", () => {
                renderDPMatrix(seq1, seq2, matrix, true);
            });

            warningCard.appendChild(p);
            warningCard.appendChild(showBtn);
            matrixWrapper.appendChild(warningCard);
            dpMatrixSection.style.display = "block";
            return;
        }

        const table = document.createElement("table");
        table.className = "dp-table";

        // Top Header Row: Column indices and Sequence 2 characters
        const thead = document.createElement("thead");
        const headerRow = document.createElement("tr");

        const cornerCell = document.createElement("th");
        cornerCell.className = "corner-cell";
        cornerCell.textContent = "i \\ j";
        headerRow.appendChild(cornerCell);

        const epsColCell = document.createElement("th");
        epsColCell.textContent = "ε (0)";
        headerRow.appendChild(epsColCell);

        for (let j = 0; j < n; j++) {
            const th = document.createElement("th");
            th.textContent = `${seq2[j]} (${j + 1})`;
            headerRow.appendChild(th);
        }
        thead.appendChild(headerRow);
        table.appendChild(thead);

        // Body: Rows for ε and Sequence 1 characters
        const tbody = document.createElement("tbody");

        for (let i = 0; i <= m; i++) {
            const tr = document.createElement("tr");

            // Row header
            const rowHeader = document.createElement("th");
            rowHeader.className = "row-header";
            if (i === 0) {
                rowHeader.textContent = "ε (0)";
            } else {
                rowHeader.textContent = `${seq1[i - 1]} (${i})`;
            }
            tr.appendChild(rowHeader);

            // Cells
            for (let j = 0; j <= n; j++) {
                const td = document.createElement("td");
                const val = (matrix && matrix[i] && matrix[i][j] !== undefined) ? matrix[i][j] : 0;
                td.textContent = val.toString();

                if (i === 0 || j === 0) {
                    td.className = "cell-zero";
                } else if (i === m && j === n && (m > 0 || n > 0)) {
                    td.className = "cell-lcs-final";
                    td.title = `LCS Length: ${val}`;
                }

                tr.appendChild(td);
            }
            tbody.appendChild(tr);
        }

        table.appendChild(tbody);
        matrixWrapper.appendChild(table);
        dpMatrixSection.style.display = "block";
    }

    /**
     * Form submission handler: sends sequences to Flask backend.
     */
    form.addEventListener("submit", async (e) => {
        e.preventDefault();
        hideAlert();

        const v1 = validateInput(seq1Input, seq1Feedback);
        const v2 = validateInput(seq2Input, seq2Feedback);

        if (!v1 || !v2) {
            showAlert("Please correct the invalid sequence inputs before comparing.");
            return;
        }

        const seq1 = seq1Input.value.trim().toUpperCase();
        const seq2 = seq2Input.value.trim().toUpperCase();
        const allowGeneral = allowGeneralToggle.checked;

        setLoading(true);

        try {
            const response = await fetch("/api/compare", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    seq1: seq1,
                    seq2: seq2,
                    allow_general: allowGeneral
                })
            });

            const data = await response.json();

            if (!response.ok || !data.success) {
                const errMessage = data.error || `Server returned error status ${response.status}.`;
                showAlert(errMessage);
                return;
            }

            // Populate Results Dashboard
            resLcsText.textContent = data.lcs.length > 0 ? data.lcs : "(None - No Common Subsequence)";
            resLcsLen.textContent = data.lcs_length.toString();
            resLen1.textContent = data.len1.toString();
            resLen2.textContent = data.len2.toString();

            const score = typeof data.overlap_score === "number" ? data.overlap_score : 0.0;
            resOverlapScore.textContent = `${score.toFixed(2)}%`;
            resOverlapProgress.style.width = `${Math.min(100, Math.max(0, score))}%`;

            resTimeComp.textContent = data.time_complexity || "O(m × n)";
            resSpaceComp.textContent = data.space_complexity || "O(m × n)";
            resultsDashboard.style.display = "block";

            // Render Visualization Track
            renderVisualization(data.seq1, data.seq2, data.match_indices_seq1, data.match_indices_seq2);

            // Render DP Matrix
            renderDPMatrix(data.seq1, data.seq2, data.dp_matrix, data.render_matrix);

            // Smooth scroll to results
            resultsDashboard.scrollIntoView({ behavior: "smooth" });

        } catch (networkError) {
            showAlert(`Failed to connect to backend service: ${networkError.message}. Make sure Flask server is running.`);
        } finally {
            setLoading(false);
        }
    });

    // Initial setup
    updateCounters();
});
