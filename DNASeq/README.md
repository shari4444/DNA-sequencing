# DNASeq — DNA Sequence Comparison Using Dynamic Programming

**Analysis of Algorithms (AOA) — Second Year Engineering Mini Project**

---

## 1. Project Introduction

**DNASeq** is an educational bioinformatics demonstration that compares two deoxyribonucleic acid (DNA) sequences and computes their **Longest Common Subsequence (LCS)** using **Dynamic Programming (DP)**.

### Why Compare DNA Sequences?
In molecular biology, DNA sequences consist of long chains of four chemical nucleotide bases:
- **A** — Adenine
- **T** — Thymine
- **G** — Guanine
- **C** — Cytosine

Computational comparison of DNA sequences allows computer scientists and bioinformaticians to identify shared evolutionary patterns, detect structural motifs, and understand sequence alignments across organisms.

### What the Application Displays
- The exact **Longest Common Subsequence** and its length.
- The input lengths of both sequences.
- An **LCS-Based Overlap Score** showing relative sequence coverage.
- An interactive **Nucleotide Alignment Ribbon** highlighting the exact matched positions backtracked by the algorithm.
- The full 2D **Dynamic Programming Table ($dp[m+1][n+1]$)** showing cell-by-cell values.
- Step-by-step algorithm explanations and theoretical complexity analysis ($O(m \times n)$ time and space).

### Educational Nature and Scope
This project was developed for the second-year engineering **Analysis of Algorithms (AOA)** course. It is designed to be:
1. **Theoretically Sound:** Demonstrates optimal substructure and overlapping subproblems in Dynamic Programming.
2. **Exam & Viva Ready:** The core algorithm in `main.c` is written in readable, standard C without ternary operators, making it easy to reproduce on paper or during a practical examination.
3. **Scientifically Honest:** Includes prominent disclaimers explaining that an algorithmic LCS overlap score is not the same as clinically or biologically validated sequence homology.

---

## 2. Algorithm Used: Longest Common Subsequence

### What is a Subsequence?
A **subsequence** is a sequence derived from another sequence by deleting zero or more elements without changing the relative order of the remaining elements.

### Subsequence vs. Substring (Key Viva Distinction)
- **Substring:** Characters must appear in **contiguous (consecutive)** positions.  
  *Example:* In `"ATGCCGTA"`, `"TGCC"` is a substring.
- **Subsequence:** Characters do **not** need to be contiguous, but their relative order must be strictly preserved.  
  *Example:* In `"ATGCCGTA"`, `"AGTA"` is a subsequence, but NOT a substring.

### What is Dynamic Programming?
**Dynamic Programming** is an algorithm design technique used to solve optimization problems by breaking them down into smaller overlapping subproblems, solving each subproblem once, and storing the results in a table (memoization or tabulation) to avoid redundant recalculation.

### Why is LCS a Dynamic Programming Algorithm?
A naive recursive solution to find the LCS would evaluate all $2^m$ subsequences of Sequence 1 and check if they exist in Sequence 2, requiring exponential time ($O(2^m)$). However, LCS possesses two properties:
1. **Optimal Substructure:** The optimal solution to the problem contains within it optimal solutions to smaller subproblems (prefixes of the sequences).
2. **Overlapping Subproblems:** The same subproblems are solved repeatedly throughout the recursive tree.

Dynamic Programming stores solutions to subproblems in a 2D matrix ($dp[i][j]$), reducing the time complexity from exponential $O(2^m)$ to polynomial $O(m \times n)$.

### Example
- **Sequence 1:** `ATGCCGTA` (Length $m = 8$)
- **Sequence 2:** `ATGCGTA` (Length $n = 7$)
- **Longest Common Subsequence:** `ATGCGTA` (Length = 7)
- **Matched Characters:** The shared sequence preserves order: `A -> T -> G -> C -> G -> T -> A`.

---

## 3. Algorithm — Step-by-Step in English

Here is the complete step-by-step procedure:

