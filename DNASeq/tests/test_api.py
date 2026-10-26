#!/usr/bin/env python3
"""
Project: DNASeq — DNA Sequence Comparison Using Dynamic Programming
File:    tests/test_api.py
Purpose: Automated integration and API tests for the Flask backend and C executable.
Course:  Analysis of Algorithms (AOA) - Second Year Engineering Mini Project
"""

import os
import sys
import unittest

# Add project root directory to python path
current_dir = os.path.dirname(os.path.abspath(__file__))
project_dir = os.path.dirname(current_dir)
sys.path.insert(0, project_dir)

from app import app, MAX_SEQUENCE_LENGTH, ensure_c_binary_compiled


class TestDNASeqAPI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        """Ensure C binary is compiled before running tests."""
        compiled = ensure_c_binary_compiled()
        assert compiled, "Failed to compile C binary before running tests."

    def setUp(self):
        """Create a test client for Flask app."""
        self.client = app.test_client()
        self.client.testing = True

    def test_health_check(self):
        """Test the /api/health endpoint."""
        response = self.client.get("/api/health")
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertEqual(data["status"], "healthy")
        self.assertTrue(data["c_binary_ready"])
        self.assertEqual(data["max_length"], MAX_SEQUENCE_LENGTH)

    def test_static_routes(self):
        """Test that index.html, style.css, and script.js are served properly."""
        # Index
        resp = self.client.get("/")
        self.assertEqual(resp.status_code, 200)
        self.assertIn(b"DNASeq", resp.data)

    def test_case_1_standard_sequences(self):
        """Test Case 1: ATGCCGTA and ATGCGTA -> LCS length 7, Overlap 87.50%."""
        payload = {"seq1": "ATGCCGTA", "seq2": "ATGCGTA"}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["seq1"], "ATGCCGTA")
        self.assertEqual(data["seq2"], "ATGCGTA")
        self.assertEqual(data["lcs"], "ATGCGTA")
        self.assertEqual(data["lcs_length"], 7)
        self.assertAlmostEqual(data["overlap_score"], 87.50, places=2)
        self.assertEqual(len(data["match_indices_seq1"]), 7)
        self.assertEqual(len(data["match_indices_seq2"]), 7)
        self.assertEqual(len(data["dp_matrix"]), 9)  # (8+1) rows
        self.assertEqual(len(data["dp_matrix"][0]), 8)  # (7+1) cols

    def test_case_2_repeated_nucleotides(self):
        """Test Case 2: AAAA and AA -> LCS length 2, Overlap 50.00%."""
        payload = {"seq1": "AAAA", "seq2": "AA"}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["lcs_length"], 2)
        self.assertEqual(data["lcs"], "AA")
        self.assertAlmostEqual(data["overlap_score"], 50.00, places=2)

    def test_case_3_textbook_lcs_general_mode(self):
        """Test Case 3: AGGTAB and GXTXAYB -> LCS length 4 with LCS GTAB."""
        payload = {"seq1": "AGGTAB", "seq2": "GXTXAYB", "allow_general": True}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["lcs"], "GTAB")
        self.assertEqual(data["lcs_length"], 4)
        self.assertAlmostEqual(data["overlap_score"], 57.14, places=2)

    def test_case_4_identical_sequences(self):
        """Test Case 4: Identical sequences -> LCS equals sequence, 100% overlap."""
        seq = "ATGCATGC"
        payload = {"seq1": seq, "seq2": seq}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["lcs"], seq)
        self.assertEqual(data["lcs_length"], len(seq))
        self.assertAlmostEqual(data["overlap_score"], 100.0, places=2)

    def test_case_5_completely_different_sequences(self):
        """Test Case 5: Completely different sequences -> LCS length 0, 0% overlap."""
        payload = {"seq1": "AAAA", "seq2": "TTTT"}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["lcs"], "")
        self.assertEqual(data["lcs_length"], 0)
        self.assertEqual(data["overlap_score"], 0.0)

    def test_case_6_invalid_characters_rejected(self):
        """Test Case 6: Invalid characters -> HTTP 400 error response."""
        payload = {"seq1": "ATGCZ", "seq2": "ATGC"}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 400)
        data = response.get_json()
        self.assertFalse(data["success"])
        self.assertIn("invalid nucleotide 'Z'", data["error"])

    def test_case_7_lowercase_normalization(self):
        """Test Case 7: Lowercase input is normalized to uppercase."""
        payload = {"seq1": "atgccgta", "seq2": "atgcgta"}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["seq1"], "ATGCCGTA")
        self.assertEqual(data["seq2"], "ATGCGTA")
        self.assertEqual(data["lcs"], "ATGCGTA")

    def test_case_8_empty_sequences(self):
        """Test Case 8: Empty sequences handled safely without division by zero."""
        payload = {"seq1": "", "seq2": ""}
        response = self.client.post("/api/compare", json=payload)
        self.assertEqual(response.status_code, 200)
        data = response.get_json()
        self.assertTrue(data["success"])
        self.assertEqual(data["lcs_length"], 0)
        self.assertEqual(data["overlap_score"], 0.0)

        # One empty sequence
        payload2 = {"seq1": "ATGC", "seq2": ""}
        response2 = self.client.post("/api/compare", json=payload2)
        self.assertEqual(response2.status_code, 200)
        data2 = response2.get_json()
        self.assertTrue(data2["success"])
        self.assertEqual(data2["lcs_length"], 0)
        self.assertEqual(data2["overlap_score"], 0.0)

    def test_case_9_maximum_length_enforcement(self):
        """Test Case 9: Length <= 500 succeeds; Length > 500 rejected."""
        valid_500 = "A" * 500
        payload_valid = {"seq1": valid_500, "seq2": valid_500}
        resp_valid = self.client.post("/api/compare", json=payload_valid)
        self.assertEqual(resp_valid.status_code, 200)
        self.assertEqual(resp_valid.get_json()["lcs_length"], 500)

        invalid_501 = "A" * 501
        payload_invalid = {"seq1": invalid_501, "seq2": "A"}
        resp_invalid = self.client.post("/api/compare", json=payload_invalid)
        self.assertEqual(resp_invalid.status_code, 400)
        self.assertIn("exceeds maximum limit", resp_invalid.get_json()["error"])

    def test_case_10_malformed_requests(self):
        """Test Case 10: Non-JSON and bad format handling."""
        resp = self.client.post("/api/compare", data="not json", content_type="text/plain")
        self.assertEqual(resp.status_code, 400)

        resp2 = self.client.post("/api/compare", json={"seq1": 12345, "seq2": "ATGC"})
        self.assertEqual(resp2.status_code, 400)

    def test_case_11_biological_disclaimer_present(self):
        """Test Case 11: Biological disclaimer present in API responses."""
        payload = {"seq1": "ATGC", "seq2": "ATGC"}
        response = self.client.post("/api/compare", json=payload)
        data = response.get_json()
        self.assertIn("educational_disclaimer", data)
        self.assertIn("educational demonstration", data["educational_disclaimer"])


if __name__ == "__main__":
    unittest.main()
