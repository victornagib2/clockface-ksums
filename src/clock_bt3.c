// clock_bt3: exhaustive search for circular arrangements p[0..n-1] of 1..n with every sum of three
// consecutive entries s_i = p[i] + p[i+1] + p[i+2] <= L (OEIS A066385).  Symmetry: p[0] = n, p[1] < p[n-1].
//
// Pruning (all necessary conditions, valid for every partial arrangement):
//  (1) budget: the slacks sigma_i = L - s_i are >= 0 and sum to T = n*L - 3n(n+1)/2 exactly.
//  (2) chains: p[i+3] - p[i] = s_{i+1} - s_i = sigma_i - sigma_{i+1}.  The positions split into g = gcd(3,n)
//      cycles i -> i+3; along each cycle the values return to their start, so its total variation is at least
//      2*(max - min) of its values.  Summing over cycles:
//         2 * sum_c range_c  <=  sum_i |sigma_i - sigma_{i+1}|  =  2T - 2 * sum_i min(sigma_i, sigma_{i+1}),
//      so  sum_c range_c + sum_i min(sigma_i, sigma_{i+1}) <= T, and both terms only grow as the search goes on.
// usage: clock_bt3 n L [maxsol] [p1]     (p1: only search arrangements with this value at position 1)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

static int n, L, g, p[128], maxsol, only_p1;
static uint64_t used, nodes;
static long nsol, T;
static int cmin[3], cmax[3], cfirst[3], clast[3]; static long ctv[3];

static inline long tvbound(void) {      // sum over chains of a lower bound on the chain's total variation
    long t = 0;
    for (int q = 0; q < g; q++) {
        if (cmax[q] < cmin[q]) continue;
        long a = 2L * (cmax[q] - cmin[q]);
        if (g == 3) { long b = ctv[q] + labs((long)clast[q] - cfirst[q]); if (b > a) a = b; }
        t += a;
    }
    return t;
}
static void dfs(int k, long budget, long smin, int prev_sigma) {   // positions 0..k-1 filled
    nodes++;
    if (k == n) {
        long s1 = p[n - 2] + p[n - 1] + p[0], s2 = p[n - 1] + p[0] + p[1];
        if (s1 <= L && s2 <= L && p[1] < p[n - 1]) {
            nsol++;
            if (nsol <= maxsol) { printf("solution:"); for (int i = 0; i < n; i++) printf(" %d", p[i]); printf("\n"); fflush(stdout); }
        }
        return;
    }
    int c = g == 3 ? k % 3 : 0;
    if (k == 1) {
        for (int v = 1; v < n; v++) {
            if (only_p1 && v != only_p1) continue;
            int om = cmin[c], oM = cmax[c];
            if (v < cmin[c]) cmin[c] = v; if (v > cmax[c]) cmax[c] = v;
            int of = cfirst[c], ol = clast[c]; long ot = ctv[c];
            if (g == 3) { cfirst[c] = clast[c] = v; ctv[c] = 0; }
            if (tvbound() <= 2 * T) { p[1] = v; used |= 1ULL << v; dfs(2, budget, 0, -1); used &= ~(1ULL << v); }
            cmin[c] = om; cmax[c] = oM; cfirst[c] = of; clast[c] = ol; ctv[c] = ot;
            if (nsol >= maxsol) return;
        }
        return;
    }
    int base = L - p[k - 2] - p[k - 1];
    int lo = base - (int)budget; if (lo < 1) lo = 1;
    int hi = base < n ? base : n;
    if (k == n - 1) { int cc = L - n - p[1]; if (hi > cc) hi = cc; }
    if (k == n - 2) { int cc = L - n - 1; if (hi > cc) hi = cc; }
    for (int v = hi; v >= lo; v--) {
        if (used >> v & 1) continue;
        int sigma = base - v;
        long sm = smin + (prev_sigma >= 0 ? (sigma < prev_sigma ? sigma : prev_sigma) : 0);
        int om = cmin[c], oM = cmax[c];
        if (v < cmin[c]) cmin[c] = v; if (v > cmax[c]) cmax[c] = v;
        int of = cfirst[c], ol = clast[c]; long ot = ctv[c];
        if (g == 3) {
            if (k < 3) { cfirst[c] = clast[c] = v; ctv[c] = 0; }
            else { ctv[c] += labs((long)v - p[k - 3]); clast[c] = v; }
        }
        if (tvbound() + 2 * sm <= 2 * T) {
            p[k] = v; used |= 1ULL << v;
            dfs(k + 1, budget - sigma, sm, sigma);
            used &= ~(1ULL << v);
        }
        cmin[c] = om; cmax[c] = oM; cfirst[c] = of; clast[c] = ol; ctv[c] = ot;
        if (nsol >= maxsol) return;
    }
}

int main(int argc, char **argv) {
    n = atoi(argv[1]); L = atoi(argv[2]); maxsol = argc > 3 ? atoi(argv[3]) : 1; only_p1 = argc > 4 ? atoi(argv[4]) : 0;
    if (n > 62) { fprintf(stderr, "n <= 62\n"); return 1; }
    g = n % 3 == 0 ? 3 : 1;
    T = (long)n * L - 3L * n * (n + 1) / 2;
    if (T < 0) { printf("n=%d L=%d: INFEASIBLE (negative slack), 0 nodes\n", n, L); return 0; }
    for (int q = 0; q < 3; q++) { cmin[q] = 1 << 30; cmax[q] = -1; }
    p[0] = n; used = 1ULL << n; cmin[0] = cmax[0] = n; cfirst[0] = clast[0] = n; ctv[0] = 0;
    dfs(1, T, 0, -1);
    printf("n=%d L=%d: %s, %llu nodes%s\n", n, L, nsol ? "FEASIBLE" : "INFEASIBLE", (unsigned long long)nodes,
           only_p1 ? " (restricted p1)" : "");
    if (nsol) printf("solutions found: %ld%s\n", nsol, nsol >= maxsol ? " (stopped at maxsol)" : "");
    return 0;
}
