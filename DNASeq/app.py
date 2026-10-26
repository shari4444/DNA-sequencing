#!/usr/bin/env python3
"""
Project: DNASeq — DNA Sequence Comparison Using Dynamic Programming
File:    app.py
Purpose: Lightweight Flask backend integrating web frontend with compiled C binary.
Course:  Analysis of Algorithms (AOA) - Second Year Engineering Mini Project
"""

import json
import os
import platform
import subprocess
import sys
from flask import Flask, jsonify, request, send_from_directory

app = Flask(__name__, static_folder=".")

# Constants
MAX_SEQUENCE_LENGTH = 500
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Binary executable name based on OS
if platform.system() == "Windows":
    BINARY_NAME = "dna_lcs.exe"
else:
    BINARY_NAME = "dna_lcs"

BINARY_PATH = os.path.join(BASE_DIR, BINARY_NAME)
C_SOURCE_PATH = os.path.join(BASE_DIR, "main.c")


def ensure_c_binary_compiled() -> bool:
    """
    Ensures that the C executable is compiled and up-to-date.
    If binary does not exist or source file is newer, attempts compilation using gcc or clang.
    """
    needs_compile = False
    if not os.path.exists(BINARY_PATH):
        needs_compile = True
    elif os.path.exists(C_SOURCE_PATH):
        if os.path.getmtime(C_SOURCE_PATH) > os.path.getmtime(BINARY_PATH):
            needs_compile = True

    if not needs_compile:
        return True

    # Identify compiler (prefer gcc, fallback to clang)
    compiler = "gcc"
    try:
        subprocess.run(["gcc", "--version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
    except Exception:
        compiler = "clang"

    compile_cmd = [compiler, "-O2", "-Wall", "-Wextra", "-pedantic", "-o", BINARY_PATH, C_SOURCE_PATH]
    try:
        result = subprocess.run(compile_cmd, capture_output=True, text=True, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"[Error] Failed to compile {C_SOURCE_PATH}: {e.stderr}", file=sys.stderr)
        return False
    except Exception as e:
        print(f"[Error] Could not invoke compiler: {e}", file=sys.stderr)
        return False


# Static File Routes
@app.route("/")
def index():
    """Serves the main single-page application HTML."""
    return send_from_directory(BASE_DIR, "index.html")


@app.route("/style.css")
def style_css():
    """Serves the CSS stylesheet."""
    return send_from_directory(BASE_DIR, "style.css")


@app.route("/script.js")
def script_js():
    """Serves the client-side JavaScript logic."""
    return send_from_directory(BASE_DIR, "script.js")


@app.route("/api/health", methods=["GET"])
def health_check():
    """Health check endpoint to verify backend status and C binary readiness."""
    compiled = ensure_c_binary_compiled()
    return jsonify({
        "status": "healthy" if compiled else "compiler_error",
        "c_binary_ready": compiled,
        "max_length": MAX_SEQUENCE_LENGTH,
        "platform": platform.system()
    }), (200 if compiled else 500)


@app.route("/api/compare", methods=["POST"])
def compare_sequences():
    """
    Compares two DNA sequences using the compiled C LCS executable.
    
    Expected JSON payload:
    {
        "seq1": "ATGCCGTA",
        "seq2": "ATGCGTA",
        "allow_general": false  (optional: true allows general A-Z textbook examples)
    }
    """
    if not request.is_json:
        return jsonify({"success": False, "error": "Request body must be valid JSON."}), 400

    data = request.get_json()
    if not isinstance(data, dict):
        return jsonify({"success": False, "error": "Invalid JSON object format."}), 400

    raw_seq1 = data.get("seq1", "")
    raw_seq2 = data.get("seq2", "")
    allow_general = bool(data.get("allow_general", False))

    if not isinstance(raw_seq1, str) or not isinstance(raw_seq2, str):
        return jsonify({"success": False, "error": "Sequences must be string values."}), 400

    # Normalize inputs: remove surrounding whitespace and convert to uppercase
    seq1 = raw_seq1.strip().upper()
    seq2 = raw_seq2.strip().upper()

    # Enforce maximum sequence length
    if len(seq1) > MAX_SEQUENCE_LENGTH:
        return jsonify({
            "success": False,
            "error": f"Sequence 1 length ({len(seq1)}) exceeds maximum limit of {MAX_SEQUENCE_LENGTH} nucleotides."
        }), 400

    if len(seq2) > MAX_SEQUENCE_LENGTH:
        return jsonify({
            "success": False,
            "error": f"Sequence 2 length ({len(seq2)}) exceeds maximum limit of {MAX_SEQUENCE_LENGTH} nucleotides."
        }), 400

    # Independent backend validation
    valid_dna_set = {"A", "T", "G", "C"}
    for idx, char in enumerate(seq1):
        if not allow_general:
            if char not in valid_dna_set:
                return jsonify({
                    "success": False,
                    "error": f"Sequence 1 contains invalid nucleotide '{char}' at position {idx + 1}. Only A, T, G, and C are allowed."
                }), 400
        else:
            if not ('A' <= char <= 'Z'):
                return jsonify({
                    "success": False,
                    "error": f"Sequence 1 contains invalid character '{char}' at position {idx + 1}. Only A-Z are allowed."
                }), 400

    for idx, char in enumerate(seq2):
        if not allow_general:
            if char not in valid_dna_set:
                return jsonify({
                    "success": False,
                    "error": f"Sequence 2 contains invalid nucleotide '{char}' at position {idx + 1}. Only A, T, G, and C are allowed."
                }), 400
        else:
            if not ('A' <= char <= 'Z'):
                return jsonify({
                    "success": False,
                    "error": f"Sequence 2 contains invalid character '{char}' at position {idx + 1}. Only A-Z are allowed."
                }), 400

    # Ensure C executable is present
    if not ensure_c_binary_compiled():
        return jsonify({
            "success": False,
            "error": "Failed to compile the core C algorithm binary (dna_lcs). Please check C compiler installation."
        }), 500

    # Build command line argument list (NO shell=True)
    cmd = [BINARY_PATH, "--json"]
    if allow_general:
        cmd.append("--general")

    # Pass sequences via standard input
    stdin_payload = f"{seq1}\n{seq2}\n"

    try:
        proc = subprocess.run(
            cmd,
            input=stdin_payload,
            capture_output=True,
            text=True,
            timeout=5.0,
            shell=False
        )
    except subprocess.TimeoutExpired:
        return jsonify({
            "success": False,
            "error": "Execution timed out while computing LCS in the C module."
        }), 504
    except Exception as e:
        return jsonify({
            "success": False,
            "error": f"Failed to execute C program: {str(e)}"
        }), 500

    # Parse JSON output from C binary
    c_stdout = proc.stdout.strip()
    if not c_stdout:
        return jsonify({
            "success": False,
            "error": f"C executable terminated with code {proc.returncode} but produced no output. Stderr: {proc.stderr.strip()}"
        }), 500

    try:
        result_json = json.loads(c_stdout)
    except json.JSONDecodeError:
        return jsonify({
            "success": False,
            "error": "Failed to parse structured output from C executable.",
            "raw_output": c_stdout
        }), 500

    # If the C binary itself returned an error response
    if proc.returncode != 0 or not result_json.get("success", False):
        err_msg = result_json.get("error", "Error executing LCS computation in C binary.")
        return jsonify({"success": False, "error": err_msg}), 400

    # Add educational metadata
    result_json["educational_disclaimer"] = (
        "This application is an educational demonstration. An LCS-based overlap score is not "
        "equivalent to validated biological sequence identity and cannot establish genetic relatedness, "
        "disease risk, or biological function."
    )
    # Flag whether matrix is small enough for complete visual table rendering
    result_json["render_matrix"] = (len(seq1) <= 35 and len(seq2) <= 35)

    return jsonify(result_json), 200


if __name__ == "__main__":
    # Ensure binary compiled before starting server
    ensure_c_binary_compiled()
    port = int(os.environ.get("PORT", 5001))
    print(f"================================================================")
    print(f"  Starting DNASeq Server at http://127.0.0.1:{port}")
    print(f"  Analysis of Algorithms (AOA) - Dynamic Programming Project")
    print(f"================================================================")
    app.run(host="127.0.0.1", port=port, debug=True)