1. **Start:** Begin the algorithm execution.
2. **Read Sequence 1 and Sequence 2:** Accept input string `seq1` (length $m$) and `seq2` (length $n$).
3. **Validate the characters:** Normalize all letters to uppercase. Ensure each character is strictly an allowed nucleotide: `A`, `T`, `G`, or `C`. Reject invalid characters.
4. **Find the lengths $m$ and $n$:** Calculate $m = \text{length}(seq1)$ and $n = \text{length}(seq2)$. Check that neither exceeds the maximum buffer limit (500).
5. **Create a DP matrix with $(m + 1)$ rows and $(n + 1)$ columns:** Declare a 2D integer array $dp[m+1][n+1]$. Row index $i$ corresponds to prefixes of $seq1$; column index $j$ corresponds to prefixes of $seq2$.
6. **Set the first row and first column to zero:** For all $i$ from 0 to $m$, set $dp[i][0] = 0$. For all $j$ from 0 to $n$, set $dp[0][j] = 0$ (an empty prefix shares 0 characters with any sequence).
7. **Traverse the matrix using nested loops:** Use an outer loop with index $i$ from $1$ to $m$, and an inner loop with index $j$ from $1$ to $n$.
8. **Compare the current characters:** Check if $seq1[i - 1]$ equals $seq2[j - 1]$ (using 0-based character indexing).
9. **If they match, add one to the diagonal cell:** If $seq1[i - 1] == seq2[j - 1]$, assign $dp[i][j] = 1 + dp[i - 1][j - 1]$.
10. **Otherwise, compare the upper and left cells:** If the characters differ, inspect $dp[i - 1][j]$ (upper cell) and $dp[i][j - 1]$ (left cell).
11. **Store the larger value:** If $dp[i - 1][j] \ge dp[i][j - 1]$, set $dp[i][j] = dp[i - 1][j]$; otherwise, set $dp[i][j] = dp[i][j - 1]$.
12. **Continue until the matrix is complete:** Repeat steps 8–11 for all $i$ and $j$ until every cell in the table has been populated.
13. **Read the LCS length from $dp[m][n]$:** The value in the bottom-right cell $dp[m][n]$ represents the maximum LCS length.
14. **Start backtracking from the bottom-right cell:** Set tracking pointers $i = m$ and $j = n$.
15. **If characters match, record the character and move diagonally:** If $seq1[i - 1] == seq2[j - 1]$, record $seq1[i - 1]$ into the output buffer, decrement $i$ by 1, and decrement $j$ by 1.
16. **Otherwise, move towards the neighboring cell with the larger value:** If $seq1[i - 1] \ne seq2[j - 1]$, check which adjacent cell is greater: if $dp[i - 1][j] \ge dp[i][j - 1]$, set $i = i - 1$; else set $j = j - 1$.
17. **Reverse the collected characters:** Backtracking gathers matching nucleotides in reverse order (from end to start). Reverse the output string in place to restore original order.
18. **Calculate the overlap score:** Compute $\text{Overlap Score} = \left(\frac{\text{LCS Length}}{\max(m, n)}\right) \times 100$. If both sequences are empty, safely evaluate to $0.0\%$.
19. **Display the result:** Present the LCS, LCS length, overlap score, highlighted alignment, and DP matrix.
20. **Stop:** Terminate the program.

---

## 4. Pseudocode

### A. DP Table Construction
```text
Algorithm ComputeLCS(seq1, seq2, m, n):
    Input: Two sequences seq1 of length m, seq2 of length n
    Output: DP table containing optimal prefix LCS lengths

    Declare integer array dp[m + 1][n + 1]

    // Base Case: empty sequence prefix
    For i from 0 to m do:
        dp[i][0] = 0
    End For
    For j from 0 to n do:
        dp[0][j] = 0
    End For

    // Table Filling
    For i from 1 to m do:
        For j from 1 to n do:
            If seq1[i - 1] == seq2[j - 1] then:
                dp[i][j] = 1 + dp[i - 1][j - 1]
            Else:
                If dp[i - 1][j] >= dp[i][j - 1] then:
                    dp[i][j] = dp[i - 1][j]
                Else:
                    dp[i][j] = dp[i][j - 1]
                End If
            End If
        End For
    End For

    Return dp[m][n]
End Algorithm
```

