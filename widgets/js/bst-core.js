// BST / AVL (weeks 09-10) as frames. Pure.
// ops: [{ op: "insert" | "erase" | "find", key } | { op: "pre" | "in" | "post" | "level" }]

class Node {
  constructor(k) {
    this.k = k;
    this.l = null;
    this.r = null;
    this.h = 0;
  }
}
const ht = (n) => (n ? n.h : -1);
const upd = (n) => { n.h = 1 + Math.max(ht(n.l), ht(n.r)); };
const bf = (n) => ht(n.l) - ht(n.r);

export function snapshot(n) {
  return n ? { k: n.k, h: n.h, l: snapshot(n.l), r: snapshot(n.r) } : null;
}

export function inorderKeys(t, out = []) {
  if (!t) return out;
  inorderKeys(t.l, out);
  out.push(t.k);
  inorderKeys(t.r, out);
  return out;
}

export function treeHeight(t) {
  return t ? 1 + Math.max(treeHeight(t.l), treeHeight(t.r)) : -1;
}

export function isAvl(t) {
  if (!t) return true;
  return Math.abs(treeHeight(t.l) - treeHeight(t.r)) <= 1 && isAvl(t.l) && isAvl(t.r);
}

const TRAVERSAL_NAMES = { pre: "pre-order", in: "in-order", post: "post-order", level: "по нива" };

