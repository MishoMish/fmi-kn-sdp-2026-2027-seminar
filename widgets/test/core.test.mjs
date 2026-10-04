// node --test widgets/test   (no dependencies)
import test from "node:test";
import assert from "node:assert/strict";
import { vectorFrames, totalWork, nextCapacity } from "../js/vector-core.js";
import { shuntingFrames } from "../js/shunting-core.js";

test("vector: x2 growth does fewer than 3n writes", () => {
  for (const n of [1, 2, 3, 17, 1000, 100000]) assert.ok(totalWork(n, { kind: "mul", factor: 2 }).writes < 3 * n);
  assert.equal(nextCapacity(0, { kind: "mul", factor: 1.5 }), 1);
  assert.equal(nextCapacity(1, { kind: "mul", factor: 1.5 }), 2);  // never stuck at 1
  const quad = totalWork(1000, { kind: "add", step: 1 });
  assert.equal(quad.copies, 999 * 1000 / 2);
});

test("vector: frames end with all elements in place", () => {
  const f = vectorFrames(9, { kind: "mul", factor: 2 });
  const last = f[f.length - 1];
  assert.equal(last.size, 9);
  assert.equal(last.cap, 16);
  assert.deepEqual(last.buf.slice(0, 9), [1, 2, 3, 4, 5, 6, 7, 8, 9]);
  assert.equal(last.copies, 1 + 2 + 4 + 8);
});

test("shunting-yard: RPN and values", () => {
  const cases = [["3 + 4 * (2 - 1)", "3 4 2 1 - * +", 7], ["8 - 3 - 2", "8 3 - 2 -", 3], ["2 ^ 3 ^ 2", "2 3 2 ^ ^", 512],
    ["(1 + 2) * (3 + 4) / 7", "1 2 + 3 4 + * 7 /", 3], ["1 + 2 * 3 - 4", "1 2 3 * + 4 -", 3], ["7 / 2", "7 2 /", 3],
    ["12", "12", 12]];
  for (const [expr, rpn, value] of cases) {
    const r = shuntingFrames(expr);
    assert.equal(r.rpn.join(" "), rpn, expr);
    assert.equal(r.value, value, expr);
  }
});

test("shunting-yard: errors", () => {
  for (const bad of ["(1 + 2", "1 + 2)", "1 +", "7 / (3 - 3)", "2 ^ (0 - 1)", "a + 1", ""]) {
    const r = shuntingFrames(bad);
    assert.equal(r.value, null, bad);
    assert.ok(r.frames[r.frames.length - 1].error, bad);
  }
});

import { hashFrames, parseOps, HASHES } from "../js/hash-core.js";

test("hash: FNV-1a 32-bit published vectors", () => {
  assert.equal(HASHES.fnv.fn(""), 0x811c9dc5);
  assert.equal(HASHES.fnv.fn("a"), 0xe40c292c);
  assert.equal(HASHES.fnv.fn("foobar"), 0xbf9cf968);
});

test("hash: anagrams collide under the byte sum", () => {
  const h = HASHES.sum.fn;
  assert.equal(h("listen"), h("silent"));
  assert.notEqual(HASHES.poly.fn("listen"), HASHES.poly.fn("silent"));
});

test("hash: parseOps", () => {
  assert.deepEqual(parseOps("a b, erase a, find b c"), [
    { op: "insert", key: "a" }, { op: "insert", key: "b" }, { op: "erase", key: "a" },
    { op: "find", key: "b" }, { op: "find", key: "c" }]);
});

test("hash: chaining and probing agree with a Set", () => {
  const words = "tree heap graph list queue stack array map set deque vector bits".split(" ");
  for (const strategy of ["chaining", "probing"]) {
    for (const hash of ["sum", "poly", "fnv"]) {
      const ops = [];
      const ref = new Set();
      const results = [];
      for (let i = 0; i < 60; i++) {
        const key = words[(i * 7 + 3) % words.length];
        const op = ["insert", "insert", "find", "erase"][(i * 5) % 4];
        ops.push({ op, key });
        if (op === "insert") ref.add(key);
        if (op === "erase") ref.delete(key);
        if (op === "find") results.push(ref.has(key));
      }
      const { frames } = hashFrames(ops, 31, hash, strategy);
      const last = frames[frames.length - 1];
      assert.equal(last.size, ref.size, `${strategy}/${hash}`);
      const finds = frames.filter((f) => f.current && f.current.op === "find" && (f.ok || f.miss));
      assert.deepEqual(finds.map((f) => !!f.ok), results, `${strategy}/${hash}`);
    }
  }
});