### B. LCS Backtracking Reconstruction
```text
Algorithm ReconstructLCS(seq1, seq2, dp, m, n):
    Input: Sequences seq1, seq2, filled dp table, lengths m and n
    Output: Reconstructed LCS string

    Declare character array lcs[]
    index = 0
    i = m
    j = n

    While i > 0 and j > 0 do:
        If seq1[i - 1] == seq2[j - 1] then:
            lcs[index] = seq1[i - 1]
            index = index + 1
            i = i - 1
            j = j - 1
        Else:
            If dp[i - 1][j] >= dp[i][j - 1] then:
                i = i - 1
            Else:
                j = j - 1
            End If
        End If
    End While

    lcs[index] = '\0'
    ReverseString(lcs, index)
    Return lcs
End Algorithm
```

### C. Overlap Score Calculation
```text
Algorithm CalculateOverlapScore(lcs_len, m, n):
    If m > n then:
        max_len = m
    Else:
        max_len = n
    End If

    If max_len == 0 then:
        Return 0.0
    End If

    score = (lcs_len / max_len) * 100.0
    Return score
End Algorithm
```

---

## 5. Time and Space Complexity of LCS

| Phase | Time Complexity | Space Complexity | Explanation |
| :--- | :--- | :--- | :--- |
| **Table Initialization** | $O(m + n)$ | $O(m \times n)$ | Initializes first row and column to zero. |
| **Table Filling** | $O(m \times n)$ | $O(m \times n)$ | Two nested loops: $(m \times n)$ cells, each taking $O(1)$ constant time. |
| **Backtracking** | $O(m + n)$ | $O(\min(m, n))$ | At each step, either $i$, $j$, or both decrease by 1. |
| **String Reversal** | $O(\text{LCS Length})$ | $O(1)$ | Simple in-place two-pointer swap. |
| **Overall** | **$O(m \times n)$** | **$O(m \times n)$** | Dominated by the 2D DP matrix. |

### Why is the Time Complexity $O(m \times n)$?
- The DP table contains $(m + 1) \times (n + 1)$ cells.
- Each cell evaluates exactly one character equality check (`seq1[i-1] == seq2[j-1]`) and either an addition or comparison of two adjacent cells.
- Since each cell evaluation requires strictly $O(1)$ constant operations, the total time to fill the table is:
  $$\sum_{i=1}^{m} \sum_{j=1}^{n} O(1) = O(m \times n)$$
- Backtracking traverses from $(m, n)$ to $(0, 0)$. In the worst case, it takes at most $m + n$ steps, which is $O(m + n)$.
- Since $O(m + n) \ll O(m \times n)$ for non-trivial strings, the overall asymptotic time complexity remains **$O(m \times n)$**.

---

## 6. Formula Used

### 1. The LCS Dynamic Programming Recurrence Relation
$$dp[i][j] = \begin{cases} 
0 & \text{if } i = 0 \text{ or } j = 0 \\
1 + dp[i-1][j-1] & \text{if } seq1[i-1] == seq2[j-1] \\
\max(dp[i-1][j], \; dp[i][j-1]) & \text{if } seq1[i-1] \ne seq2[j-1]
\end{cases}$$

### 2. Overlap Score Formula
$$\text{Overlap Score} = \left(\frac{\text{LCS Length}}{\max(m, n)}\right) \times 100\%$$

- **Zero-Denominator Safety:** If both input sequences are empty ($m = 0$ and $n = 0$), $\max(m, n) = 0$. The formula safely catches this boundary condition and returns $0.00\%$, preventing division-by-zero crashes.
- **Scientific Caveat:** This score measures algorithmic sequence coverage. It is strictly an **LCS-Based Overlap Score**, not an official biological sequence identity or homology metric.

---

## 7. Example Input and Output

### Input
- **Sequence 1:** `ATGCCGTA` (Length $m = 8$)
- **Sequence 2:** `ATGCGTA` (Length $n = 7$)

### Output
- **Longest Common Subsequence:** `ATGCGTA`
- **LCS Length:** `7`
- **Sequence 1 Length ($m$):** `8`
- **Sequence 2 Length ($n$):** `7`
- **LCS-Based Overlap Score:** `87.50%`  
  $$\text{Overlap Score} = \frac{7}{\max(8, 7)} \times 100\% = \frac{7}{8} \times 100\% = 87.50\%$$
