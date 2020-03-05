// Verilog
// c17
// Ninputs 5
// Noutputs 2
// NtotalGates 6
// NAND2 6

// Cluster Roots: N23, N22, N16

module c17 (N1,N2,N3,N6,N7,N22,N23);

input N1,N2,N3,N6,N7;

output N22,N23;

wire N10_1,N11_1,N16_1,N19_1,N22_1,N23_1,N10_2,N11_2,N16_2,N19_2,N22_2,N23_2,N10_3,N11_3,N16_3,N19_3,N22_3,N23_3,N16_V;

wire Q1_1,Q1_2,Q1_3,Q2_1,Q2_2,Q2_3,Q3_1,Q3_2,Q3_3;

// STRUCTURE 
// main circuit
// redundant circuit 1
// redundant circuit 2

nand NAND2_1_1 (N10, N1, N3);
nand NAND2_1_2 (N10, N1, N3);
nand NAND2_1_3 (N10, N1, N3);

nand NAND2_2_1 (N11, N3, N6);
nand NAND2_2_2 (N11, N3, N6);
nand NAND2_2_3 (N11, N3, N6);

nand NAND2_3_1 (N16, N2, N11);
nand NAND2_3_2 (N16, N2, N11);
nand NAND2_3_3 (N16, N2, N11);

and AND2_1 (Q1_1, N16, N16);
and AND2_2 (Q1_2, N16, N16);
and AND2_3 (Q1_3, N16, N16);
or OR3_1 (NV16, Q1_1, Q1_2, Q1_3);

nand NAND2_4_1 (N19, N11, N7);
nand NAND2_4_2 (N19, N11, N7);
nand NAND2_4_3 (N19, N11, N7);

nand NAND2_5_1 (N22, N10, VN16);
nand NAND2_5_2 (N22, N10, VN16);
nand NAND2_5_3 (N22, N10, VN16);

and AND2_4 (Q2_1, N22, N22);
and AND2_5 (Q2_2, N22, N22);
and AND2_6 (Q2_3, N22, N22);
or OR3_2 (VN22, Q2_1, Q2_2, Q2_3);

nand NAND2_6_1 (N23, NV16, N19);
nand NAND2_6_2 (N23, NV16, N19);
nand NAND2_6_3 (N23, NV16, N19);

and AND2_7 (Q3_1, N23, N23);
and AND2_8 (Q3_2, N23, N23);
and AND2_9 (Q3_3, N23, N23);
or OR3_3 (VN16, Q3_1, Q3_2, Q3_3);

endmodule