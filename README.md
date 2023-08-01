# Pseudo-Malloc
This project was created for the course "Sistemi Operativi" at Sapienza University of Rome.

The requirements are:
  The system relies on mmap for physical allocation of memory, but handles the requests in two different ways:
  1. Request of less than PageSize / 4: uses a Buddy Allocator, implemented with a bitmap that manages 1 MB of memory.
  2. Request of PageSize / 4 or more: uses mmap.

