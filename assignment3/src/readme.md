# Dynamic Memory Manager Module - Lab 3

## Name: James Kyoung Ho Kim
## Student ID: 20178-13897

### Overview

This project implements a dynamic memory management module in C for doubly linked pointers

### Implementations

- **heapmgr1.c**: Based off Skeleton Code
- **heapmgr2.c**: Based off Skeleton Code

### Testing and Performance

The performance of each implementation was evaluated using the provided test scripts (`testheap1`, `testheap2`)

### CPU Times and Memory Usage

The following table summarizes the CPU times and heap memory consumed by the different implementations during testing:


# Test Results (HEAPMGR1 )

## HEAPMGR1 Chunk Size: 1000

| Executable          | Test         | Count   | Size | Time  | Memory     |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrgnu    | LIFO_fixed   | 100000  | 1000 | 0.04  | 100700160  |
| ./testheapmgrgnu    | FIFO_fixed   | 100000  | 1000 | 0.03  | 8192       |
| ./testheapmgrgnu    | LIFO_random  | 100000  | 1000 | 0.02  | 51499008   |
| ./testheapmgrgnu    | FIFO_random  | 100000  | 1000 | 0.02  | 536576     |
| ./testheapmgrgnu    | random_fixed  | 100000  | 1000 | 0.01  | 24915968   |
| ./testheapmgrgnu    | random_random | 100000  | 1000 | 0.01  | 16220160   |
| ./testheapmgrgnu    | worst        | 100000  | 1000 | 0.01  | 27303936   |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrkr     | LIFO_fixed   | 100000  | 1000 | 0.03  | 102400000  |
| ./testheapmgrkr     | FIFO_fixed   | 100000  | 1000 | 0.03  | 102400000  |
| ./testheapmgrkr     | LIFO_random  | 100000  | 1000 | 0.16  | 52576256   |
| ./testheapmgrkr     | FIFO_random  | 100000  | 1000 | 0.12  | 52576256   |
| ./testheapmgrkr     | random_fixed  | 100000  | 1000 | 1.40  | 32440320   |
| ./testheapmgrkr     | random_random | 100000  | 1000 | 0.60  | 16629760   |
| ./testheapmgrkr     | worst        | 100000  | 1000 | 3.88  | 28278784   |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrbase   | LIFO_fixed   | 100000  | 1000 | 0.04  | 102500000  |
| ./testheapmgrbase   | FIFO_fixed   | 100000  | 1000 | 0.04  | 102500000  |
| ./testheapmgrbase   | LIFO_random  | 100000  | 1000 | 0.02  | 52496400   |
| ./testheapmgrbase   | FIFO_random  | 100000  | 1000 | 0.02  | 52496400   |
| ./testheapmgrbase   | random_fixed  | 100000  | 1000 | 0.63  | 32504800   |
| ./testheapmgrbase   | random_random | 100000  | 1000 | 0.48  | 16531200   |
| ./testheapmgrbase   | worst        | 100000  | 1000 | 3.55  | 27863600   |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgr1      | LIFO_fixed   | 100000  | 1000 | 0.06  | 105600000  |
| ./testheapmgr1      | FIFO_fixed   | 100000  | 1000 | 0.06  | 105600000  |
| ./testheapmgr1      | LIFO_random  | 100000  | 1000 | 0.04  | 56054640   |
| ./testheapmgr1      | FIFO_random  | 100000  | 1000 | 0.04  | 56054640   |
| ./testheapmgr1      | random_fixed  | 100000  | 1000 | 0.02  | 33439296   |
| ./testheapmgr1      | random_random | 100000  | 1000 | 0.02  | 17802096   |
| ./testheapmgr1      | worst        | 100000  | 1000 | 8.45  | 31603200   |

## HEAPMGR1 Chunk Size: 10000

