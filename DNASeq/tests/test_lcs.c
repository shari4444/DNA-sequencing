/*
 * ============================================================================
 * Project: DNASeq — DNA Sequence Comparison Using Dynamic Programming
 * File:    tests/test_lcs.c
 * Purpose: Automated unit tests for C-based LCS functions and validations.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_SEQ_LEN 500

/* Global DP table matching main.c */
static int dp[MAX_SEQ_LEN + 1][MAX_SEQ_LEN + 1];

/* Function declarations matching main.c */
void normalize_sequence(char *str);
int validate_sequence(const char *str, int allow_general, char *invalid_char);
void reverse_string(char *str, int length);
void reverse_int_array(int *arr, int length);
int compute_lcs(const char *seq1, const char *seq2, int m, int n,
                char *lcs_out, int *match_idx1, int *match_idx2);
double calculate_overlap_score(int lcs_len, int m, int n);

/* Helper implementations for standalone test execution */
void normalize_sequence(char *str) {
    int i = 0;
    int j = 0;
    while (str[i] != '\0') {
        if (str[i] != '\r' && str[i] != '\n') {
            if (str[i] >= 'a' && str[i] <= 'z') {
                str[j] = (char)(str[i] - 32);
            } else {
                str[j] = str[i];
            }
            j++;
        }
        i++;
    }
    str[j] = '\0';
}