- **Time Complexity:** $O(m \times n) = O(8 \times 7) = 56$ cell evaluations
- **Space Complexity:** $O(m \times n) = 9 \times 8 = 72$ table integers

### Dynamic Programming Table ($dp$)
```text
        eps   A   T   G   C   G   T   A
eps       0   0   0   0   0   0   0   0
A         0   1   1   1   1   1   1   1
T         0   1   2   2   2   2   2   2
G         0   1   2   3   3   3   3   3
C         0   1   2   3   4   4   4   4
C         0   1   2   3   4   4   4   4
G         0   1   2   3   4   5   5   5
T         0   1   2   3   4   5   6   6
A         0   1   2   3   4   5   6   7   <-- LCS Length = 7
```

---

## 8. Technology Stack

- **C (`main.c`):** Implements the core Longest Common Subsequence algorithm, DP matrix filling, character validation, and backtracking. Designed to compile independently using GCC or Clang for exams and viva.
- **HTML5 (`index.html`):** Provides accessible, semantic single-page structure adhering to the biological interface layout.
- **CSS3 (`style.css`):** Implements the custom dark blood-maroon and warm off-white palette with responsive layouts for mobile, tablet, and desktop.
- **Vanilla JavaScript (`script.js`):** Manages frontend state, character counting, input validation, and SVG/DOM rendering of the DP matrix and nucleotide tracks. Never calculates the LCS algorithm on the client.
- **Python Flask (`app.py`):** Acts as the lightweight backend coordinator. Validates requests, securely executes the compiled C binary via Python's `subprocess` (without `shell=True`), and delivers structured JSON to the browser.

---

## 9. Project Folder Structure

```
DNASeq/
├── main.c              # Core C program (standalone CLI & exam reference)
├── app.py              # Flask server invoking the compiled C binary
├── requirements.txt    # Python dependencies (Flask)
├── index.html          # Frontend page layout
├── style.css           # Styling with dark blood-maroon and off-white theme
├── script.js           # Frontend interactivity and visualization
├── README.md           # Complete documentation and viva guide
├── .gitignore          # Git ignore rules for binaries and virtual environment
└── tests/
    ├── test_lcs.c      # Automated unit tests in C for all algorithm cases
    └── test_api.py     # Automated API & integration tests in Python
```

### Purpose of Each File:
- **`main.c`:** Standalone C program containing the LCS dynamic programming algorithm. Can be compiled and executed directly from the terminal. Supports standard interactive mode and `--json` mode for the backend.
- **`app.py`:** Flask web application that serves frontend files and exposes `/api/compare`. Executes `./dna_lcs` safely using `subprocess.run()`.
- **`requirements.txt`:** Specifies Python package requirements (`Flask`).
- **`index.html`:** Clean HTML5 structure containing input cards, dashboard, nucleotide ribbons, DP matrix container, algorithm timeline, complexity panel, and footer.
- **`style.css`:** Custom styling using CSS custom properties with strict dark maroon (`#26090F`, `#6D1728`) and off-white (`#F5F0E8`) colors.
- **`script.js`:** Pure vanilla JavaScript frontend script that calls `/api/compare` and renders results.
- **`tests/test_lcs.c`:** Suite of 28 unit tests written in C covering all prompt edge cases (identical, empty, mismatched, bounds checking, general textbook cases).
- **`tests/test_api.py`:** Python `unittest` suite covering 13 test cases verifying API endpoints, input validation, C binary invocation, and error handling.

---

## 10. Installation and Execution

### Prerequisites
- **C Compiler:** `gcc` or `clang` (pre-installed on macOS and most Linux distributions).
- **Python:** Python 3.8 or higher.

---

### Step 1: Verify Compiler & Python
```bash
# Check C compiler
gcc --version
# or
clang --version

# Check Python version
python3 --version
```

---

### Step 2: Set Up Python Virtual Environment
Navigate to the `DNASeq` directory:
```bash
cd DNASeq
```

Create and activate a virtual environment:
```bash
# macOS / Linux
python3 -m venv venv
source venv/bin/activate

# Windows (Command Prompt)
python -m venv venv
venv\Scripts\activate

# Windows (PowerShell)
python -m venv venv
venv\Scripts\Activate.ps1
```

