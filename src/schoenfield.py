"""Jon E. Schoenfield's construction (OEIS A066385 comments, Jul 07 2019): an arrangement of 1..n whose
largest sum of three consecutive entries is at most f(n) = ceil(3n/2) + 3. Used here as the witness for the
upper bound a(n) <= f(n)."""
import math, sys
from fractions import Fraction as F

def ceil(x): return math.ceil(x)

def construct(n):
    p = []
    # the comment's index k runs over 1..n; for n mod 3 != 0 the formula needs k = 0..n-1 to give a permutation
    # (k = 1..n works for n == 1 (mod 3) only), which is just a rotation of the circle
    ks = range(1, n + 1) if n % 3 == 0 else range(0, n)
    for k in ks:
        r = k % 3
        if n % 9 == 0:
            t, u = n // 3, n // 9
            s = (k // 3 - 2 * r * u) % t
            v = ceil(n - (t + F(1, 2)) * r - F(3, 2) * s) if s < 2 * u else 1 + 3 * s - (t - 1) * r
        elif n % 9 in (3, 6):
            t = n // 3
            s = (k // 3 - ((t * (t % 3) - 1) * r) // 3) % t
            v = ceil(n - t * r - F(3, 2) * s) if s < F(2 * t, 3) else 1 + 3 * s - t * r
        else:
            v = k + 1 if (k - (n + 1)) % 3 == 0 else ceil(F(n * ((r % 2) + 1) - k, 2))
        p.append(int(v))
    return p

def f(n): return math.ceil(1.5 * n) + 3

if __name__ == "__main__":
    lo, hi = int(sys.argv[1]), int(sys.argv[2])
    for n in range(lo, hi + 1):
        p = construct(n)
        ok = sorted(p) == list(range(1, n + 1))
        m = max(p[i] + p[(i + 1) % n] + p[(i + 2) % n] for i in range(n))
        print(f"n={n}: permutation={ok}, max triple sum {m}, f(n)={f(n)} {'OK' if ok and m <= f(n) else 'PROBLEM'}")