| Executable          | Test         | Count   | Size  | Time  | Memory      |
|---------------------|--------------|---------|-------|-------|-------------|
| ./testheapmgrgnu    | LIFO_fixed   | 100000  | 10000 | 0.32  | 0           |
| ./testheapmgrgnu    | FIFO_fixed   | 100000  | 10000 | 0.12  | 0           |
| ./testheapmgrgnu    | LIFO_random  | 100000  | 10000 | 0.09  | 500830208   |
| ./testheapmgrgnu    | FIFO_random  | 100000  | 10000 | 0.10  | 62382080    |
| ./testheapmgrgnu    | random_fixed  | 100000  | 10000 | 0.05  | 0           |
| ./testheapmgrgnu    | random_random | 100000  | 10000 | 0.04  | 159399936   |
| ./testheapmgrgnu    | worst        | 100000  | 10000 | 0.04  | 252338176   |
|---------------------|--------------|---------|-------|-------|-------------|
| ./testheapmgrkr     | LIFO_fixed   | 100000  | 10000 | Killed|             |
| ./testheapmgrkr     | FIFO_fixed   | 100000  | 10000 | Killed|             |
| ./testheapmgrkr     | LIFO_random  | 100000  | 10000 | Killed|             |
| ./testheapmgrkr     | FIFO_random  | 100000  | 10000 | Killed|             |
| ./testheapmgrkr     | random_fixed  | 100000  | 10000 | Killed|             |
| ./testheapmgrkr     | random_random | 100000  | 10000 | 7.25  | 165052416   |
| ./testheapmgrkr     | worst        | 100000  | 10000 | Killed|             |
|---------------------|--------------|---------|-------|-------|-------------|
| ./testheapmgrbase   | LIFO_fixed   | 100000  | 10000 | Killed|             |
| ./testheapmgrbase   | FIFO_fixed   | 100000  | 10000 | Killed|             |
| ./testheapmgrbase   | LIFO_random  | 100000  | 10000 | 22.96 | 508072000   |
| ./testheapmgrbase   | FIFO_random  | 100000  | 10000 | 13.93 | 507006000   |
| ./testheapmgrbase   | random_fixed  | 100000  | 10000 | 12.50 | 371017200   |
| ./testheapmgrbase   | random_random | 100000  | 10000 | 4.12  | 161244800   |
| ./testheapmgrbase   | worst        | 100000  | 10000 | 6.29  | 344564000   |
|---------------------|--------------|---------|-------|-------|-------------|
| ./testheapmgr1      | LIFO_fixed   | 100000  | 10000 | 0.21  | 1005600000  |
| ./testheapmgr1      | FIFO_fixed   | 100000  | 10000 | 0.21  | 1005600000  |
| ./testheapmgr1      | LIFO_random  | 100000  | 10000 | 0.17  | 505342416    |
| ./testheapmgr1      | FIFO_random  | 100000  | 10000 | 0.17  | 505342416    |
| ./testheapmgr1      | random_fixed  | 100000  | 10000 | 0.08  | 318855648    |
| ./testheapmgr1      | random_random | 100000  | 10000 | 0.06  | 160130352    |
| ./testheapmgr1      | worst        | 100000  | 10000 | 19.54 | 256600320    |

# Test Results (HEAPMGR2)

## HEAPMGR2 Chunk Size: 1000

