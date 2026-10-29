// plain enumeration: count circular arrangements of 1..n with p[0]=n, p[1]<p[n-1], all k-window sums <= L
#include <stdio.h>
#include <stdlib.h>
static int n, k, L, p[32], used[33]; static long long cnt;
static void rec(int t) {
    if (t == n) {
        if (!(p[1] < p[n-1])) return;
        for (int i = 0; i < n; i++) { int s = 0; for (int j = 0; j < k; j++) s += p[(i + j) % n]; if (s > L) return; }
        cnt++; return;
    }
    for (int v = 1; v < n; v++) if (!used[v]) { used[v] = 1; p[t] = v; rec(t + 1); used[v] = 0; }
}
int main(int c, char **a) { n = atoi(a[1]); k = atoi(a[2]); L = atoi(a[3]); p[0] = n; used[n] = 1; rec(1); printf("%lld\n", cnt); return 0; }