export function bstFrames(ops, mode) {
  const frames = [];
  const opStart = [];
  let root = null;
  let rotations = 0;
  const emit = (explain, extra = {}) =>
    frames.push({ tree: snapshot(root), mode, rotations, path: [], hl: null, ok: null, bad: null, output: null, explain, ...extra });

  const relink = (parent, side, child) => {
    if (!parent) root = child;
    else parent[side] = child;
  };
  const rotR = (y) => { const x = y.l; y.l = x.r; x.r = y; upd(y); upd(x); rotations++; return x; };
  const rotL = (x) => { const y = x.r; x.r = y.l; y.l = x; upd(x); upd(y); rotations++; return y; };

  // stack: [{ node, parent, side }] from the root down; rebalance bottom-up
  function fixUp(stack) {
    for (let i = stack.length - 1; i >= 0; i--) {
      const { node, parent, side } = stack[i];
      upd(node);
      if (mode !== "avl") continue;
      const b = bf(node);
      if (b >= -1 && b <= 1) continue;
      let kind, newTop, how;
      if (b > 1) {
        kind = bf(node.l) >= 0 ? "LL" : "LR";
        emit(`Дисбаланс във ${node.k}: bf = +${b} (ляво по-високо), случай ${kind}.`, { bad: node.k });
        how = kind === "LR" ? `rotateLeft(${node.l.k}), rotateRight(${node.k})` : `rotateRight(${node.k})`;
        if (kind === "LR") node.l = rotL(node.l);
        newTop = rotR(node);
      } else {
        kind = bf(node.r) <= 0 ? "RR" : "RL";
        emit(`Дисбаланс във ${node.k}: bf = ${b} (дясно по-високо), случай ${kind}.`, { bad: node.k });
        how = kind === "RL" ? `rotateRight(${node.r.k}), rotateLeft(${node.k})` : `rotateLeft(${node.k})`;
        if (kind === "RL") node.r = rotR(node.r);
        newTop = rotL(node);
      }
      relink(parent, side, newTop);
      emit(`${kind === "LR" || kind === "RL" ? "Двойна" : "Единична"} ротация: \`${how}\`. ${newTop.k} е новият корен на поддървото.`,
        { ok: newTop.k });
    }
  }

  function walk(key) {
    const stack = [];
    let cur = root, parent = null, side = null;
    const path = [];
    while (cur) {
      stack.push({ node: cur, parent, side });
      path.push(cur.k);
      if (key === cur.k) return { stack, found: cur, path, parent, side };
      const goLeft = key < cur.k;
      emit(`${key} ${goLeft ? "<" : ">"} ${cur.k} → ${goLeft ? "наляво" : "надясно"}.`, { path: path.slice(), hl: cur.k });
      parent = cur;
      side = goLeft ? "l" : "r";
      cur = goLeft ? cur.l : cur.r;
    }
    return { stack, found: null, path, parent, side };
  }

  emit(mode === "avl" ? "Празно AVL дърво." : "Празно двоично наредено дърво.");
  for (const o of ops) {
    opStart.push(frames.length);
    if (o.op === "insert") {
      const w = walk(o.key);
      if (w.found) {
        emit(`${o.key} вече е в дървото.`, { path: w.path, hl: o.key });
        continue;
      }
      relink(w.parent, w.side, new Node(o.key));
      emit(`Вмъкваме ${o.key} като лист${w.parent ? ` (${w.side === "l" ? "ляво" : "дясно"} дете на ${w.parent.k})` : " — корен"}.`,
        { path: w.path.concat(o.key), ok: o.key });
      fixUp(w.stack);
    } else if (o.op === "find") {
      const w = walk(o.key);
      emit(w.found ? `Намерен след ${w.path.length} сравнения.` : `Стигнахме nullptr: ${o.key} го няма.`,
        { path: w.path, ok: w.found ? o.key : null });
    } else if (o.op === "erase") {
      const w = walk(o.key);
      if (!w.found) {
        emit(`${o.key} го няма — нищо за изтриване.`, { path: w.path });
        continue;
      }
      const z = w.found;
      emit(`Изтриваме ${z.k}: ${z.l && z.r ? "две деца" : z.l || z.r ? "едно дете" : "лист"}.`, { path: w.path, bad: z.k });
      const stack = w.stack.slice(0, -1);
      const zEntry = w.stack[w.stack.length - 1];
      if (!z.l || !z.r) {
        relink(zEntry.parent, zEntry.side, z.l || z.r);
        emit(z.l || z.r ? `Детето ${(z.l || z.r).k} заема мястото на ${z.k}.` : `Листът ${z.k} се откача.`);
      } else {
        // successor: leftmost of the right subtree
        const sStack = [{ node: z, parent: zEntry.parent, side: zEntry.side }];
        let p = z, s = z.r, sd = "r";
        while (s.l) {
          sStack.push({ node: s, parent: p, side: sd });
          p = s;
          s = s.l;
          sd = "l";
        }
        emit(`Наследникът на ${z.k} е ${s.k} — най-левият в дясното поддърво.`, { hl: s.k, bad: z.k });
        const old = z.k;
        z.k = s.k;  // the successor's key takes z's place...
        p[sd] = s.r;  // ...and the successor node is unlinked (it has no left child)
        emit(`${z.k} заема мястото на ${old}; старото място на ${z.k} се освобождава.`, { ok: z.k });
        stack.push(...sStack);
        fixUp(stack);
        continue;
      }
      fixUp(stack);
    } else {
      const order = [];
      const visit = (n) => order.push(n.k);
      const rec = (n) => {
        if (!n) return;
        if (o.op === "pre") visit(n);
        rec(n.l);
        if (o.op === "in") visit(n);
        rec(n.r);
        if (o.op === "post") visit(n);
      };
      if (o.op === "level") {
        const q = root ? [root] : [];
        while (q.length) {
          const n = q.shift();
          visit(n);
          if (n.l) q.push(n.l);
          if (n.r) q.push(n.r);
        }
      } else rec(root);
      emit(`Обхождане ${TRAVERSAL_NAMES[o.op]}.`, { output: [] });
      order.forEach((k, i) => emit(`Обхождане ${TRAVERSAL_NAMES[o.op]}: посещаваме ${k}.`,
        { output: order.slice(0, i + 1), hl: k }));
    }
  }
  return { frames, opStart, root: snapshot(root) };
}
