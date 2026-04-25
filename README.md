# Chess Engine: AlphaZero-Style Native C++ Implementation

A high-performance C++ chess engine combining low-level bit-manipulation architecture with a deep reinforcement learning pipeline.

---

## 🏗️ Pillar I: The Bitboard Infrastructure (Systems Engineering)
This layer handles the "physics" of the game. To achieve competitive performance, the engine utilizes a **Bitboard-centric architecture**, shifting complexity from expensive runtime computations to optimized memory-based lookups.

### 1. Magic Bitboards & Move Generation
Instead of calculating sliding piece attacks (Rooks/Bishops) on every frame, the engine utilizes the **Magic Bitboard** algorithm.
* **Optimization:** Pre-calculated attack patterns are stored in memory and accessed via bit-multiplication and hashing.
* **Result:** Move generation complexity is effectively **$O(1)$**, allowing the engine to search millions of positions per second.

### 2. Zobrist Hashing & State Tracking
To handle the **50-move rule** and **Threefold Repetition** efficiently:
* **The Solution:** Every piece/square combination is assigned a unique 64-bit number using a pseudo-random number generator.
* **Efficiency:** Entire board states are compared in a single 64-bit operation. The historical stack allows for nearly instantaneous state verification.

---

## 🧠 Pillar II: Neural Reinforcement Learning (AI Research)
This layer handles "strategy" and "evaluation." Inspired by DeepMind’s **AlphaZero**, the engine replaces hand-crafted heuristics with a self-improving neural pipeline.

### 1. Parallel Monte Carlo Tree Search (MCTS)
The search is powered by a custom MCTS implementation integrated with **LibTorch**.
* **Tensor Batching:** To solve the CPU-GPU communication bottleneck, the search is engineered to group leaf-node evaluations into batches of **128** (`batch_size`).
* **Throughput:** This allows the engine to fully saturate GPU compute units during the selection and expansion phases of the search tree.

### 2. Dual-Head ResNet Architecture
The engine utilizes a **Residual Network** with two distinct output heads:
* **Policy Head:** Provides a probability distribution over all legal moves.
* **Value Head:** Provides a scalar estimation (win/loss/draw) of the current position.

### 3. RL Hyperparameters
| Parameter | Value | Role in Learning |
| :--- | :--- | :--- |
| `NUM_SEARCHES` | `800` | Simulation budget per move; balances depth vs. inference time. |
| `C` (CPUCT) | `2` | Controls the UCB1 trade-off between exploration and exploitation. |
| `DIRICHLET_EPSILON` | `0.25` | Fraction of noise added to root node priors to maintain opening diversity. |
| `DIRICHLET_ALPHA` | `0.3` | Concentration parameter for Dirichlet noise (tuned for chess branching factor). |
| `temperature` | `1.25` | Controls move stochasticity (Exploration $> 1$, Exploitation $< 1$). |

---

## 💾 Memory Allocation & Data Structures
The engine is designed for extreme memory efficiency, targeting the CPU cache to minimize latency.

### 1. Pre-Calculated Attack Tables (Static Allocation)
| Array | Type | Description |
| :--- | :--- | :--- |
| `PawnAttacks[2][64]` | `uint64_t` | Pre-computed pawn captures for both colors. |
| `RookAttacks[64][4096]` | `uint64_t` | Magic lookup table for Rook moves. |
| `BishopAttacks[64][512]` | `uint64_t` | Magic lookup table for Bishop moves. |
| `kingMoves[64]` / `knightMoves[64]` | `uint64_t` | Static move patterns for King and Knight. |
| `CheckRays[64][64]` | `uint64_t` | Pre-computed paths between squares for pin/check logic. |

### 2. Hashing & Game State (Dynamic/Stack Allocation)
| Variable | Type | Purpose |
| :--- | :--- | :--- |
| `zobristTable[768]` | `uint64_t` | `[Player][PieceType][Square]` hashing constants. |
| `zobristHash[50]` | `uint64_t` | History stack for repetition detection. |
| `Boards[2][8]` | `BitBoard*` | Current bitboard representation per piece and color. |
| `pinMap[64]` | `uint8_t` | Map identifying currently pinned pieces. |

### ⚡ Cache Efficiency & Optimization
* **Total Memory Footprint:** ~**64 KB**.
* **L3 Cache Targeting:** By keeping the entire engine state (move generation + hashing) within the L3 cache, we eliminate the latency of fetching data from system RAM. 
* **No-Allocation Search:** The engine uses a "Make/Undo" mechanic rather than copying board objects, ensuring zero heap allocations during search tree traversal.

---

## 🔮 Future Works & Research Directions
The current implementation serves as a high-performance baseline for Reinforcement Learning research. Future iterations will focus on further reducing the memory bottleneck and increasing search throughput.

### 1. Optimized State Representation
* **The Problem:** While bitboards are efficient, the large memory overhead of copying entire chessboard states during deep MCTS expansions can lead to cache pressure and slower iteration cycles.
* **The Strategy:** I am experimenting with compressed board representations and more efficient data packing to reduce the memory footprint of each node in the search tree, which is critical for faster convergence in deep RL models.

### 2. Zero-Allocation Search Mechanics
* **Implementation:** Refining the **"Make/Undo"** move mechanic to replace state-cloning. By updating the existing board state and backtracking, the engine ensures **zero heap allocations** during search tree traversal.
* **Impact:** This minimizes the overhead on the system allocator and ensures that the CPU remains focused on move generation and neural inference rather than memory management.

### 3. Distributed Self-Play
* Expanding the training pipeline to support distributed data generation across multiple nodes to accelerate the AlphaZero self-play loop and reduce the time to reach Grandmaster-level proficiency.

---

## 🛠️ Tech Stack
* **Language:** C++20
* **Deep Learning:** LibTorch (PyTorch C++ Backend)
* **Graphics/UI:** SFML (Chessboard Visualization & Debugging)

---
**Developed by Ahmad Zeeshan** | *Focused on High-Performance AI & Systems Engineering*
