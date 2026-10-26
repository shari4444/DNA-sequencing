/*
 * ============================================================================
 * Project: DNASeq — DNA Sequence Comparison Using Dynamic Programming
 * File:    main.c
 * Purpose: Standalone C program implementing the Longest Common Subsequence
 *          (LCS) algorithm for comparing two DNA sequences.
 * Course:  Analysis of Algorithms (AOA) - Second Year Engineering Mini Project
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Maximum allowed sequence length for safety and bounds checking */
#define MAX_SEQ_LEN 500

/* Global DP table to avoid stack overflow for large arrays */
static int dp[MAX_SEQ_LEN + 1][MAX_SEQ_LEN + 1];

/**
 * Normalizes a string by converting all lowercase letters to uppercase
 * and removing any trailing newline characters from user input.
 */
void normalize_sequence(char *str) {
    int i = 0;
    int j = 0;
    while (str[i] != '\0') {
        /* Discard newline and carriage return */
        if (str[i] != '\r' && str[i] != '\n') {
            str[j] = (char)toupper((unsigned char)str[i]);
            j++;
        }
        i++;
    }
    str[j] = '\0';
}

/**
 * Validates whether a sequence contains only valid characters.
 * In strict DNA mode (allow_general == 0), only A, T, G, and C are allowed.
 * In general mode (allow_general == 1), uppercase letters A-Z are allowed
 * (useful for textbook LCS algorithm examples such as AGGTAB and GXTXAYB).
 *
 * Returns 1 if valid, 0 if an invalid character is encountered.
 * If invalid, invalid_char is populated with the offending character.
 */
int validate_sequence(const char *str, int allow_general, char *invalid_char) {
    int i = 0;
    while (str[i] != '\0') {
        char ch = str[i];
        if (allow_general == 0) {
            /* Strict DNA validation */
            if (ch != 'A' && ch != 'T' && ch != 'G' && ch != 'C') {
                if (invalid_char != NULL) {
                    *invalid_char = ch;
                }
                return 0;
            }
        } else {
            /* General textbook LCS mode: uppercase alphabet characters */
            if (ch < 'A' || ch > 'Z') {
                if (invalid_char != NULL) {
                    *invalid_char = ch;
                }
                return 0;
            }
        }
        i++;
    }
    return 1;
}

/**
 * Reverses a null-terminated string in place.
 * Used to restore the reconstructed LCS characters to their original forward order.
 */
void reverse_string(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

/**
 * Reverses an integer array in place.
 * Used to sort matched indices in ascending forward order.
 */
void reverse_int_array(int *arr, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        int temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}

/**
 * Computes the Longest Common Subsequence (LCS) using Dynamic Programming.
 * 
 * Parameters:
 *   seq1: First sequence
 *   seq2: Second sequence
 *   m: Length of seq1
 *   n: Length of seq2
 *   lcs_out: Buffer to store the reconstructed LCS string (must be at least MAX_SEQ_LEN + 1)
 *   match_idx1: Array to store 0-based indices of matching nucleotides in seq1
 *   match_idx2: Array to store 0-based indices of matching nucleotides in seq2
 * 
 * Returns:
 *   The length of the Longest Common Subsequence.
 */
int compute_lcs(const char *seq1, const char *seq2, int m, int n,
                char *lcs_out, int *match_idx1, int *match_idx2) {
    int i;
    int j;

    /* Step 1: Initialize the first row and first column to zero */
    for (i = 0; i <= m; i++) {
        dp[i][0] = 0;
    }
    for (j = 0; j <= n; j++) {
        dp[0][j] = 0;
    }

    /* Step 2: Fill the DP table using the recurrence relation */
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (seq1[i - 1] == seq2[j - 1]) {
                /* Match found: add 1 to the diagonal value */
                dp[i][j] = 1 + dp[i - 1][j - 1];
            } else {
                /* Mismatch: store the maximum of upper and left values */
                if (dp[i - 1][j] >= dp[i][j - 1]) {
                    dp[i][j] = dp[i - 1][j];
                } else {
                    dp[i][j] = dp[i][j - 1];
                }
            }
        }
    }

    /* The bottom-right cell contains the length of the LCS */
    int lcs_len = dp[m][n];

    /* Step 3: Backtrack from dp[m][n] to reconstruct the LCS */
    i = m;
    j = n;
    int current_index = 0;

    while (i > 0 && j > 0) {
        if (seq1[i - 1] == seq2[j - 1]) {
            /* Character is part of the LCS */
            lcs_out[current_index] = seq1[i - 1];
            if (match_idx1 != NULL) {
                match_idx1[current_index] = i - 1; /* 0-based index */
            }
            if (match_idx2 != NULL) {
                match_idx2[current_index] = j - 1; /* 0-based index */
            }
            current_index++;
            i--;
            j--;
        } else {
            /* Move in the direction of the larger neighbor */
            if (dp[i - 1][j] >= dp[i][j - 1]) {
                i--;
            } else {
                j--;
            }
        }
    }
    lcs_out[current_index] = '\0';

    /* Step 4: Reverse the reconstructed characters to restore original order */
    reverse_string(lcs_out, current_index);
    if (match_idx1 != NULL) {
        reverse_int_array(match_idx1, current_index);
    }
    if (match_idx2 != NULL) {
        reverse_int_array(match_idx2, current_index);
    }

    return lcs_len;
}

