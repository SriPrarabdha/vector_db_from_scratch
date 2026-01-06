# VectorDB — A Vector Database Built From First Principles (Pure C++)

> **A research-grade, production-inspired vector database implemented entirely from scratch in modern C++.**  
> No Faiss. No Annoy. No shortcuts. Just fundamentals, performance, and clarity.
<br>
> Work under Progress !!!!
---

## ✨ Why This Project Exists

Vector databases power **semantic search, recommendation systems, RAG pipelines, and multimodal AI**.  
Yet most practitioners interact with them as *black boxes*.

This project exists to **demystify modern vector retrieval systems** by rebuilding them **from first principles**, while still respecting the **design constraints of real production systems**:
- latency
- memory
- recall
- scalability
- persistence

**VectorDB is not a wrapper.**  
It is an educational + systems-engineering project that re-derives state-of-the-art ANN techniques step by step.

---

## 🧠 Project Philosophy

### 1. First principles > abstractions
Every algorithm is implemented **from scratch**:
- distance functions
- indexing structures
- graph construction
- quantization
- pruning heuristics

If it’s here, it’s because it was implemented — not imported.

---

<!-- ### 2. Measure everything
All design decisions are **empirically validated**:
- recall@k
- latency (p50 / p95)
- memory footprint
- nodes visited
- pruning efficiency

No claims without benchmarks. -->

---

<!-- ### 3. Correctness first, approximation second
Every approximate method is:
1. Verified against **exact linear scan**
2. Measured for **accuracy degradation**
3. Tuned for **performance/recall tradeoffs**

--- -->

### 2. Research-grade ideas, production-style engineering
This project intentionally bridges:
- **ANN research literature**
- **real-world vector DB engineering**

Features like persistence, memory layout optimization, and Python bindings are treated as first-class citizens.

---

<!-- ### 5. Readable, hackable, extensible
The codebase is:
- modular
- well-documented
- designed for experimentation

Contributors should be able to:
> “Swap one idea and observe the impact.”

--- -->

## 🚀 What This Project Implements

### 🔹 Baseline & Exact Methods
- **Linear Scan**
  - brute-force kNN
  - correctness oracle
- **KD-Tree**
  - recursive axis-aligned splitting
  - branch-and-bound pruning
  - empirical study of high-dimensional failure

---

### 🔹 Inverted Indexing
- **IVF (Inverted File Index)**
  - k-means coarse quantization
  - controllable `nlist` / `nprobe`
- **IVF + PQ**
  - product quantization
  - compressed storage
  - asymmetric distance computation (ADC)

---

### 🔹 Graph-Based ANN (HNSW)
Implemented closely to the original paper — **without copying code**:
- multi-layer navigable small world graph
- skip-list inspired level assignment
- greedy descent on upper layers
- beam search on layer 0
- bidirectional edge creation
- degree-bounded neighbor lists
- **diverse neighbor selection heuristics**
- pruning to prevent long-range edge clustering

---

### 🔹 Hybrid Indexes
- IVF-routed HNSW
- Demonstrates how modern systems combine **coarse quantization + graph search**

---

### 🔹 Systems & Infrastructure
- Memory-efficient layouts
- SIMD-friendly distance loops
- Persistent on-disk index format
- `mmap`-based fast reload
- Multi-threaded index construction
- Python bindings via `pybind11`
- CLI and benchmark runners

---

## 📊 Benchmarks 

Linear Index Performance with Scalar and AVX2 Based Implementation

Dataset size : 100000 , Dimension : 1024 , Queries : 100  ,Top-K : 10

| Implementation Type | Search Time | Queries Per Second (QPS) | Instructions per Cycle |
|---------------------|-------------|---------------------------|-------------------------|
| Scalar              | 7.792 sec   | 12.83                     | 0.97                    |
| AVX2                | 2.339 sec   | 42.74                     | 1.04                    |

Performance Comparison between KD Tree and Linear Scan Index

Dataset size : 100000 , Dimension  , Queries : 1  ,Top-K : 10

| Dimension | Linear Search Time | KD-Tree Search Time | Visited Nodes | Pruned Branches |
|-----------|--------------------|---------------------|---------------|-----------------|
| 4         | 2.15 ms            | 0.088 ms            | 376           |129              |
| 8         | 2.74 ms            | 1.32 ms             | 8,982         |3432             |
| 32        | 5.59 ms            | 35.75 ms            | 100,000       |0                |
| 128       | 17.11 ms           | 64.73 ms            | 100,000       |0                |
| 1024      | 158.299 ms         | 188.459 ms          | 100,000       |0                |


---

## 🧪 How to Run

```bash
mkdir build && cd build
cmake --build . -j$(nproc)
perf stat ./bench/bench_linear --type scalar --threads 1 --structure aos
```

