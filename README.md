# Pseudo-Malloc
Project for the course [Sistemi Operativi](https://sites.google.com/diag.uniroma1.it/sistemi-operativi-2022-23) of Computer and Systems Engineering at Sapienza University of Rome, September 2023.

## Requirements
The aim of the project is to create a replacement of the lib function `malloc`.

The system relies on mmap for physical allocation of memory, but handles the requests in two different ways:
  - Request of less than PageSize / 4: uses a Buddy Allocator, implemented with a bitmap that manages 1 MB of memory.
  - Request of PageSize / 4 or more: uses `mmap`.

## Programming Languages
- C

## License
- MIT
