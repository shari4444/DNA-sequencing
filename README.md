# DNASeq — DNA Sequence Comparison Using Dynamic Programming

**Analysis of Algorithms (AOA) — Second Year Engineering Mini Project**

All source code, backend files, frontend assets, tests, and detailed documentation are located in the [`DNASeq/`](./DNASeq/) directory.

### Quick Start
```bash
# 1. Navigate to the project directory
cd DNASeq

# 2. Run the standalone C program directly
gcc -Wall -Wextra -pedantic -O2 -o dna_lcs main.c
./dna_lcs ATGCCGTA ATGCGTA

# 3. Or launch the full web application
source venv/bin/activate
python3 app.py
```
Open `http://127.0.0.1:5001` in your browser.

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


Please refer to the comprehensive [**DNASeq/README.md**](./DNASeq/README.md) for full project documentation, algorithm walkthrough, viva questions & answers, pseudocode, and complexity analysis.
