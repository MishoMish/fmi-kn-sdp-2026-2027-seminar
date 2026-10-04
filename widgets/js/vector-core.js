// Dynamic array growth (week 04): frames for n push_backs under a policy.
// Pure - no DOM - so it can be tested with node --test.

// policy: { kind: "mul", factor } or { kind: "add", step }
export function nextCapacity(cap, policy) {
  if (cap === 0) return 1;
  if (policy.kind === "mul") return Math.max(cap + 1, Math.floor(cap * policy.factor));
  return cap + policy.step;
}

// Writes (element constructions) of n push_backs: n writes plus every copy
// made while reallocating. Also the number of reallocations.
export function totalWork(n, policy) {
  let cap = 0, size = 0, copies = 0, reallocs = 0;
  for (let i = 0; i < n; i++) {
    if (size === cap) {
      copies += size;
      cap = nextCapacity(cap, policy);
      reallocs++;
    }
    size++;
  }
  return { writes: n + copies, copies, reallocs, cap };
}

export function vectorFrames(n, policy) {
  const frames = [];
  let cap = 0, size = 0, copies = 0, reallocs = 0;
  let buf = [];
  const base = () => ({ size, cap, copies, reallocs, writes: size + copies });
  frames.push({ ...base(), buf: [], old: null, hl: [], explain: "Празен вектор: `size = 0`, `capacity = 0`, няма буфер." });
  for (let i = 0; i < n; i++) {
    const value = i + 1;
    if (size === cap) {
      const newCap = nextCapacity(cap, policy);
      const old = buf.slice();
      frames.push({ ...base(), buf: new Array(newCap).fill(null), old, hl: [], newCap,
        explain: size === 0
          ? `push_back(${value}): няма буфер — заделяме място за ${newCap}.`
          : `push_back(${value}): няма място (size = capacity = ${cap}). Заделяме нов буфер с капацитет ${newCap}.` });
      if (size > 0) {
        copies += size;
        reallocs++;
        buf = old.concat(new Array(newCap - size).fill(null));
        frames.push({ ...base(), cap: newCap, buf: buf.slice(), old, hl: [...Array(size).keys()], newCap,
          explain: `Копираме (местим) всичките ${size} елемента в новия буфер и освобождаваме стария. Това струва ${size} записа.` });
      } else {
        reallocs++;
        buf = new Array(newCap).fill(null);
      }
      cap = newCap;
    }
    buf[size] = value;
    size++;
    frames.push({ ...base(), buf: buf.slice(), old: null, hl: [size - 1],
      explain: `Записваме ${value} на позиция ${size - 1}. size = ${size}, capacity = ${cap}.` });
  }
  return frames;
}
