"""Stress test of every inequality in the lower-bound proof (Lemmas 5, 6, 8, Proposition 7 and the final inequality)
on large n: random permutations, near-optimal permutations found by local search (the regime where the
inequalities are nearly tight), and the explicit Theorem 2 arrangements.  Independent of tests/lemmas.c."""
import random, math, sys
import os; sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src'))
from construction import array, circle

def check(p, k, L):
    n = len(p); g = math.gcd(n, k)
    s = [sum(p[(i + j) % n] for j in range(k)) for i in range(n)]
    assert L >= max(s)
    sig = [L - x for x in s]
    T = sum(sig); assert T == n * L - k * n * (n + 1) // 2
    d = [sig[i] - sig[(i + 1) % n] for i in range(n)]
    assert all(d[i] == p[(i + k) % n] - p[i] for i in range(n))
    M = sum(min(sig[i], sig[(i + 1) % n]) for i in range(n))
    P = sum(1 for i in range(n) if sig[i] > 0 and sig[(i + 1) % n] > 0)
    tv = sum(abs(x) for x in d)
    D = sum(1 for i in range(n) if d[i] * d[(i + k) % n] < 0)
    # peaks per chain
    seen = [False] * n; peaks = 0
    for st in range(n):
        if seen[st]: continue
        ch = []; i = st
        while not seen[i]: seen[i] = True; ch.append(p[i]); i = (i + k) % n
        l = len(ch); peaks += sum(1 for j in range(l) if ch[j] > ch[j - 1] and ch[j] > ch[(j + 1) % l])
    ok = {
        "5(i) d_i != 0": all(x != 0 for x in d),
        "5(ii) sum|d| = 2T-2M": tv == 2 * T - 2 * M,
        "5(iii) M >= P": M >= P,
        "D = 2 * peaks >= 2g": D == 2 * peaks and D >= 2 * g,
        "6 sum|d| >= 2n-4g+D": tv >= 2 * n - 4 * g + D,
        "7 2M+D <= 2(T-n)+4g": 2 * M + D <= 2 * (T - n) + 4 * g,
    }
    if k % 2:
        ok["8 n-D <= (k+1)P"] = n - D <= (k + 1) * P
        ok["final n <= 2(T-n)+4g+(k-1)P"] = n <= 2 * (T - n) + 4 * g + (k - 1) * P
    slack6 = tv - (2 * n - 4 * g + D)
    return ok, slack6

def local_search(n, k, rng, iters):
    p = list(range(1, n + 1)); rng.shuffle(p)
    def cost(q):
        s = sum(q[:k]); mx = s; cnt = 0
        for i in range(n):
            s += q[(i + k) % n] - q[i]
            if s > mx: mx, cnt = s, 1
            elif s == mx: cnt += 1
        return mx * n + cnt
    c = cost(p)
    for _ in range(iters):
        i, j = rng.randrange(n), rng.randrange(n)
        p[i], p[j] = p[j], p[i]
        c2 = cost(p)
        if c2 <= c: c = c2
        else: p[i], p[j] = p[j], p[i]
    return p

rng = random.Random(2026)
fails = {}; cases = 0; tight6 = 0
def run(p, k, tag):
    global cases, tight6
    n = len(p); mx = max(sum(p[(i + j) % n] for j in range(k)) for i in range(n))
    for L in (mx, mx + 1, mx + 2):
        ok, sl = check(p, k, L); cases += 1
        if sl == 0: tight6 += 1
        for name, v in ok.items():
            if not v: fails.setdefault(name, []).append((tag, n, k, L))
for t in range(300):                                  # random permutations
    k = rng.choice([1, 2, 3, 4, 5, 7, 9, 11, 13, 21]); n = rng.randrange(k + 1, 400)
    p = list(range(1, n + 1)); rng.shuffle(p); run(p, k, "random")
for t in range(150):                                  # near-optimal permutations
    k = rng.choice([3, 5, 7, 9, 11]); n = rng.randrange(k + 2, 120)
    run(local_search(n, k, rng, 4000), k, "local search")
for k in range(3, 22, 2):                             # Theorem 2 arrangements (msum = 1)
    for m in range(3, k + 3, 2):
        run(circle(array(k, m, 1), k, m), k, "theorem 2")
print(f"{cases} (permutation, L) cases checked; Lemma 6 held with equality in {tight6} of them")
print("failures:", {k: len(v) for k, v in fails.items()} if fails else "none")