/**
 * Calculates the LCS-based overlap score:
 * Overlap Score = (LCS Length / max(m, n)) * 100
 * Handles zero-length inputs safely without division by zero.
 */
double calculate_overlap_score(int lcs_len, int m, int n) {
    int max_len;
    if (m > n) {
        max_len = m;
    } else {
        max_len = n;
    }

    if (max_len == 0) {
        return 0.0;
    }

    return (((double)lcs_len) / ((double)max_len)) * 100.0;
}

/**
 * Prints the Dynamic Programming matrix to stdout in an exam-friendly tabular view.
 */
void print_dp_matrix_human(const char *seq1, const char *seq2, int m, int n) {
    int i;
    int j;

    printf("\n--- Dynamic Programming Matrix dp[m+1][n+1] ---\n");
    printf("        eps");
    for (j = 0; j < n; j++) {
        printf("%4c", seq2[j]);
    }
    printf("\n");

    for (i = 0; i <= m; i++) {
        if (i == 0) {
            printf("eps  ");
        } else {
            printf("%-4c ", seq1[i - 1]);
        }
        for (j = 0; j <= n; j++) {
            printf("%4d", dp[i][j]);
        }
        printf("\n");
    }
    printf("------------------------------------------------\n");
}

/**
 * Outputs the results in JSON format for consumption by the Flask backend.
 */
void output_json(const char *seq1, const char *seq2, int m, int n,
                 const char *lcs, int lcs_len, double overlap_score,
                 const int *match_idx1, const int *match_idx2) {
    int i;
    int j;

    printf("{\n");
    printf("  \"success\": true,\n");
    printf("  \"seq1\": \"%s\",\n", seq1);
    printf("  \"seq2\": \"%s\",\n", seq2);
    printf("  \"len1\": %d,\n", m);
    printf("  \"len2\": %d,\n", n);
    printf("  \"lcs\": \"%s\",\n", lcs);
    printf("  \"lcs_length\": %d,\n", lcs_len);
    printf("  \"overlap_score\": %.2f,\n", overlap_score);
    printf("  \"time_complexity\": \"O(m * n)\",\n");
    printf("  \"space_complexity\": \"O(m * n)\",\n");

    /* Matched indices for Sequence 1 */
    printf("  \"match_indices_seq1\": [");
    for (i = 0; i < lcs_len; i++) {
        printf("%d", match_idx1[i]);
        if (i < lcs_len - 1) {
            printf(", ");
        }
    }
    printf("],\n");

    /* Matched indices for Sequence 2 */
    printf("  \"match_indices_seq2\": [");
    for (i = 0; i < lcs_len; i++) {
        printf("%d", match_idx2[i]);
        if (i < lcs_len - 1) {
            printf(", ");
        }
    }
    printf("],\n");

    /* DP Table matrix (rows: 0 to m, cols: 0 to n) */
    printf("  \"dp_matrix\": [\n");
    for (i = 0; i <= m; i++) {
        printf("    [");
        for (j = 0; j <= n; j++) {
            printf("%d", dp[i][j]);
            if (j < n) {
                printf(", ");
            }
        }
        printf("]");
        if (i < m) {
            printf(",\n");
        } else {
            printf("\n");
        }
    }
    printf("  ]\n");
    printf("}\n");
}