Install Flask:
```bash
pip install -r requirements.txt
```

---

### Step 3: Compile the C Program
Compile `main.c` into an executable binary:

```bash
# macOS / Linux
gcc -Wall -Wextra -pedantic -O2 -o dna_lcs main.c

# Windows (MinGW / GCC)
gcc -Wall -Wextra -pedantic -O2 -o dna_lcs.exe main.c

# Windows (MSVC Developer Command Prompt)
cl /W4 /O2 main.c /Fe:dna_lcs.exe
```

*(Note: `app.py` also automatically compiles `main.c` if the binary is missing or modified).*

---

### Step 4: Run the Web Application
```bash
python3 app.py
```
Open your browser and navigate to:
```
http://127.0.0.1:5001
```

---

### Step 5: Running the Standalone C Program Independently (For Exam / Practical)

You can run `main.c` directly in the terminal without starting Python or Flask:

**Interactive Mode:**
```bash
./dna_lcs
```
Prompts:
```text
Enter Sequence 1 (Nucleotides A, T, G, C): ATGCCGTA
Enter Sequence 2 (Nucleotides A, T, G, C): ATGCGTA
```

**Command-Line Argument Mode:**
```bash
./dna_lcs ATGCCGTA ATGCGTA
```

**Textbook LCS Mode (Allows A-Z, e.g., AGGTAB and GXTXAYB):**
```bash
./dna_lcs --general AGGTAB GXTXAYB
```

**JSON Output Mode (Used by Backend):**
```bash
./dna_lcs --json ATGCCGTA ATGCGTA
```

---

### Step 6: Running Automated Tests

**Run C Unit Tests:**
```bash
gcc -Wall -Wextra -pedantic -o tests/test_lcs tests/test_lcs.c
./tests/test_lcs
```
*Expected: 28 / 28 tests passing.*

**Run Flask API Integration Tests:**
```bash
python3 tests/test_api.py
```
*Expected: 13 / 13 tests passing.*

---

## 11. Biological Applications and Limitations

### Educational Relevance
In bioinformatics education, comparing DNA sequences through LCS demonstrates fundamental concepts:
- **Discrete Sequence Analysis:** How algorithms process biological text represented as discrete strings of characters ($A, T, G, C$).
- **Evolutionary Substitutions and Deletions:** How insertion, deletion, and mutation can be modeled algorithmically.
- **Dynamic Programming Power:** Why brute-force approaches fail on genomic scales and how optimal substructures provide tractable solutions.

### Scientific Limitations
It is critical to distinguish an educational algorithm from real-world genomic software:
1. **No Gap Penalties:** LCS treats all insertions and deletions with equal cost (0 penalty). Biological alignment tools (e.g., Needleman-Wunsch) apply affine gap penalties to differentiate between opening and extending gaps.
2. **No Substitution Matrices:** LCS considers all non-identical nucleotides equally mismatched. Real biological systems use scoring matrices (e.g., PAM, BLOSUM for amino acids, or transition/transversion weights for DNA) because certain mutations (transitions: $A \leftrightarrow G, C \leftrightarrow T$) occur more frequently in nature.
3. **No Functional Annotation:** LCS cannot identify open reading frames (ORFs), promoter regions, introns, or exons.
4. **Clinical Non-Equivalence:** An LCS-based overlap score **cannot** establish genetic relatedness, calculate hereditary disease risk, or predict protein structure.

---

## 12. Viva Questions and Answers

### 1. What is LCS (Longest Common Subsequence)?
**Answer:** The Longest Common Subsequence of two sequences is the longest sequence of characters that appears in both original sequences in the same relative order, though not necessarily consecutively.

### 2. What is Dynamic Programming?
**Answer:** Dynamic Programming is an algorithmic technique for solving optimization problems by breaking them into overlapping subproblems, solving each subproblem once, and storing the subproblem results in a lookup table (tabulation or memoization).

### 3. Why is LCS considered a Dynamic Programming algorithm?
**Answer:** LCS exhibits both **optimal substructure** (the optimal solution for prefixes contains optimal solutions for smaller sub-prefixes) and **overlapping subproblems** (the same prefix comparisons are repeated multiple times). DP stores these subproblem results in a 2D table, preventing exponential re-computations.