int validate_sequence(const char *str, int allow_general, char *invalid_char) {
    int i = 0;
    while (str[i] != '\0') {
        char ch = str[i];
        if (allow_general == 0) {
            if (ch != 'A' && ch != 'T' && ch != 'G' && ch != 'C') {
                if (invalid_char != NULL) {
                    *invalid_char = ch;
                }
                return 0;
            }
        } else {
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

int compute_lcs(const char *seq1, const char *seq2, int m, int n,
                char *lcs_out, int *match_idx1, int *match_idx2) {
    int i;
    int j;

    for (i = 0; i <= m; i++) {
        dp[i][0] = 0;
    }
    for (j = 0; j <= n; j++) {
        dp[0][j] = 0;
    }

    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (seq1[i - 1] == seq2[j - 1]) {
                dp[i][j] = 1 + dp[i - 1][j - 1];
            } else {
                if (dp[i - 1][j] >= dp[i][j - 1]) {
                    dp[i][j] = dp[i - 1][j];
                } else {
                    dp[i][j] = dp[i][j - 1];
                }
            }
        }
    }

    int lcs_len = dp[m][n];
    i = m;
    j = n;
    int current_index = 0;

    while (i > 0 && j > 0) {
        if (seq1[i - 1] == seq2[j - 1]) {
            lcs_out[current_index] = seq1[i - 1];
            if (match_idx1 != NULL) {
                match_idx1[current_index] = i - 1;
            }
            if (match_idx2 != NULL) {
                match_idx2[current_index] = j - 1;
            }
            current_index++;
            i--;
            j--;
        } else {
            if (dp[i - 1][j] >= dp[i][j - 1]) {
                i--;
            } else {
                j--;
            }
        }
    }
    lcs_out[current_index] = '\0';

    reverse_string(lcs_out, current_index);
    if (match_idx1 != NULL) {
        reverse_int_array(match_idx1, current_index);
    }
    if (match_idx2 != NULL) {
        reverse_int_array(match_idx2, current_index);
    }

    return lcs_len;
}

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

/* Test Runner */
int tests_run = 0;
int tests_passed = 0;

void assert_test(int condition, const char *test_name) {
    tests_run++;
    if (condition) {
        printf("  [PASS] %s\n", test_name);
        tests_passed++;
    } else {
        printf("  [FAIL] %s\n", test_name);
    }
}

int main(void) {
    printf("====================================================\n");
    printf("   Running DNASeq C Unit Tests                      \n");
    printf("====================================================\n\n");

    char lcs[MAX_SEQ_LEN + 1];
    int idx1[MAX_SEQ_LEN + 1];
    int idx2[MAX_SEQ_LEN + 1];

    /* Test 1: ATGCCGTA and ATGCGTA -> LCS length 7 */
    {
        const char *s1 = "ATGCCGTA";
        const char *s2 = "ATGCGTA";
        int len = compute_lcs(s1, s2, strlen(s1), strlen(s2), lcs, idx1, idx2);
        assert_test(len == 7, "Test 1: ATGCCGTA and ATGCGTA has LCS length 7");
        assert_test(strcmp(lcs, "ATGCGTA") == 0, "Test 1: LCS string matches 'ATGCGTA'");
        double score = calculate_overlap_score(len, strlen(s1), strlen(s2));
        assert_test(score >= 87.49 && score <= 87.51, "Test 1: Overlap score is 87.50%");
    }

    /* Test 2: AAAA and AA -> LCS length 2 */
    {
        const char *s1 = "AAAA";
        const char *s2 = "AA";
        int len = compute_lcs(s1, s2, strlen(s1), strlen(s2), lcs, idx1, idx2);
        assert_test(len == 2, "Test 2: AAAA and AA has LCS length 2");
        assert_test(strcmp(lcs, "AA") == 0, "Test 2: LCS string is 'AA'");
    }

    /* Test 3: AGGTAB and GXTXAYB -> LCS length 4, LCS "GTAB" (textbook LCS) */
    {
        const char *s1 = "AGGTAB";
        const char *s2 = "GXTXAYB";
        int len = compute_lcs(s1, s2, strlen(s1), strlen(s2), lcs, idx1, idx2);
        assert_test(len == 4, "Test 3: AGGTAB and GXTXAYB has LCS length 4");
        assert_test(strcmp(lcs, "GTAB") == 0, "Test 3: LCS string is 'GTAB'");
    }

    /* Test 4: Identical sequences */
    {
        const char *s1 = "ATGCATGC";
        const char *s2 = "ATGCATGC";
        int len = compute_lcs(s1, s2, strlen(s1), strlen(s2), lcs, idx1, idx2);
        assert_test(len == (int)strlen(s1), "Test 4: Identical sequences LCS length equals sequence length");
        assert_test(strcmp(lcs, s1) == 0, "Test 4: Identical sequences LCS string matches input");
        double score = calculate_overlap_score(len, strlen(s1), strlen(s2));
        assert_test(score >= 99.99 && score <= 100.01, "Test 4: Overlap score is 100%");
    }

    /* Test 5: Completely different sequences (no common characters) */
    {
        const char *s1 = "AAAA";
        const char *s2 = "TTTT";
        int len = compute_lcs(s1, s2, strlen(s1), strlen(s2), lcs, idx1, idx2);
        assert_test(len == 0, "Test 5: Completely different sequences has LCS length 0");
        assert_test(strlen(lcs) == 0, "Test 5: LCS string is empty");
        double score = calculate_overlap_score(len, strlen(s1), strlen(s2));
        assert_test(score == 0.0, "Test 5: Overlap score is 0.00%");
    }

    /* Test 6: Invalid characters rejection */
    {
        char bad_char = '\0';
        int valid1 = validate_sequence("ATGCXG", 0, &bad_char);
        assert_test(valid1 == 0 && bad_char == 'X', "Test 6a: Character 'X' rejected in DNA sequence");
        int valid2 = validate_sequence("ATG1CG", 0, &bad_char);
        assert_test(valid2 == 0 && bad_char == '1', "Test 6b: Character '1' rejected in DNA sequence");
        int valid3 = validate_sequence("ATGCCG", 0, &bad_char);
        assert_test(valid3 == 1, "Test 6c: Valid sequence 'ATGCCG' accepted");
    }

    /* Test 7: Lowercase input normalization to uppercase */
    {
        char test_str[] = "atgccgta\n";
        normalize_sequence(test_str);
        assert_test(strcmp(test_str, "ATGCCGTA") == 0, "Test 7: Lowercase input normalized to uppercase and newline stripped");
    }

    /* Test 8: Empty sequences handling safely */
    {
        const char *s1 = "";
        const char *s2 = "";
        int len = compute_lcs(s1, s2, 0, 0, lcs, idx1, idx2);
        assert_test(len == 0, "Test 8a: Both empty sequences has LCS length 0");
        assert_test(strlen(lcs) == 0, "Test 8b: LCS string is empty");
        double score = calculate_overlap_score(len, 0, 0);
        assert_test(score == 0.0, "Test 8c: Overlap score for empty sequences is safely 0.00% without zero division");

        const char *s3 = "ATGC";
        const char *s4 = "";
        len = compute_lcs(s3, s4, 4, 0, lcs, idx1, idx2);
        assert_test(len == 0, "Test 8d: One empty sequence has LCS length 0");
    }

    /* Test 9: Maximum supported sequence length bounds check */
    {
        char long_seq1[MAX_SEQ_LEN + 1];
        char long_seq2[MAX_SEQ_LEN + 1];
        for (int i = 0; i < MAX_SEQ_LEN; i++) {
            if (i % 2 == 0) {
                long_seq1[i] = 'A';
                long_seq2[i] = 'A';
            } else {
                long_seq1[i] = 'T';
                long_seq2[i] = 'T';
            }
        }
        long_seq1[MAX_SEQ_LEN] = '\0';
        long_seq2[MAX_SEQ_LEN] = '\0';
        int len = compute_lcs(long_seq1, long_seq2, MAX_SEQ_LEN, MAX_SEQ_LEN, lcs, idx1, idx2);
        assert_test(len == MAX_SEQ_LEN, "Test 9: Max length (500) sequence computes correctly without buffer overflow");
    }

    /* Test 12: DP Matrix dimensions and index tracking correctness */
    {
        const char *s1 = "AGC";
        const char *s2 = "AC";
        int len = compute_lcs(s1, s2, 3, 2, lcs, idx1, idx2);
        assert_test(len == 2, "Test 12a: LCS of AGC and AC is length 2");
        assert_test(strcmp(lcs, "AC") == 0, "Test 12b: LCS is 'AC'");
        assert_test(idx1[0] == 0 && idx1[1] == 2, "Test 12c: Seq1 match indices are [0, 2]");
        assert_test(idx2[0] == 0 && idx2[1] == 1, "Test 12d: Seq2 match indices are [0, 1]");
        /* Matrix check: dp[3][2] should be 2, dp[0][0]=0 */
        assert_test(dp[3][2] == 2, "Test 12e: dp[3][2] == 2");
        assert_test(dp[0][0] == 0, "Test 12f: dp[0][0] == 0");
    }

    printf("\n====================================================\n");
    printf("   Summary: %d / %d tests passed\n", tests_passed, tests_run);
    printf("====================================================\n");

    if (tests_passed == tests_run) {
        return 0;
    } else {
        return 1;
    }
}
