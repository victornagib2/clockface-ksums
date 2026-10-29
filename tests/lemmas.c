// Exhaustive check of every step of the lower-bound argument, for ALL circular arrangements of 1..n
// (p[0] = n fixed, all (n-1)! orders) and window length k (odd), with L = max window sum + e, e = 0, 1, 2.
// s_i = p_i + ... + p_{i+k-1}, sigma_i = L - s_i >= 0, T = sum sigma_i, d_i = sigma_i - sigma_{i+1} = p_{i+k} - p_i,
// M = sum min(sigma_i, sigma_{i+1}), P = #{i : sigma_i > 0 and sigma_{i+1} > 0},
// chains i -> i+k (g = gcd(n,k) of them, m = n/g values each), r_c = number of local maxima on chain c,
// S = #{i : sign d_i = sign d_{i+k}}, D = n - S.
// Steps:  (1) d_i != 0   (2) sum|d_i| = 2T - 2M   (3) S <= (k+1) P   (4) TV_c >= 2(m-1) + 2(r_c-1) and D = 2 sum r_c
//         (5) 2M + D <= 2(T-n) + 4g          (6) n <= 2(T-n) + 4g + (k-1) P
// Also reports min over arrangements of the max window sum (= a(n) for k = 3).
#include <stdio.h>
#include <stdlib.h>
static int n, k, g, m, p[32], used[33];
static long long cnt, fail[7], best = 1LL << 60;
static int gcdi(int a, int b) { while (b) { int t = a % b; a = b; b = t; } return a; }
static void check(void) {
    int s[32], mx = 0;
    for (int i = 0; i < n; i++) { int t = 0; for (int j = 0; j < k; j++) t += p[(i + j) % n]; s[i] = t; if (t > mx) mx = t; }
    if (mx < best) best = mx;
    for (int e = 0; e <= 2; e++) {
        int L = mx + e; long sg[32], T = 0, M = 0, P = 0, S = 0, sumabs = 0; long d[32];
        for (int i = 0; i < n; i++) { sg[i] = L - s[i]; T += sg[i]; }
        for (int i = 0; i < n; i++) {
            long a = sg[i], b = sg[(i + 1) % n]; d[i] = a - b;
            if (d[i] != p[(i + k) % n] - p[i]) { fprintf(stderr, "identity broken\n"); exit(1); }
            if (d[i] == 0) fail[1]++;
            sumabs += labs(d[i]); M += a < b ? a : b; if (a > 0 && b > 0) P++;
        }
        if (sumabs != 2 * T - 2 * M) fail[2]++;
        for (int i = 0; i < n; i++) if ((d[i] > 0) == (d[(i + k) % n] > 0)) S++;
        long D = n - S;
        if (S > (long)(k + 1) * P) fail[3]++;
        long rsum = 0; int ok4 = 1;
        for (int c = 0; c < g; c++) {
            int vals[32], idx = c; long tv = 0; int r = 0, lo = 1 << 30, hi = -1;
            for (int j = 0; j < m; j++) { vals[j] = p[idx]; tv += labs(d[idx]); idx = (idx + k) % n; }
            if (idx != c) { fprintf(stderr, "chain does not close\n"); exit(1); }
            for (int j = 0; j < m; j++) {
                int a = vals[(j + m - 1) % m], b = vals[j], cc = vals[(j + 1) % m];
                if (b > a && b > cc) r++;
                if (b < lo) lo = b; if (b > hi) hi = b;
            }
            if (m == 1) r = 0;
            if (!(tv >= 2L * (m - 1) + 2L * (r - 1) && hi - lo >= m - 1)) ok4 = 0;
            rsum += r;
        }
        if (D != 2 * rsum) ok4 = 0;
        if (!ok4) fail[4]++;
        if (2 * M + D > 2 * (T - n) + 4L * g) fail[5]++;
        if (n > 2 * (T - n) + 4L * g + (long)(k - 1) * P) fail[6]++;
        cnt++;
    }
}
static void rec(int pos) {
    if (pos == n) { check(); return; }
    for (int v = 1; v < n; v++) if (!used[v]) { used[v] = 1; p[pos] = v; rec(pos + 1); used[v] = 0; }
}
int main(int argc, char **argv) {
    n = atoi(argv[1]); k = atoi(argv[2]);
    g = gcdi(n, k); m = n / g;
    p[0] = n; used[n] = 1; rec(1);
    printf("k=%d n=%d: %lld (arrangement, L) cases; failures: (1) %lld (2) %lld (3) %lld (4) %lld (5) %lld (6) %lld; min max window sum = %lld\n",
           k, n, cnt, fail[1], fail[2], fail[3], fail[4], fail[5], fail[6], best);
    return 0;
}
