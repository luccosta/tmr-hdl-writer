// Verilog
// c17
// Ninputs 5
// Noutputs 2
// NtotalGates 6
// NAND2 6

// Cluster Roots: N23, N22, N16

module c17 (N1,N2,N3,N6,N7,NV22,NV23);

input N1,N2,N3,N6,N7;

output NV22,NV23;

wire N10,N11,N16,N19,1N10,1N11,1N16,1N19,2N10,2N11,2N16,2N19,N22,1N22,2N22,N23,1N23,2N23,NV16;

// main circuit
// redundant circuit 1
// redundant circuit 2

nand NAND2_1 (N10, N1, N3);
nand NAND2_1_1 (1N10, N1, N3);
nand NAND2_2_1 (2N10, N1, N3);

nand NAND2_2 (N11, N3, N6);
nand NAND2_1_2 (1N11, N3, N6);
nand NAND2_2_2 (2N11, N3, N6);

nand NAND2_3 (N16, N2, N11);
nand NAND2_1_3 (1N16, N2, 1N11);
nand NAND2_2_3 (2N16, N2, 2N11);

voter VOTER_1 (NV16, N16, 1N16, 2N16);

nand NAND2_4 (N19, N11, N7);
nand NAND2_1_4 (1N19, 1N11, N7);
nand NAND2_2_4 (2N19, 2N11, N7);

nand NAND2_5 (N22, N10, N16);
nand NAND2_1_5 (1N22, 1N10, NV16);
nand NAND2_2_5 (2N22, 2N10, NV16);

voter VOTER_2 (NV22, N22, 1N22, 2N22);

nand NAND2_6 (N23, N16, N19);
nand NAND2_1_6 (1N23, NV16, 1N19);
nand NAND2_2_6 (2N23, NV16, 2N19);

voter VOTER_3 (NV22, N22, 1N22, 2N22);

endmodule

// voter module
module voter (V,A,B,C);

input A,B,C;

output V;

wire Q1,Q2,Q3;

and AND2_1 (Q1, A, B);
and AND2_2 (Q2, B, C);
and AND2_3 (Q3, C, A);
or OR3_1 (V, Q1, Q2, Q3);

endmodule