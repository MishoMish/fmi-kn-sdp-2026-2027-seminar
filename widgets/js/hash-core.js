// Hash tables (week 08) as frames: chaining or linear probing with
// tombstones; three hash functions over the UTF-8 bytes of the key. Pure.

const enc = new TextEncoder();

export const HASHES = {
  sum: { name: "сума на байтовете", fn: (s) => enc.encode(s).reduce((h, c) => (h + c) >>> 0, 0) },
  poly: { name: "полиномиален, база 31", fn: (s) => enc.encode(s).reduce((h, c) => (Math.imul(h, 31) + c) >>> 0, 0) },
  fnv: { name: "FNV-1a (32 бита)", fn: (s) => enc.encode(s).reduce((h, c) => Math.imul(h ^ c, 16777619) >>> 0, 2166136261) },
};

// ops: [{ op: "insert" | "find" | "erase", key }]
// Returns { frames, opStart } - opStart[k] is the first frame of op k.
export function hashFrames(ops, m, hashName, strategy) {
  const hash = HASHES[hashName].fn;
  const frames = [];
  const opStart = [];
  const chains = Array.from({ length: m }, () => []);
  const slots = Array.from({ length: m }, () => ({ state: "empty", key: null }));
  let size = 0, tombstones = 0, comparisons = 0;

  const snap = (explain, extra = {}) => frames.push({
    strategy, m, size, tombstones, comparisons,
    chains: chains.map((c) => c.slice()),
    slots: slots.map((s) => ({ ...s })),
    hl: null, probe: [], ...extra, explain,
  });

  snap(`Празна таблица с ${m} клетки.`);
  ops.forEach((o, k) => {
    opStart.push(frames.length);
    const h = hash(o.key);
    const home = h % m;
    snap(`${o.op}("${o.key}"): hash = ${h}, ${h} % ${m} = ${home}.`, { hl: home, current: o });
    if (strategy === "chaining") {
      const chain = chains[home];
      let found = -1;
      for (let j = 0; j < chain.length; j++) {
        comparisons++;
        const eq = chain[j] === o.key;
        snap(`Сравняваме с „${chain[j]}“ във веригата на клетка ${home}: ${eq ? "съвпада" : "не съвпада"}.`,
          { hl: home, chainHl: j, current: o });
        if (eq) { found = j; break; }
      }
      if (o.op === "insert") {
        if (found >= 0) snap(`„${o.key}“ вече е в таблицата — нищо не правим.`, { hl: home, current: o });
        else {
          chain.push(o.key);
          size++;
          snap(`Добавяме „${o.key}“ в края на веригата на клетка ${home}${chain.length > 1 ? " — колизия" : ""}.`,
            { hl: home, chainHl: chain.length - 1, current: o, ok: true });
        }
      } else if (o.op === "find") {
        snap(found >= 0 ? `Намерен в клетка ${home}.` : `Веригата свърши: „${o.key}“ го няма.`,
          { hl: home, chainHl: found, current: o, ok: found >= 0, miss: found < 0 });
      } else {
        if (found >= 0) {
          chain.splice(found, 1);
          size--;
        }
        snap(found >= 0 ? `Махаме „${o.key}“ от веригата.` : `„${o.key}“ го няма — нищо за махане.`, { hl: home, current: o, ok: found >= 0 });
      }
      return;
    }
    // linear probing
    let firstTomb = -1;
    let i = home;
    let found = -1;
    const probe = [];
    for (let step = 0; step < m; step++) {
      probe.push(i);
      const s = slots[i];
      if (s.state === "empty") {
        snap(`Клетка ${i} е празна → ключът го няма${o.op === "insert" ? ", тук или в първия камък по пътя" : ""}.`,
          { hl: i, probe: probe.slice(), current: o });
        break;
      }
      if (s.state === "deleted") {
        if (firstTomb < 0) firstTomb = i;
        snap(`Клетка ${i}: надгробен камък — продължаваме (ключът може да е по-нататък).`, { hl: i, probe: probe.slice(), current: o });
      } else {
        comparisons++;
        const eq = s.key === o.key;
        snap(`Клетка ${i}: „${s.key}“ — ${eq ? "съвпада" : "друг ключ, продължаваме"}.`, { hl: i, probe: probe.slice(), current: o });
        if (eq) { found = i; break; }
      }
      i = (i + 1) % m;
    }
    if (o.op === "insert") {
      if (found >= 0) snap(`„${o.key}“ вече е в таблицата.`, { hl: found, probe, current: o });
      else {
        const target = firstTomb >= 0 ? firstTomb : (slots[i].state === "empty" ? i : -1);
        if (target < 0) snap(`Таблицата е пълна — трябва преоразмеряване.`, { probe, current: o, miss: true });
        else {
          if (slots[target].state === "deleted") tombstones--;
          slots[target] = { state: "full", key: o.key };
          size++;
          snap(`Записваме „${o.key}“ в клетка ${target}${target !== home ? ` (${probe.length - 1 > 0 ? "проби: " + probe.length : ""}, групиране!)` : ""}.`,
            { hl: target, probe, current: o, ok: true });
        }
      }
    } else if (o.op === "find") {
      snap(found >= 0 ? `Намерен в клетка ${found} след ${probe.length} проби.` : `Няма го (${probe.length} проби).`,
        { hl: found >= 0 ? found : null, probe, current: o, ok: found >= 0, miss: found < 0 });
    } else {
      if (found >= 0) {
        slots[found] = { state: "deleted", key: o.key };
        size--;
        tombstones++;
      }
      snap(found >= 0 ? `Клетка ${found} → надгробен камък (не „празна“! иначе по-нататъшните ключове се „губят“).`
        : `„${o.key}“ го няма.`, { hl: found >= 0 ? found : null, probe, current: o, ok: found >= 0 });
    }
  });
  return { frames, opStart };
}

export function parseOps(text) {
  // "tree heap graph" or "insert tree, find heap, erase graph"
  const ops = [];
  for (const part of String(text).split(/[,;\n]+/)) {
    const words = part.trim().split(/\s+/).filter(Boolean);
    if (!words.length) continue;
    let op = "insert";
    if (["insert", "find", "erase"].includes(words[0])) op = words.shift();
    for (const w of words) ops.push({ op, key: w });
  }
  return ops;
}
