// Grid pathfinding (weeks 13-14): BFS, DFS, Dijkstra, A*. Pure.
// grid: array of rows of 0 (free), 1 (wall), 2 (mud: costs 5 to enter).
// Neighbours in the order up, right, down, left.

export const COST = { 0: 1, 2: 5 };
const DIRS = [[-1, 0], [0, 1], [1, 0], [0, -1]];

class MinHeap {
  constructor() { this.a = []; }
  get size() { return this.a.length; }
  push(item) {
    const a = this.a;
    a.push(item);
    let i = a.length - 1;
    while (i > 0) {
      const p = (i - 1) >> 1;
      if (a[p][0] <= a[i][0]) break;
      [a[p], a[i]] = [a[i], a[p]];
      i = p;
    }
  }
  pop() {
    const a = this.a;
    const top = a[0];
    const last = a.pop();
    if (a.length) {
      a[0] = last;
      let i = 0;
      for (;;) {
        let c = 2 * i + 1;
        if (c >= a.length) break;
        if (c + 1 < a.length && a[c + 1][0] < a[c][0]) c++;
        if (a[i][0] <= a[c][0]) break;
        [a[i], a[c]] = [a[c], a[i]];
        i = c;
      }
    }
    return top;
  }
}

// Returns { order: [cell ids in the order they were finished], frontier: [arrays per step],
//   path: [cell ids from start to target] | null, cost, maxFrontier }
export function search(grid, start, target, algorithm) {
  const rows = grid.length, cols = grid[0].length;
  const id = (r, c) => r * cols + c;
  const rc = (k) => [Math.floor(k / cols), k % cols];
  const N = rows * cols;
  const dist = new Array(N).fill(Infinity);
  const parent = new Array(N).fill(-1);
  const done = new Array(N).fill(false);
  const order = [];
  const frontier = [];
  let maxFrontier = 0;
  const s = id(...start), t = id(...target);
  const h = (k) => {
    const [r, c] = rc(k);
    return Math.abs(r - target[0]) + Math.abs(c - target[1]);
  };
  const neighbours = (k) => {
    const [r, c] = rc(k);
    const out = [];
    for (const [dr, dc] of DIRS) {
      const nr = r + dr, nc = c + dc;
      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc] !== 1) out.push(id(nr, nc));
    }
    return out;
  };
  const cellCost = (k) => COST[grid[Math.floor(k / cols)][k % cols]];

  dist[s] = 0;
  if (algorithm === "bfs" || algorithm === "dfs") {
    const box = [s];  // a queue (BFS) or a stack (DFS)
    const seen = new Array(N).fill(false);
    seen[s] = true;
    while (box.length) {
      const u = algorithm === "bfs" ? box.shift() : box.pop();
      if (algorithm === "dfs") {
        if (done[u]) continue;
      }
      done[u] = true;
      order.push(u);
      if (u === t) { frontier.push(box.slice()); break; }
      for (const v of neighbours(u)) {
        if (algorithm === "bfs") {
          if (seen[v]) continue;
          seen[v] = true;
          dist[v] = dist[u] + 1;
          parent[v] = u;
          box.push(v);
        } else if (!done[v]) {
          parent[v] = u;
          dist[v] = dist[u] + 1;
          box.push(v);
        }
      }
      maxFrontier = Math.max(maxFrontier, box.length);
      frontier.push(box.slice());
    }
  } else {
    const heap = new MinHeap();
    heap.push([algorithm === "astar" ? h(s) : 0, s]);
    while (heap.size) {
      const [, u] = heap.pop();
      if (done[u]) continue;  // stale
      done[u] = true;
      order.push(u);
      if (u === t) { frontier.push(heap.a.map((x) => x[1]).filter((k) => !done[k])); break; }
      for (const v of neighbours(u)) {
        const nd = dist[u] + cellCost(v);
        if (nd < dist[v]) {
          dist[v] = nd;
          parent[v] = u;
          heap.push([algorithm === "astar" ? nd + h(v) : nd, v]);
        }
      }
      const open = [...new Set(heap.a.map((x) => x[1]).filter((k) => !done[k]))];
      maxFrontier = Math.max(maxFrontier, open.length);
      frontier.push(open);
    }
  }
  let path = null, cost = null;
  if (done[t]) {
    path = [];
    for (let k = t; k !== -1; k = parent[k]) path.push(k);
    path.reverse();
    cost = 0;
    for (let i = 1; i < path.length; i++) cost += cellCost(path[i]);
  }
  return { order, frontier, path, cost, maxFrontier };
}

// A maze by randomized DFS on the odd cells (rows, cols odd).
export function makeMaze(rows, cols, rand) {
  const g = Array.from({ length: rows }, () => new Array(cols).fill(1));
  const stack = [[1, 1]];
  g[1][1] = 0;
  while (stack.length) {
    const [r, c] = stack[stack.length - 1];
    const options = [[-2, 0], [0, 2], [2, 0], [0, -2]].filter(([dr, dc]) => {
      const nr = r + dr, nc = c + dc;
      return nr > 0 && nr < rows - 1 && nc > 0 && nc < cols - 1 && g[nr][nc] === 1;
    });
    if (!options.length) { stack.pop(); continue; }
    const [dr, dc] = options[Math.floor(rand() * options.length)];
    g[r + dr / 2][c + dc / 2] = 0;
    g[r + dr][c + dc] = 0;
    stack.push([r + dr, c + dc]);
  }
  // a few extra openings so there is more than one route
  for (let k = 0; k < Math.floor((rows * cols) / 40); k++) {
    const r = 1 + Math.floor(rand() * (rows - 2)), c = 1 + Math.floor(rand() * (cols - 2));
    if ((r + c) % 2 === 1) g[r][c] = 0;
  }
  return g;
}
