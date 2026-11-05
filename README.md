# clockface-ksums

Small maximal sums of k consecutive numbers on a circle, and a complete solution of the clock-face problem
(OEIS [A066385](https://oeis.org/A066385)).

Arrange 1, 2, …, n on a circle. `msum(n, k)` is the smallest possible value of the largest sum of k
neighbouring numbers, minus the average k(n+1)/2 of these sums (notation of Higashitani and Kurimoto).

## Results

The note [paper/clockface.pdf](paper/clockface.pdf) proves:

| Result | Statement |
| --- | --- |
| Theorem 1 | For odd k ≥ 3, n > k and g = gcd(n, k): msum(n, k) ≥ 2 if n is odd and n > (k+3)g; msum(n, k) ≥ 3/2 if n is even and n ≠ 2k. |
| Theorem 2 | For odd k and odd 3 ≤ m ≤ k+2 there is an explicit arrangement of 1..mk with every k-sum at most k(mk+1)/2 + 1. |
| Corollary 3 | For odd k ≥ 3 and odd m ≥ 3: msum(mk, k) = 1 if m ≤ k+2, and 2 if m ≥ k+4. Previously known only for m = 3, for m ≥ 2k+3, for n = 15 and 21 when k = 3, and for n = 35 when k = 7 (by computer: Bogdanović, Maksimović, Simić and Milošević, 2017). |
| Corollary 4 | Clock-face numbers: a(n) = ⌈3n/2⌉ + 3 for all n ≥ 4 except a(5) = 10, a(6) = 11, a(9) = 16, a(15) = 25. |

Corollary 4 confirms the conjecture of J. E. Schoenfield in A066385. Its lower bound is proved without any
computer; the matching upper bound is due to Anstee, Ferguson and Griggs and to Schoenfield's construction. For odd
n the result also follows from Corollary 1.6 of Higashitani and Kurimoto (2019), whose case n = 21 rests on a
personal communication.

For example, Corollary 3 gives msum(n, 5) = 1 for n = 15, 25, 35 and msum(n, 5) = 2 for n = 45, 55, 65, …;
the values for n = 25 to 55 were open.

[results/b066385.txt](results/b066385.txt) lists a(n) for n = 3..10000 in OEIS b-file format.

## Computer checks

None of the proofs in the note needs a computer. As a check:

- `tests/lemmas.c` verifies every inequality of the lower-bound proof on all circular arrangements for
  n ≤ 11 (k = 3 and k = 5), about 2.4 × 10^7 cases.
- `src/construction.py` builds the arrangements of Theorem 2 and checks them for all odd k ≤ 61 and odd
  m ≤ k + 2.
- [results/lemma_checks.log](results/lemma_checks.log) and
  [results/construction_check_k_up_to_61.log](results/construction_check_k_up_to_61.log) are the outputs of these two
  checks, and [results/theorem2_arrangements.txt](results/theorem2_arrangements.txt) lists the arrangements of Theorem 2
  for all odd k ≤ 11, so they can be checked by hand.
- `src/clock_bt3.c` is an exhaustive branch-and-bound search for k = 3, and `src/kwin.c` the same for any k.
  Both are checked against plain enumeration (`tests/brute.c`). They confirm a(n) for every n from 3 to 44:
  [results/verify_known_3_31.log](results/verify_known_3_31.log) and
  [results/clock_run_32_plus.log](results/clock_run_32_plus.log).

## Reproducing

    gcc -O3 -march=native -o src/clock_bt3 src/clock_bt3.c -lm
    gcc -O3 -march=native -o src/kwin src/kwin.c
    gcc -O2 -o tests/lemmas tests/lemmas.c
    for n in 4 5 6 7 8 9 10 11; do tests/lemmas $n 3; done      # lemma checks, k = 3
    python3 src/construction.py 61                               # Theorem 2 arrangements
    src/verify_clock.sh 44                                       # a(3)..a(44) by exhaustive search
    src/kwin 35 5 91 1 0 1                                       # an arrangement of 1..35 with every 5-sum <= 91

`kwin n k L [maxsol] [p1] [block]` prints arrangements of 1..n whose k-sums are all at most L, or reports that
none exists (`block = 1` restricts the search to the block structure of Theorem 2, which is faster but can only
find arrangements, not rule them out).

## Version history

- **1.1 (6 November 2026).** Added the reference to Bogdanović, Maksimović, Simić and Milošević (2017), whose
  computation disc(35, 7) = 1 already gave msum(35, 7) = 1, so that case of Corollary 3 was known before; the note's
  introduction and abstract now say so. Added the logs of the lemma and construction checks. The theorems and proofs
  are unchanged.
- **1.0 (30 October 2026).** First version.

## How to cite

> Victor Nidal Bassem Nagib, *Small maximal k-consecutive sums on a circle and the clock-face problem*, 2026,
> https://github.com/victornagib2/clockface-ksums

## References

- A. Higashitani and K. Kurimoto, Permutations with small maximal k-consecutive sums, Discrete Math. 342 (2019)
  2467–2481, [arXiv:1801.00416](https://arxiv.org/abs/1801.00416).
- R. P. Anstee, R. Ferguson and J. R. Griggs, Permutations with low discrepancy consecutive k-sums,
  J. Combin. Theory Ser. A 100 (2002) 302–321.
- M. Bogdanović, Z. Maksimović, A. Simić and J. Milošević, A mixed integer linear programming formulation for low
  discrepancy consecutive k-sums permutation problem, Yugosl. J. Oper. Res. 27 (2017) 125–132,
  [doi:10.2298/YJOR160104005B](https://doi.org/10.2298/YJOR160104005B).
- OEIS [A066385](https://oeis.org/A066385), with the comments and the construction of J. E. Schoenfield (2019).

The code is MIT licensed. The note, the code and the computations were produced with the help of an AI assistant
(Claude, by Anthropic), working under the direction of the author, who checked the results.
