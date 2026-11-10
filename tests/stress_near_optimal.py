"""Stress test of the lower-bound inequalities in the regime where they are tight: optimal clock-face arrangements
(Schoenfield's construction, k = 3, n = 4..300) and near-optimal arrangements found by simulated annealing for
k = 5, 7, 9.  Uses check() from stress_lemmas.py.  Run from the repository root's parent with the repo at
../clockface-ksums, or edit REPO below."""
import random, math, sys
import os; REPO = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src')
sys.path.insert(0, REPO)
from schoenfield import construct
src = open(__file__.replace('stress_near_optimal.py', 'stress_lemmas.py')).read().split("rng = random.Random(2026)")[0]
ns = {"__file__": __file__.replace("stress_near_optimal.py", "stress_lemmas.py")}; exec(src, ns); check = ns["check"]

def anneal(n, k, rng, iters=60000):
    p = list(range(1, n + 1)); rng.shuffle(p)
    s = [sum(p[(i + j) % n] for j in range(k)) for i in range(n)]
    cost = lambda s: max(s) * 1000 + s.count(max(s))
    c = cost(s); T = 2.0
    for _ in range(iters):
        i, j = rng.randrange(n), rng.randrange(n)
        if i == j: continue
        delta = p[j] - p[i]; s2 = s[:]
        for w in range(k):
            s2[(i - w) % n] += delta; s2[(j - w) % n] -= delta
        c2 = cost(s2)
        if c2 <= c or rng.random() < math.exp((c - c2) / (T * 1000)):
            p[i], p[j] = p[j], p[i]; s, c = s2, c2
        T = max(0.05, T * 0.9999)
    return p

rng = random.Random(7); cases = 0; fails = []; tight = 0; excess = []
def run(p, k, tag):
    global cases, tight
    n = len(p); mx = max(sum(p[(i + j) % n] for j in range(k)) for i in range(n))
    excess.append(mx - k * (n + 1) / 2)
    for L in (mx, mx + 1):
        ok, sl = check(p, k, L); cases += 1; tight += (sl == 0)
        fails.extend((tag, n, k, L, name) for name, v in ok.items() if not v)
for n in range(4, 301):
    run(construct(n), 3, "schoenfield")
for t in range(120):
    k = rng.choice([5, 7, 9]); n = rng.randrange(k + 2, 70)
    run(anneal(n, k, rng), k, "anneal")
print(f"{cases} cases; Lemma 6 held with equality in {tight}; excess of the inputs (largest sum minus average): "
      f"min {min(excess)}, median {sorted(excess)[len(excess)//2]}, max {max(excess)}")
print("failures:", fails[:5] if fails else "none")
