// Sorting algorithms (weeks 11-12) as frames. Pure.
// Every frame: { a, cmp: [i, j] | null, hot: [indices written], comparisons, writes, done, note }

export const ALGORITHMS = {
  selection: "selection sort",
  bubble: "bubble sort",
  insertion: "insertion sort",
  shell: "Shell sort (Ciura)",
  heap: "heapsort",
  merge: "merge sort",
  quickFirst: "quicksort, pivot = първият",
  quickRandom: "quicksort, случаен pivot",
  radix: "radix sort (LSD, база 10)",
};

export function makeInput(kind, n, rand) {
  const a = Array.from({ length: n }, (_, i) => i + 1);
  if (kind === "sorted") return a;
  if (kind === "reversed") return a.reverse();
  if (kind === "few") return Array.from({ length: n }, () => 1 + Math.floor(rand() * 4) * Math.ceil(n / 4));
  for (let i = n - 1; i > 0; i--) {  // shuffle
    const j = Math.floor(rand() * (i + 1));
    [a[i], a[j]] = [a[j], a[i]];
  }
  if (kind === "nearly" && n > 1) {
    a.sort((x, y) => x - y);
    for (let k = 0; k < Math.max(1, Math.floor(n / 10)); k++) {
      const i = Math.floor(rand() * (n - 1));
      [a[i], a[i + 1]] = [a[i + 1], a[i]];
    }
  }
  return a;
}

export function sortFrames(input, algorithm, rand = Math.random) {
  const a = input.slice();
  const frames = [];
  let comparisons = 0, writes = 0;
  const push = (extra = {}) => frames.push({ a: a.slice(), cmp: null, hot: [], comparisons, writes, done: false, ...extra });
  const less = (i, j) => {
    comparisons++;
    push({ cmp: [i, j] });
    return a[i] < a[j];
  };
  const lessVal = (i, v, j = -1) => {  // compare a[i] with a value held aside
    comparisons++;
    push({ cmp: [i, j] });
    return a[i] < v;
  };
  const swap = (i, j) => {
    if (i === j) return;
    [a[i], a[j]] = [a[j], a[i]];
    writes += 2;
    push({ hot: [i, j] });
  };
  const write = (i, v) => {
    a[i] = v;
    writes++;
    push({ hot: [i] });
  };
  const n = a.length;
  push({ note: "вход" });

  if (algorithm === "selection") {
    for (let i = 0; i < n - 1; i++) {
      let m = i;
      for (let j = i + 1; j < n; j++) if (less(j, m)) m = j;
      swap(i, m);
    }
  } else if (algorithm === "bubble") {
    for (let end = n; end > 1; end--) {
      let swapped = false;
      for (let j = 1; j < end; j++) if (less(j, j - 1)) { swap(j, j - 1); swapped = true; }
      if (!swapped) break;
    }
  } else if (algorithm === "insertion" || algorithm === "shell") {
    const gaps = [];
    if (algorithm === "shell") {
      const ciura = [1, 4, 10, 23, 57, 132, 301, 701];
      for (const g of ciura) if (g < n) gaps.unshift(g);
    } else gaps.push(1);
    for (const gap of gaps) {
      for (let i = gap; i < n; i++) {
        const v = a[i];
        let j = i;
        while (j >= gap && (comparisons++, push({ cmp: [j - gap, i === j ? i : -1] }), v < a[j - gap])) {
          write(j, a[j - gap]);
          j -= gap;
        }
        if (j !== i) write(j, v);
      }
    }
  } else if (algorithm === "heap") {
    const down = (i, size) => {
      for (;;) {
        let c = 2 * i + 1;
        if (c >= size) return;
        if (c + 1 < size && less(c, c + 1)) c++;
        if (!less(i, c)) return;
        swap(i, c);
        i = c;
      }
    };
    for (let i = Math.floor(n / 2) - 1; i >= 0; i--) down(i, n);
    for (let size = n; size > 1; size--) {
      swap(0, size - 1);
      down(0, size - 1);
    }
  } else if (algorithm === "merge") {
    const buf = new Array(n);
    const rec = (lo, hi) => {
      if (hi - lo < 2) return;
      const mid = (lo + hi) >> 1;
      rec(lo, mid);
      rec(mid, hi);
      let i = lo, j = mid, k = 0;
      while (i < mid && j < hi) buf[k++] = less(j, i) ? a[j++] : a[i++];
      while (i < mid) buf[k++] = a[i++];
      while (j < hi) buf[k++] = a[j++];
      for (let t = 0; t < k; t++) write(lo + t, buf[t]);
    };
    rec(0, n);
  } else if (algorithm === "quickFirst" || algorithm === "quickRandom") {
    const part = (lo, hi) => {  // Lomuto on [lo, hi], pivot moved to hi
      const p = algorithm === "quickRandom" ? lo + Math.floor(rand() * (hi - lo + 1)) : lo;
      swap(p, hi);
      let store = lo;
      for (let i = lo; i < hi; i++) if (less(i, hi)) { swap(i, store); store++; }
      swap(store, hi);
      return store;
    };
    const stack = [[0, n - 1]];
    while (stack.length) {
      const [lo, hi] = stack.pop();
      if (lo >= hi) continue;
      const p = part(lo, hi);
      stack.push([lo, p - 1], [p + 1, hi]);
    }
  } else if (algorithm === "radix") {
    const max = Math.max(...a, 0);
    for (let exp = 1; Math.floor(max / exp) > 0; exp *= 10) {
      const buckets = Array.from({ length: 10 }, () => []);
      for (const x of a) buckets[Math.floor(x / exp) % 10].push(x);
      let k = 0;
      for (const b of buckets) for (const x of b) write(k++, x);
    }
  } else {
    throw new Error(`unknown algorithm ${algorithm}`);
  }
  push({ done: true });
  return frames;
}
