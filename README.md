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

### 2. Measure everything
All design decisions are **empirically validated**:
- recall@k
- latency (p50 / p95)
- memory footprint
- nodes visited
- pruning efficiency

No claims without benchmarks.

---

### 3. Correctness first, approximation second
Every approximate method is:
1. Verified against **exact linear scan**
2. Measured for **accuracy degradation**
3. Tuned for **performance/recall tradeoffs**

---

### 4. Research-grade ideas, production-style engineering
This project intentionally bridges:
- **ANN research literature**
- **real-world vector DB engineering**

Features like persistence, memory layout optimization, and Python bindings are treated as first-class citizens.

---

### 5. Readable, hackable, extensible
The codebase is:
- modular
- well-documented
- designed for experimentation

Contributors should be able to:
> “Swap one idea and observe the impact.”

---

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

## 📊 Benchmarks (Example)

| Index        | Recall@10 | Latency (ms) | Memory (MB) |
|-------------|-----------|--------------|-------------|
| Linear Scan | 1.00      | 120.4        | 512         |
| KD-Tree    | 1.00      | 35.2         | 540         |
| IVF        | 0.91      | 4.8          | 180         |
| IVF + PQ   | 0.86      | 2.1          | 64          |
| HNSW       | 0.95      | 1.6          | 220         |
| IVF+HNSW   | 0.94      | 0.9          | 140         |

> Full benchmark scripts and reproducibility instructions are provided.

---

## 🗂️ Repository Structure

core/ → vectors, distances, SIMD helpers
indexes/ → linear scan, KD-tree, IVF, PQ, HNSW
storage/ → persistence, mmap, serialization
api/ → C++ API + Python bindings
bench/ → datasets, metrics, benchmark runner
demo/ → CLI demo
tools/ → dataset generators


---

## 🧪 How to Run

```bash
mkdir build && cd build
cmake ..
make -j
./bench/bench
./demo/cli_demo
```

