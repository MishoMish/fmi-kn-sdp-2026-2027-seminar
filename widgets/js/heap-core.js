// Binary heap (week 11) as frames: push, pop, Floyd's makeHeap, heapsort.
// max-heap by default; "min" flips the comparison. Pure.

export function heapFrames(start, ops, kind = "max") {
  const a = start.slice();
  let n = a.length;  // the heap is a[0, n); a[n, len) is the sorted tail in heapsort
  let comparisons = 0, swaps = 0;
  const frames = [];
  const opStart = [];
  const less = (x, y) => (kind === "max" ? x < y : x > y);  // "x should be below y"
  const snap = (explain, extra = {}) => frames.push({ a: a.slice(), n, comparisons, swaps, hl: [], ok: [], kind, explain, ...extra });
  const swap = (i, j) => { [a[i], a[j]] = [a[j], a[i]]; swaps++; };
  const word = kind === "max" ? "по-голямо" : "по-малко";

  function siftUp(i) {
    while (i > 0) {
      const p = (i - 1) >> 1;
      comparisons++;
      if (!less(a[p], a[i])) {
        snap(`Родителят ${a[p]} (индекс ${p}) не е ${kind === "max" ? "по-малък" : "по-голям"} от ${a[i]} → стоп.`, { hl: [p, i] });
        return;
      }
      snap(`${a[i]} е ${word} от родителя ${a[p]} → размяна (siftUp).`, { hl: [p, i] });
      swap(p, i);
      i = p;
      snap(`След размяната ${a[i]} е на индекс ${i}.`, { ok: [i] });
    }
    snap(`${a[0]} стигна корена.`, { ok: [0] });
  }

  function siftDown(i) {
    while (2 * i + 1 < n) {
      let c = 2 * i + 1;
      if (c + 1 < n) {
        comparisons++;
        if (less(a[c], a[c + 1])) c++;
      }
      comparisons++;
      if (!less(a[i], a[c])) {
        snap(`${a[i]} не е ${kind === "max" ? "по-малко" : "по-голямо"} от ${word === "по-голямо" ? "по-голямото" : "по-малкото"} дете ${a[c]} → стоп.`,
          { hl: [i, c] });
        return;
      }
      snap(`${word === "по-голямо" ? "По-голямото" : "По-малкото"} дете на ${a[i]} е ${a[c]} → размяна (siftDown).`, { hl: [i, c] });
      swap(i, c);
      i = c;
    }
    snap(`${a[i]} е лист — готово.`, { ok: [i] });
  }

  snap(n ? `Масив: [${a.join(", ")}].` : "Празна пирамида.");
  for (const o of ops) {
    opStart.push(frames.length);
    if (o.op === "push") {
      a.splice(n, 0, o.value);
      n++;
      snap(`push(${o.value}): добавяме в края (индекс ${n - 1}).`, { hl: [n - 1] });
      siftUp(n - 1);
    } else if (o.op === "pop") {
      if (n === 0) {
        snap("pop(): пирамидата е празна.", { error: true });
        continue;
      }
      const top = a[0];
      snap(`pop(): махаме ${top} — ${kind === "max" ? "най-големия" : "най-малкия"}.`, { hl: [0], removed: top });
      a[0] = a[n - 1];
      a.splice(n - 1, 1);
      n--;
      if (n === 0) {
        snap("Пирамидата остана празна.", { removed: top });
        continue;
      }
      snap(`Последният (${a[0]}) отива в корена.`, { hl: [0], removed: top });
      siftDown(0);
    } else if (o.op === "heapify") {
      snap(`makeHeap (Floyd): siftDown на всеки вътрешен възел, от индекс ${Math.floor(n / 2) - 1} към 0.`);
      for (let i = Math.floor(n / 2) - 1; i >= 0; i--) {
        snap(`siftDown(${i}).`, { hl: [i] });
        siftDown(i);
      }
      snap("Пирамидата е построена.", { ok: [0] });
    } else if (o.op === "sort") {
      snap("Heapsort: първо makeHeap, после n − 1 пъти „върхът отзад“.");
      for (let i = Math.floor(n / 2) - 1; i >= 0; i--) siftDown(i);
      snap("makeHeap готово.", { ok: [0] });
      while (n > 1) {
        snap(`Разменяме върха ${a[0]} с последния от пирамидата (индекс ${n - 1}).`, { hl: [0, n - 1] });
        swap(0, n - 1);
        n--;
        siftDown(0);
      }
      n = 0;
      snap(`Сортирано ${kind === "max" ? "възходящо" : "низходящо"}: [${a.join(", ")}].`, { sorted: true });
    }
  }
  return { frames, opStart, array: a, n };
}

export function isHeap(a, n, kind = "max") {
  for (let i = 1; i < n; i++) {
    const p = (i - 1) >> 1;
    if (kind === "max" ? a[p] < a[i] : a[p] > a[i]) return false;
  }
  return true;
}
