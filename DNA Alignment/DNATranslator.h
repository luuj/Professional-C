#pragma once
#include <vector>
#include <string>

class DNATranslator
{
public:
    DNATranslator(const std::string& sequence);
    const char FindNextState(char currNucleotide, std::vector<char> &currTransition);
    const std::vector<int>& GetAminoCount();

private:
    std::vector<std::vector<char>> mStateMachine;
    std::vector<int> mAminoCount;
};

/********************
Amino acid codes
0 = Alanine (A)
1 = Arginine (R)
2 = Asparagine (N)
3 = Aspartic Acid (D)
4 = Cysteine (C)
5 = Glutamine (Q)
6 = Glutamic Acid (E)
7 = Glycine (G)
8 = Histidine (H)
9 = Isoleucine (I)
10 = Methionine (M)
11 = Leucine (L)
12 = Lysine (K)
13 = Phenylalanine (F)
14 = Proline (P)
15 = Serine (S)
16 = Threonine (T)
17 = Tryptophan (W)
18 = Tyrosine (Y)
19 = Valine (V) 
*********************/

enum AminoAcidCode
{
    A=24, R, N, D, C, Q, E, G, H, I, M, L, K, F, P, S, T, W, Y, V
};