| Executable          | Test         | Count   | Size | Time  | Memory     |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrgnu    | LIFO_fixed   | 100000  | 1000 | 0.04  | 100700160  |
| ./testheapmgrgnu    | FIFO_fixed   | 100000  | 1000 | 0.03  | 8192       |
| ./testheapmgrgnu    | LIFO_random  | 100000  | 1000 | 0.02  | 51363840   |
| ./testheapmgrgnu    | FIFO_random  | 100000  | 1000 | 0.02  | 51363840   |
| ./testheapmgrgnu    | random_fixed  | 100000  | 1000 | 0.01  | 28987392   |
| ./testheapmgrgnu    | random_random | 100000  | 1000 | 0.01  | 16220160   |
| ./testheapmgrgnu    | worst        | 100000  | 1000 | 0.01  | 27303936   |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrgnu    | LIFO_fixed   | 100000  | 10000 | 0.32  | 0          |
| ./testheapmgrgnu    | FIFO_fixed   | 100000  | 10000 | 0.12  | 0          |
| ./testheapmgrgnu    | LIFO_random  | 100000  | 10000 | 0.09  | 501583872   |
| ./testheapmgrgnu    | FIFO_random  | 100000  | 10000 | 0.10  | 42721280   |
| ./testheapmgrgnu    | random_fixed  | 100000  | 10000 | 0.05  | 0          |
| ./testheapmgrgnu    | random_random | 100000  | 10000 | 0.04  | 158220288  |
| ./testheapmgrgnu    | worst        | 100000  | 10000 | 0.05  | 252338176  |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrkr     | LIFO_fixed   | 100000  | 1000 | 0.03  | 102400000  |
| ./testheapmgrkr     | FIFO_fixed   | 100000  | 1000 | 0.03  | 102400000  |
| ./testheapmgrkr     | LIFO_random  | 100000  | 1000 | 0.16  | 52707328   |
| ./testheapmgrkr     | FIFO_random  | 100000  | 1000 | 0.12  | 52707328   |
| ./testheapmgrkr     | random_fixed  | 100000  | 1000 | 1.35  | 32456704   |
| ./testheapmgrkr     | random_random | 100000  | 1000 | 0.59  | 16662528   |
| ./testheapmgrkr     | worst        | 100000  | 1000 | 3.91  | 28278784   |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrkr     | LIFO_fixed   | 100000  | 10000 | Killed |           |
| ./testheapmgrkr     | FIFO_fixed   | 100000  | 10000 | Killed |           |
| ./testheapmgrkr     | LIFO_random  | 100000  | 10000 | Killed |           |
| ./testheapmgrkr     | FIFO_random  | 100000  | 10000 | Killed |           |
| ./testheapmgrkr     | random_fixed  | 100000  | 10000 | Killed |           |
| ./testheapmgrkr     | random_random | 100000  | 10000 | 7.12  | 164773888  |
| ./testheapmgrkr     | worst        | 100000  | 10000 | Killed |           |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrbase   | LIFO_fixed   | 100000  | 1000 | 0.04  | 102500000  |
| ./testheapmgrbase   | FIFO_fixed   | 100000  | 1000 | 0.04  | 102500000  |
| ./testheapmgrbase   | LIFO_random  | 100000  | 1000 | 0.02  | 52463600   |
| ./testheapmgrbase   | FIFO_random  | 100000  | 1000 | 0.02  | 52463600   |
| ./testheapmgrbase   | random_fixed  | 100000  | 1000 | 0.62  | 32472000   |
| ./testheapmgrbase   | random_random | 100000  | 1000 | 0.49  | 16711600   |
| ./testheapmgrbase   | worst        | 100000  | 1000 | 3.74  | 27863600   |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgrbase   | LIFO_fixed   | 100000  | 10000 | Killed |           |
| ./testheapmgrbase   | FIFO_fixed   | 100000  | 10000 | Killed |           |
| ./testheapmgrbase   | LIFO_random  | 100000  | 10000 | 24.02 | 506366400  |
| ./testheapmgrbase   | FIFO_random  | 100000  | 10000 | 14.01 | 506464800  |
| ./testheapmgrbase   | random_fixed  | 100000  | 10000 | 12.05 | 370590800  |
| ./testheapmgrbase   | random_random | 100000  | 10000 | 3.98  | 161064400  |
| ./testheapmgrbase   | worst        | 100000  | 10000 | 6.16  | 344564000  |
|---------------------|--------------|---------|------|-------|------------|
| ./testheapmgr1      | LIFO_fixed   | 100000  | 1000 | 0.06  | 105600000  |
| ./testheapmgr1      | FIFO_fixed   | 100000  | 1000 | 0.07  | 105600000  |
| ./testheapmgr1      | LIFO_random  | 100000  | 1000 | 0.04  | 55869264   |
| ./testheapmgr1      | FIFO_random  | 100000  | 1000 | 0.04  | 55869264   |
| .
