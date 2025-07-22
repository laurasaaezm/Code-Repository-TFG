# Quantum Noise Injection Accelerator – Code Repository

This repository contains all the C/C++ source code developed for the Bachelor's Thesis:  
**“FPGA-Based Quantum Noise Injection for STIM Error Correction Framework”**.

The repository is structured according to the major sections in the thesis report (Sections 3.3 and 3.4). Each folder includes the implementation file and its corresponding testbench used for simulation, debugging, or synthesis using Vitis HLS.

## 📁 Structure based on the memory of the project

### 📌 Section 3.3 – Key Building Blocks of the Proposed Design

#### 🔹 3.3.1.2 – Initial Error Injection Test (Failed)
**Folder:** `Initial Error Injection Test (Failed)`  
**Files:** `error_injection_failed.cpp`, `testbench.cpp`  
➡️ First implementation attempt. Synthesizes and simulates, but does not yield valid output. Included for documentation of early development steps.

#### 🔹 3.3.1.3 – Verifying AXI-Stream Communication Using Random Number Generator  
**Folder:** `Random Number Generation`  
**Files:** `random_number.cpp`, `testbench.cpp`  
➡️ Implements and tests a pseudo-random number generator using AXI-Stream interfaces.

#### 🔹 3.3.1.4 – Final Working Version of the Error Injection Logic  
**Folder:** `Functional Error Injection Logic`  
**Files:** `error_injection_logic.cpp`, `testbench.cpp`  
➡️ Correct probabilistic bit-flip implementation using threshold comparison logic.

#### 🔹 3.3.2 – BRAM Management and Parallel Output Preparation  
**Folder:** `BRAM Management`  
**Files:** `bram.cpp`, `testbench.cpp`  
➡️ Manages loading and reuse of probability vectors in BRAM to reduce latency.

---

### 📌 Section 3.4 – Integration and Final Implementation

#### 🔹 3.4.1 – First Attempt of Integrating BRAM-Based Error Injection with Parallel Output  
**Folder:** `Final Implementation First Attempt`  
**Files:** `noise_generation_v1.cpp`, `testbench.cpp`  
➡️ First integrated version combining BRAM and random error generation with parallelism.

#### 🔹 3.4.2 – Second Attempt of Integrating BRAM-Based Error Injection with Parallel Output  
**Folder:** `Final Implementation Second Attempt`  
**Files:** `noise_generation_v2.cpp`, `testbench.cpp`  
➡️ Improved version with better pipelining, HLS optimizations and interface management.

#### 🔹 3.4.3 – Final Working Implementation and Results  
**Folder:** `Functional Final Implementation`  
**Files:** `noise_generation_functional.cpp`, `testbench.cpp`  
➡️ Fully working version used for final validation with STIM integration.

**Folder:** `Final Implementation Results`  
**Files:** `noise_generation_results.cpp`, `testbench.cpp`  
➡️ Final version for synthesis reports and resource/timing evaluation.

---


## 🧾 License

This project is open for academic and research purposes. Please cite or credit the author if used in derived works.

