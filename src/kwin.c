// kwin: exhaustive search for circular arrangements p[0..n-1] of 1..n in which every sum of k consecutive
// entries is <= L (k = 3 is OEIS A066385).  Symmetry: p[0] = n, p[1] < p[n-1].
// Slack sigma_j = L - s_j >= 0 with sum T = n*L - k*n(n+1)/2.  d_j = sigma_j - sigma_{j+1} = p_{j+k} - p_j.
// Chains j -> j+k (g = gcd(n,k) of them, m = n/g values each) have total variation sum_j |d_j| = 2T - 2M,
// M = sum_j min(sigma_j, sigma_{j+1}).  Pruning: sum over chains of a lower bound on each chain's final total
// variation, plus 2M of the windows completed so far, must stay <= 2T.  Per chain the bound is
//   2*max(m-1, observed range) + 2*max(0, observed interior peaks - 1)      (always)
//   observed path variation + |last - first|                                 (when k | n: chain order = fill order)
// usage: kwin n k L [maxsol] [p1] [block]   (p1 = 0: no restriction; block = 1: chain-block restricted search, needs k | n)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct { int mn, mx, first, last, cnt, dir, peaks; long tv; } Chain;
static int n, k, L, g, m, ordered, p[130], pre[131], only_p1, block;
static long maxsol;
static uint8_t used[131];
static long long nodes, nsol;
static long T, lbtot;
static Chain ch[130];

static int gcdi(int a, int b) { while (b) { int t = a % b; a = b; b = t; } return a; }
static inline long lb_chain(const Chain *c) {
    if (c->cnt == 0) return 2L * (m - 1);
    long r = c->mx - c->mn; if (r < m - 1) r = m - 1;
    long b = 2 * r + (c->peaks > 1 ? 2L * (c->peaks - 1) : 0);
    if (ordered) { long a = c->tv + labs((long)c->last - c->first); if (a > b) b = a; }
    return b;
}
static inline void chain_add(Chain *c, int v) {
    if (c->cnt == 0) { c->mn = c->mx = c->first = c->last = v; c->cnt = 1; c->tv = 0; c->dir = 0; c->peaks = 0; return; }
    int diff = v - c->last;
    if (ordered) { if (c->dir > 0 && diff < 0) c->peaks++; c->tv += labs(diff); c->dir = diff > 0 ? 1 : -1; }
    c->last = v; if (v < c->mn) c->mn = v; if (v > c->mx) c->mx = v; c->cnt++;
}

static void dfs(int t, long budget, long M, long prev_sigma) {   // positions 0..t-1 filled
    nodes++;
    if (t == n) {
        for (int j = n - k + 1; j < n; j++) if ((pre[n] - pre[j]) + pre[j + k - n] > L) return;
        if (!block && !(p[1] < p[n - 1])) return;
        nsol++;
        if (nsol <= maxsol) { printf("solution:"); for (int i = 0; i < n; i++) printf(" %d", p[i]); printf("\n"); fflush(stdout); }
        return;
    }
    long base = -1; int lo = 1, hi = n;
    if (t >= k - 1) {
        base = L - (pre[t] - pre[t - k + 1]);
        if (base < hi) hi = (int)base;
        if (base - budget > lo) lo = (int)(base - budget);
    }
    if (t == n - 1 && hi > L) hi = L;
    int c = t % g;
    for (int v = hi; v >= lo; v--) {
        if (used[v]) continue;
        if (block && (v - 1) / m != (k - 1 - c) % k) continue;
        if (t == 1 && only_p1 && v != only_p1) continue;
        if (!block && t == n - 1 && v <= p[1]) continue;
        long sigma = t >= k - 1 ? base - v : -1;
        long nM = M + (sigma >= 0 && prev_sigma >= 0 ? (sigma < prev_sigma ? sigma : prev_sigma) : 0);
        // wrap-around windows j = n-k+1..n-1 that already contain position t: filled part + smallest possible rest <= L
        if (t >= n - k + 1) {
            int ok = 1, u = n - 1 - t;
            for (int j = n - k + 1; j <= t && ok; j++) {
                long s = (pre[t] + v - pre[j]) + pre[j + k - n] + (long)u * (u + 1) / 2;
                if (s > L) ok = 0;
            }
            if (!ok) continue;
        }
        Chain save = ch[c]; long oldlb = lb_chain(&ch[c]);
        chain_add(&ch[c], v);
        long nlb = lbtot - oldlb + lb_chain(&ch[c]);
        if (nlb + 2 * nM <= 2 * T) {
            long olbt = lbtot; lbtot = nlb;
            p[t] = v; used[v] = 1; pre[t + 1] = pre[t] + v;
            dfs(t + 1, sigma >= 0 ? budget - sigma : budget, nM, sigma);
            used[v] = 0; lbtot = olbt;
        }
        ch[c] = save;
        if (nsol >= maxsol) return;
    }
}

int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "usage: kwin n k L [maxsol] [p1]\n"); return 1; }
    n = atoi(argv[1]); k = atoi(argv[2]); L = atoi(argv[3]);
    maxsol = argc > 4 ? atol(argv[4]) : 1; only_p1 = argc > 5 ? atoi(argv[5]) : 0; block = argc > 6 ? atoi(argv[6]) : 0;
    if (n > 128 || k < 2 || k >= n) { fprintf(stderr, "need 2 <= k < n <= 128\n"); return 1; }
    g = gcdi(n, k); m = n / g; ordered = (g == k);
    if (block && g != k) { fprintf(stderr, "block mode needs k | n\n"); return 1; }
    T = (long)n * L - (long)k * n * (n + 1) / 2;
    if (T < 0) { printf("n=%d k=%d L=%d: INFEASIBLE (negative slack), 0 nodes\n", n, k, L); return 0; }
    lbtot = 2L * (m - 1) * g;
    p[0] = n; used[n] = 1; pre[0] = 0; pre[1] = n;
    { Chain *c0 = &ch[0]; long o = lb_chain(c0); chain_add(c0, n); lbtot += lb_chain(c0) - o; }
    if (lbtot <= 2 * T) dfs(1, T, 0, -1);
    printf("n=%d k=%d L=%d: %s, %lld nodes%s%s\n", n, k, L, nsol ? "FEASIBLE" : "INFEASIBLE", nodes, only_p1 ? " (restricted p1)" : "", block ? " (block-restricted)" : "");
    if (nsol) printf("solutions found: %lld%s\n", nsol, nsol >= maxsol ? " (stopped at maxsol)" : "");
    return 0;
}
