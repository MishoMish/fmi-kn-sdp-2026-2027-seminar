// Bit operations (week 15) on w-bit unsigned values (w <= 32). Pure.

export const mask = (w) => (w === 32 ? 0xffffffff : (1 << w) - 1) >>> 0;
export const wrap = (x, w) => (x & mask(w)) >>> 0;

export function toSigned(x, w) {
  const u = wrap(x, w);
  return u >= 2 ** (w - 1) ? u - 2 ** w : u;
}

export function parseValue(text, w) {
  const s = String(text).trim().toLowerCase().replace(/'/g, "");
  let v;
  if (/^-?0x[0-9a-f]+$/.test(s)) v = s.startsWith("-") ? -parseInt(s.slice(3), 16) : parseInt(s, 16);
  else if (/^-?0b[01]+$/.test(s)) v = s.startsWith("-") ? -parseInt(s.slice(3), 2) : parseInt(s.slice(2), 2);
  else if (/^-?\d+$/.test(s)) v = parseInt(s, 10);
  else return null;
  return wrap(v, w);
}

export function popcount(x) {
  let c = 0;
  x >>>= 0;
  while (x) { x = (x & (x - 1)) >>> 0; c++; }
  return c;
}

export function operations(x, y, k, w) {
  const m = mask(w);
  const r = (v) => wrap(v, w);
  const shift = Math.max(0, Math.min(w - 1, k));
  const lowBit = r(x & r(~x + 1));
  let next = 1;
  while (next < x && next !== 0) next = r(next * 2);
  return [
    { name: "x & y", value: r(x & y), note: "И: и двата" },
    { name: "x | y", value: r(x | y), note: "ИЛИ: поне единият" },
    { name: "x ^ y", value: r(x ^ y), note: "XOR: точно единият" },
    { name: "~x", value: r(~x & m), note: "обръща всички" },
    { name: `x << ${shift}`, value: r(x << shift), note: `× 2^${shift}, изпадналите се губят` },
    { name: `x >> ${shift}`, value: r(x >>> shift), note: `÷ 2^${shift} (беззнаково)` },
    { name: "x & (x − 1)", value: r(x & r(x - 1)), note: "маха най-младшата единица" },
    { name: "x & −x", value: lowBit, note: "само най-младшата единица" },
    { name: "−x = ~x + 1", value: r(~x + 1), note: "допълнителен код" },
    { name: "следваща степен на 2", value: x === 0 ? 1 : next, note: x === 0 ? "" : next === 0 ? "не се побира" : `≥ ${x}` },
  ];
}