/**
 * Outputs a JSON error response.
 */
void output_json_error(const char *error_message) {
    printf("{\n");
    printf("  \"success\": false,\n");
    printf("  \"error\": \"%s\"\n", error_message);
    printf("}\n");
}

/**
 * Main function providing interactive terminal interface and CLI argument support.
 */
int main(int argc, char *argv[]) {
    int json_mode = 0;
    int allow_general = 0;
    char raw_seq1[MAX_SEQ_LEN + 10] = "";
    char raw_seq2[MAX_SEQ_LEN + 10] = "";
    int arg_idx = 1;
    int seq_count = 0;

    /* Parse command-line flags and arguments */
    while (arg_idx < argc) {
        if (strcmp(argv[arg_idx], "--json") == 0) {
            json_mode = 1;
        } else if (strcmp(argv[arg_idx], "--general") == 0) {
            allow_general = 1;
        } else if (strcmp(argv[arg_idx], "--help") == 0 || strcmp(argv[arg_idx], "-h") == 0) {
            printf("Usage: %s [--json] [--general] [SEQUENCE1] [SEQUENCE2]\n", argv[0]);
            printf("  --json     Output result in JSON format (used by web backend)\n");
            printf("  --general  Allow all uppercase letters A-Z (textbook LCS mode)\n");
            printf("  --help     Show this help message\n");
            return 0;
        } else {
            if (seq_count == 0) {
                strncpy(raw_seq1, argv[arg_idx], sizeof(raw_seq1) - 1);
                raw_seq1[sizeof(raw_seq1) - 1] = '\0';
                seq_count++;
            } else if (seq_count == 1) {
                strncpy(raw_seq2, argv[arg_idx], sizeof(raw_seq2) - 1);
                raw_seq2[sizeof(raw_seq2) - 1] = '\0';
                seq_count++;
            }
        }
        arg_idx++;
    }

    /* If sequences were not provided via CLI arguments, read from stdin */
    if (seq_count < 2) {
        if (json_mode == 0) {
            printf("=================================================================\n");
            printf("  DNASeq: DNA Sequence Comparison Using Dynamic Programming      \n");
            printf("  Analysis of Algorithms (AOA) - Educational LCS Implementation  \n");
            printf("=================================================================\n\n");
            printf("Enter Sequence 1 (Nucleotides A, T, G, C): ");
            if (fgets(raw_seq1, sizeof(raw_seq1), stdin) == NULL) {
                raw_seq1[0] = '\0';
            }
            printf("Enter Sequence 2 (Nucleotides A, T, G, C): ");
            if (fgets(raw_seq2, sizeof(raw_seq2), stdin) == NULL) {
                raw_seq2[0] = '\0';
            }
        } else {
            /* JSON mode stdin reading: line 1 = seq1, line 2 = seq2 */
            if (seq_count == 0) {
                if (fgets(raw_seq1, sizeof(raw_seq1), stdin) == NULL) {
                    raw_seq1[0] = '\0';
                }
            }
            if (seq_count <= 1) {
                if (fgets(raw_seq2, sizeof(raw_seq2), stdin) == NULL) {
                    raw_seq2[0] = '\0';
                }
            }
        }
    }

    /* Normalize inputs (uppercase and strip newlines) */
    normalize_sequence(raw_seq1);
    normalize_sequence(raw_seq2);

    int m = (int)strlen(raw_seq1);
    int n = (int)strlen(raw_seq2);

    /* Check sequence length constraints */
    if (m > MAX_SEQ_LEN || n > MAX_SEQ_LEN) {
        char err_msg[128];
        snprintf(err_msg, sizeof(err_msg),
                 "Sequence length exceeds maximum limit of %d nucleotides.", MAX_SEQ_LEN);
        if (json_mode == 1) {
            output_json_error(err_msg);
        } else {
            fprintf(stderr, "Error: %s\n", err_msg);
        }
        return 1;
    }

    /* Validate Sequence 1 characters */
    char invalid_ch1 = '\0';
    if (validate_sequence(raw_seq1, allow_general, &invalid_ch1) == 0) {
        char err_msg[160];
        if (allow_general == 0) {
            snprintf(err_msg, sizeof(err_msg),
                     "Sequence 1 contains invalid character '%c'. Allowed nucleotides are A, T, G, C.",
                     invalid_ch1);
        } else {
            snprintf(err_msg, sizeof(err_msg),
                     "Sequence 1 contains invalid character '%c'. Allowed characters are A-Z.",
                     invalid_ch1);
        }
        if (json_mode == 1) {
            output_json_error(err_msg);
        } else {
            fprintf(stderr, "Validation Error: %s\n", err_msg);
        }
        return 1;
    }

    /* Validate Sequence 2 characters */
    char invalid_ch2 = '\0';
    if (validate_sequence(raw_seq2, allow_general, &invalid_ch2) == 0) {
        char err_msg[160];
        if (allow_general == 0) {
            snprintf(err_msg, sizeof(err_msg),
                     "Sequence 2 contains invalid character '%c'. Allowed nucleotides are A, T, G, C.",
                     invalid_ch2);
        } else {
            snprintf(err_msg, sizeof(err_msg),
                     "Sequence 2 contains invalid character '%c'. Allowed characters are A-Z.",
                     invalid_ch2);
        }
        if (json_mode == 1) {
            output_json_error(err_msg);
        } else {
            fprintf(stderr, "Validation Error: %s\n", err_msg);
        }
        return 1;
    }

    /* Buffers for reconstruction */
    char lcs[MAX_SEQ_LEN + 1];
    int match_idx1[MAX_SEQ_LEN + 1];
    int match_idx2[MAX_SEQ_LEN + 1];

    /* Execute the Longest Common Subsequence dynamic programming algorithm */
    int lcs_len = compute_lcs(raw_seq1, raw_seq2, m, n, lcs, match_idx1, match_idx2);
    double overlap_score = calculate_overlap_score(lcs_len, m, n);

    /* Output formatting */
    if (json_mode == 1) {
        output_json(raw_seq1, raw_seq2, m, n, lcs, lcs_len, overlap_score, match_idx1, match_idx2);
    } else {
        printf("\n================ COMPARISON RESULTS ================\n");
        printf("Sequence 1:                 %s (Length: %d)\n", raw_seq1, m);
        printf("Sequence 2:                 %s (Length: %d)\n", raw_seq2, n);
        if (lcs_len > 0) {
            printf("Longest Common Subsequence: %s\n", lcs);
        } else {
            printf("Longest Common Subsequence: (None)\n");
        }
        printf("LCS Length:                 %d\n", lcs_len);
        printf("LCS-Based Overlap Score:    %.2f%%\n", overlap_score);
        printf("Time Complexity:            O(m * n) = O(%d * %d) = %d cell evaluations\n",
               m, n, (m + 1) * (n + 1));
        printf("Space Complexity:           O(m * n) = (%d * %d) integers in DP table\n",
               m + 1, n + 1);
        printf("====================================================\n");

        if (m <= 30 && n <= 30) {
            print_dp_matrix_human(raw_seq1, raw_seq2, m, n);
        } else {
            printf("\n[Note: DP Matrix omitted in terminal for large inputs (m > 30 or n > 30)].\n");
        }

        printf("\n[Scientific Disclaimer]: This is an educational demonstration.\n");
        printf("An LCS overlap score is not equal to validated biological sequence identity\n");
        printf("and cannot establish genetic relatedness, disease risk, or biological function.\n\n");
    }

    return 0;
}
