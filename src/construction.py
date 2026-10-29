"""Explicit arrangement of 1..mk (k, m odd, 3 <= m <= k+2) whose k-window sums are all <= k(mk+1)/2 + 1.
Base (k0 = m-2 columns): column c is C = (m, m-1, ..., h+1, 1, 2, ..., h), h = (m-1)/2, started at index h*c.
Step (k -> k+2): insert, in front of some column U, the two columns U and (m+1) - U.
Circle: position q*k + c holds b(c)*m + a[q][c], where the blocks b(c) are any permutation of 0..k-1."""
import sys

def base(m):
    h = (m - 1) // 2
    C = list(range(m, h, -1)) + list(range(1, h + 1))
    return [[C[(q + h * c) % m] for c in range(m - 2)] for q in range(m)]

def extend(A, m, at=1):
    """insert columns U, m+1-U before column `at` (U = that column)"""
    out = []
    for row in A:
        U = row[at % len(row)]
        out.append(row[:at] + [U, m + 1 - U] + row[at:])
    return out

def array(k, m, at=1):
    A = base(m)
    while len(A[0]) < k: A = extend(A, m, at if at < len(A[0]) else 0)
    return A

def circle(A, k, m, blocks=None):
    blocks = blocks or [(k - 1 - c) % k for c in range(k)]
    return [blocks[c] * m + A[q][c] for q in range(m) for c in range(k)]

def max_excess(p, k):
    n = len(p); assert sorted(p) == list(range(1, n + 1)), "not a permutation"
    s = sum(p[:k]); mx = s
    for i in range(n): s += p[(i + k) % n] - p[i]; mx = max(mx, s)
    return mx - k * (n + 1) // 2          # k, n odd: integer

if __name__ == "__main__":
    K = int(sys.argv[1]) if len(sys.argv) > 1 else 41
    import random
    bad = 0; cnt = 0
    for k in range(3, K + 1, 2):
        for m in range(3, k + 3, 2):
            for at in {0, 1, (k // 2), random.randrange(max(1, m - 2))}:
                A = array(k, m, at)
                assert len(A) == m and all(len(r) == k for r in A)
                for col in range(k): assert sorted(A[q][col] for q in range(m)) == list(range(1, m + 1))
                blocks = list(range(k)); random.shuffle(blocks)
                e = max_excess(circle(A, k, m, blocks), k); cnt += 1
                if e != 1: bad += 1; print("FAIL", k, m, at, e)
    print(f"{cnt} constructions checked (all odd 3 <= k <= {K}, odd 3 <= m <= k+2, several insertion points, random block orders); failures: {bad}")