test("hash: a tombstone keeps later keys reachable", () => {
  // with the byte sum, "ab" and "ba" collide; erase the first, find the second
  const { frames } = hashFrames(parseOps("ab ba, erase ab, find ba"), 7, "sum", "probing");
  assert.ok(frames[frames.length - 1].ok);
});

import { bstFrames, inorderKeys, treeHeight, isAvl } from "../js/bst-core.js";

test("bst/avl: random operations keep the keys sorted and AVL balanced", () => {
  let seed = 7;
  const rand = () => (seed = (seed * 1103515245 + 12345) % 2147483648) / 2147483648;
  for (const mode of ["bst", "avl"]) {
    const ops = [];
    const ref = new Set();
    for (let i = 0; i < 400; i++) {
      const key = Math.floor(rand() * 60);
      const op = rand() < 0.65 ? "insert" : "erase";
      ops.push({ op, key });
      if (op === "insert") ref.add(key); else ref.delete(key);
    }
    const { root } = bstFrames(ops, mode);
    assert.deepEqual(inorderKeys(root), [...ref].sort((a, b) => a - b), mode);
    if (mode === "avl") assert.ok(isAvl(root));
  }
});

test("bst/avl: sorted input - a chain vs a perfect tree", () => {
  const ops = Array.from({ length: 15 }, (_, i) => ({ op: "insert", key: i + 1 }));
  assert.equal(treeHeight(bstFrames(ops, "bst").root), 14);
  const avl = bstFrames(ops, "avl").root;
  assert.equal(treeHeight(avl), 3);
  assert.equal(avl.k, 8);
});

test("bst/avl: the four rotation cases", () => {
  for (const keys of [[3, 2, 1], [1, 2, 3], [3, 1, 2], [1, 3, 2]]) {
    const r = bstFrames(keys.map((key) => ({ op: "insert", key })), "avl").root;
    assert.equal(r.k, 2, keys.join(","));
  }
});

test("bst: traversals", () => {
  const ops = [4, 2, 6, 1, 3, 7].map((key) => ({ op: "insert", key }));
  for (const [op, expected] of [["pre", [4, 2, 1, 3, 6, 7]], ["in", [1, 2, 3, 4, 6, 7]], ["post", [1, 3, 2, 7, 6, 4]],
    ["level", [4, 2, 6, 1, 3, 7]]]) {
    const { frames } = bstFrames([...ops, { op }], "bst");
    assert.deepEqual(frames[frames.length - 1].output, expected, op);
  }
});

import { heapFrames, isHeap } from "../js/heap-core.js";

test("heap: push and pop give sorted output", () => {
  for (const kind of ["max", "min"]) {
    const values = [5, 1, 9, 3, 7, 9, 2, 8];
    const ops = values.map((value) => ({ op: "push", value })).concat(values.map(() => ({ op: "pop" })));
    const { frames } = heapFrames([], ops, kind);
    const popped = frames.filter((f) => f.removed !== undefined && /^pop/.test(f.explain)).map((f) => f.removed);
    const expected = values.slice().sort((a, b) => (kind === "max" ? b - a : a - b));
    assert.deepEqual(popped, expected, kind);
  }
});

test("heap: heapify and heapsort", () => {
  const start = [3, 1, 6, 5, 2, 4, 8, 7, 9, 10, 11, 12, 13, 14, 15];
  const h = heapFrames(start, [{ op: "heapify" }]);
  assert.ok(isHeap(h.array, h.n));
  assert.ok(h.frames[h.frames.length - 1].comparisons <= 2 * start.length);  // Floyd: at most 2n
  const s = heapFrames(start, [{ op: "sort" }]);
  assert.deepEqual(s.array, start.slice().sort((a, b) => a - b));
});

import { sortFrames, makeInput, ALGORITHMS } from "../js/sort-core.js";