### 4. What is the recurrence relation for LCS?
**Answer:**
- $dp[i][j] = 0$ when $i = 0$ or $j = 0$ (base case: comparison with an empty string).
- $dp[i][j] = 1 + dp[i-1][j-1]$ when $seq1[i-1] == seq2[j-1]$ (match: extend diagonal).
- $dp[i][j] = \max(dp[i-1][j], dp[i][j-1])$ when $seq1[i-1] \ne seq2[j-1]$ (mismatch: take the max of top and left cells).

### 5. Why do we initialize the first row and first column to zero?
**Answer:** Row 0 and Column 0 represent comparisons with an empty string ($\epsilon$). The length of a common subsequence between any sequence and an empty sequence is zero.

### 6. What is backtracking in the context of LCS?
**Answer:** Backtracking is the process of retracing the optimal path backwards through the populated DP table, starting from $dp[m][n]$ to $dp[0][0]$, to identify which specific characters contributed to the optimal score and reconstruct the actual LCS string.

### 7. Why do we reverse the reconstructed LCS?
**Answer:** Backtracking begins at the end of the sequences (bottom-right cell $dp[m][n]$) and works toward the beginning $(0, 0)$. Consequently, matching characters are collected in reverse order (3' to 5'). Reversing the gathered characters restores their original forward order (5' to 3').

### 8. What is the time complexity of LCS?
**Answer:** The overall time complexity is **$O(m \times n)$**, where $m$ and $n$ are the lengths of the two sequences. Filling the table requires evaluating $(m+1) \times (n+1)$ cells in $O(1)$ constant time each. Backtracking takes $O(m + n)$ time, which is strictly dominated by $O(m \times n)$.

### 9. What is the space complexity of LCS?
**Answer:** The space complexity is **$O(m \times n)$** because we must store the entire 2D matrix of size $(m + 1) \times (n + 1)$ integers in memory to enable backtracking. If only the length of the LCS is required without reconstructing the sequence, space can be optimized to $O(\min(m, n))$ using two rows.

### 10. What is the difference between a subsequence and a substring?
**Answer:** A **substring** consists of characters that appear contiguously (consecutively) in the string (e.g., `"GCC"` in `"ATGCCG"`). A **subsequence** does not need to be contiguous as long as the relative character order is preserved (e.g., `"ACG"` in `"ATGCCG"`).

### 11. What is the overlap score used in this project?
**Answer:** The overlap score is defined as:
$$\text{Overlap Score} = \left(\frac{\text{LCS Length}}{\max(m, n)}\right) \times 100$$
It normalizes the LCS length against the longer of the two sequences, representing the percentage coverage of the longer sequence.

### 12. Why is LCS insufficient for biological sequence identity?
**Answer:** LCS does not model biological realities such as weighted nucleotide transitions vs. transversions, codon reading frames, gap penalties for insertions/deletions, or biologically conserved functional domains. Professional bioinformatics tools use algorithms like Smith-Waterman or BLAST with specialized scoring matrices.

### 13. Why is the algorithm written in C?
**Answer:** C provides direct memory management, predictable performance, and is the standard programming language taught in university Analysis of Algorithms courses. Implementing it in C ensures students can reproduce and explain the code during practical examinations and viva voce.

### 14. What role does Flask play in this project?
**Answer:** Flask serves as a lightweight local API backend that coordinates requests from the web frontend, invokes the actual compiled C binary via Python's `subprocess` module, passes input through standard input, and returns structured JSON to the browser.

### 15. Why does the project use HTML, CSS, and Vanilla JavaScript?
**Answer:** Using vanilla HTML5, CSS3, and JavaScript ensures the application remains simple, lightweight, and transparent. There are no heavy frameworks (like React or Next.js) obscuring how the data flows from the C binary to the user interface.

---

## 13. Conclusion

**DNASeq** demonstrates how classic algorithm design principles — specifically **Dynamic Programming** and **Optimal Substructure** — can be applied to real-world data types like DNA sequences. By combining a high-performance, exam-friendly C implementation with an interactive, blood-maroon themed web dashboard, this project bridges theoretical algorithm analysis and practical bioinformatics exploration.
