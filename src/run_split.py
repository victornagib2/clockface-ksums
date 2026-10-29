"""Run the exhaustive k = 3 search (clock_bt3) for (n, L), split by the value at position 1, on several cores; resumable.
usage: python3 run_split.py n L [workers]   -> log in split_n_L.jsonl, verdict printed at the end"""
import json, os, subprocess, sys, time
from multiprocessing import Pool
HERE = os.path.dirname(os.path.abspath(__file__))

def one(args):
    n, L, p1 = args
    t0 = time.time()
    out = subprocess.run([os.path.join(HERE, "clock_bt3"), str(n), str(L), "1", str(p1)], capture_output=True, text=True).stdout
    sol = [l for l in out.splitlines() if l.startswith("solution:")]
    nodes = int(out.split(" nodes")[0].split(",")[-1])
    return {"n": n, "L": L, "p1": p1, "feasible": bool(sol), "solution": sol[0][10:] if sol else None,
            "nodes": nodes, "seconds": round(time.time() - t0, 2)}

if __name__ == "__main__":
    n, L = int(sys.argv[1]), int(sys.argv[2]); w = int(sys.argv[3]) if len(sys.argv) > 3 else 2
    log = os.path.join(os.getcwd(), f"split_{n}_{L}.jsonl")
    done = {}
    if os.path.exists(log):
        for line in open(log): r = json.loads(line); done[r["p1"]] = r
    todo = [(n, L, v) for v in range(1, n) if v not in done]
    t0 = time.time()
    with Pool(w) as pool, open(log, "a") as fh:
        for r in pool.imap_unordered(one, todo):
            fh.write(json.dumps(r) + "\n"); fh.flush(); done[r["p1"]] = r
            if r["feasible"]: print(f"n={n} L={L}: ARRANGEMENT FOUND with p1={r['p1']}: {r['solution']}", flush=True)
    feasible = [r for r in done.values() if r["feasible"]]
    nodes = sum(r["nodes"] for r in done.values())
    print(f"n={n} L={L}: {'FEASIBLE' if feasible else 'INFEASIBLE'} over all {len(done)} values of p1, "
          f"{nodes} nodes, {time.time() - t0:.0f} s this run", flush=True)