test("sort: every algorithm sorts every kind of input", () => {
  let seed = 3;
  const rand = () => (seed = (seed * 1103515245 + 12345) % 2147483648) / 2147483648;
  for (const algo of Object.keys(ALGORITHMS)) {
    for (const kind of ["random", "sorted", "reversed", "nearly", "few"]) {
      for (const n of [1, 2, 7, 40]) {
        const input = makeInput(kind, n, rand);
        const frames = sortFrames(input, algo, rand);
        const last = frames[frames.length - 1];
        assert.ok(last.done);
        assert.deepEqual(last.a, input.slice().sort((x, y) => x - y), `${algo} ${kind} ${n}`);
      }
    }
  }
});

test("sort: comparison counts match the theory", () => {
  const sorted = makeInput("sorted", 40, Math.random);
  const cmp = (algo, input) => sortFrames(input, algo).at(-1).comparisons;
  assert.equal(cmp("insertion", sorted), 39);
  assert.equal(cmp("bubble", sorted), 39);
  assert.equal(cmp("selection", sorted), 40 * 39 / 2);
  assert.equal(cmp("quickFirst", sorted), 40 * 39 / 2);  // the worst case
  assert.equal(cmp("radix", sorted), 0);
});

import { search, makeMaze } from "../js/path-core.js";

test("path: BFS, Dijkstra and A* agree on unit costs; DFS finds some path", () => {
  let seed = 11;
  const rand = () => (seed = (seed * 1103515245 + 12345) % 2147483648) / 2147483648;
  for (let trial = 0; trial < 10; trial++) {
    const g = makeMaze(17, 31, rand);
    const s = [1, 1], t = [15, 29];
    const bfs = search(g, s, t, "bfs"), dij = search(g, s, t, "dijkstra"), ast = search(g, s, t, "astar"), dfs = search(g, s, t, "dfs");
    assert.ok(bfs.path);
    assert.equal(dij.cost, bfs.cost);
    assert.equal(ast.cost, bfs.cost);
    assert.ok(ast.order.length <= dij.order.length);  // A* never finishes more cells
    assert.ok(dfs.cost >= bfs.cost);
  }
});

test("path: mud makes Dijkstra go around; BFS ignores the cost", () => {
  // a 3x5 corridor: the straight middle row is mud, the top row is free
  const g = [[0, 0, 0, 0, 0], [0, 2, 2, 2, 0], [1, 1, 1, 1, 1]];
  const dij = search(g, [1, 0], [1, 4], "dijkstra");
  assert.equal(dij.cost, 6);  // up, 4 right, down (each 1)
  const bfs = search(g, [1, 0], [1, 4], "bfs");
  assert.equal(bfs.path.length - 1, 4);  // fewest steps
  assert.equal(bfs.cost, 5 * 3 + 1);     // ...but through the mud
});

test("path: unreachable target", () => {
  const g = [[0, 1, 0], [0, 1, 0]];
  for (const a of ["bfs", "dfs", "dijkstra", "astar"]) assert.equal(search(g, [0, 0], [0, 2], a).path, null);
});

import { operations, parseValue, toSigned, popcount as pc, wrap } from "../js/bits-core.js";

test("bits: parsing, signed view, tricks", () => {
  assert.equal(parseValue("0x58", 8), 88);
  assert.equal(parseValue("0b1011000", 8), 88);
  assert.equal(parseValue("-1", 8), 255);
  assert.equal(parseValue("hello", 8), null);
  assert.equal(toSigned(255, 8), -1);
  assert.equal(toSigned(128, 8), -128);
  assert.equal(toSigned(0xffffffff, 32), -1);
  assert.equal(pc(0xffffffff), 32);
  const ops = Object.fromEntries(operations(88, 0b10101100, 2, 8).map((o) => [o.name, o.value]));
  assert.equal(ops["x & (x − 1)"], 80);
  assert.equal(ops["x & −x"], 8);
  assert.equal(ops["~x"], 167);
  assert.equal(ops["x << 2"], wrap(88 << 2, 8));
  assert.equal(ops["следваща степен на 2"], 128);
  const big = Object.fromEntries(operations(0x80000001, 0, 31, 32).map((o) => [o.name, o.value]));
  assert.equal(big["следваща степен на 2"], 0);  // does not fit in 32 bits
  assert.equal(big["x << 31"], 0x80000000);
});
