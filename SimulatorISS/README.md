# Embedded-System-
Project for Embedded System Development 

A simple instruction set simulator (ISS) for a new 8-bit embedded processor that is being designed.

- Recognizing the MOV, ADD, LD, ST, CMP, JE, and JMP instructions (described below in more detail);
- Supporting 6 general purpose integer registers (each register has 8 bits), named R1, R2, ... R6;
- Supporting a byte-addressable 256-Byte local memory;
- Counting the total number of executed instructions;
- Computing the total cycle count based on the following assumptions:
o Each move, add, compare, or jump instruction takes 1 clock cycle;
o Each load/store instruction takes 2 cycles when the requested address is already in the local memory
and 45 cycles if the data has to be brought from external memory. 