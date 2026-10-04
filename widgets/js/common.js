// Shared helpers for the widgets: DOM/SVG creation, a seeded RNG and the
// step-by-step player. Every animated widget computes ALL frames up front
// (pure functions in the other modules, tested with node --test) and the
// player only shows frame i - so stepping back is free.

export const SVG_NS = "http://www.w3.org/2000/svg";

export function el(tag, attrs = {}, parent = null, text = null) {
  const e = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (k === "class") e.className = v;
    else e.setAttribute(k, v);
  }
  if (text !== null) e.textContent = text;
  if (parent) parent.appendChild(e);
  return e;
}

export function svg(tag, attrs = {}, parent = null, text = null) {
  const e = document.createElementNS(SVG_NS, tag);
  for (const [k, v] of Object.entries(attrs)) e.setAttribute(k, String(v));
  if (text !== null) e.textContent = text;
  if (parent) parent.appendChild(e);
  return e;
}

export function svgRoot(parent, width, height, title) {
  parent.textContent = "";
  const root = svg("svg", { viewBox: `0 0 ${width} ${height}`, role: "img", "aria-label": title }, parent);
  const defs = svg("defs", {}, root);
  const m = svg("marker", { id: "arrow", viewBox: "0 0 10 10", refX: 9, refY: 5, markerWidth: 7, markerHeight: 7,
    orient: "auto-start-reverse" }, defs);
  svg("path", { d: "M0,0 L10,5 L0,10 z", class: "arrowhead" }, m);
  return root;
}

// Text of a frame explanation may contain `code` spans.
export function setExplain(node, text) {
  node.textContent = "";
  const parts = String(text).split("`");
  parts.forEach((p, i) => {
    if (i % 2 === 1) el("code", {}, node, p);
    else node.appendChild(document.createTextNode(p));
  });
}

export function setStats(dl, pairs) {
  dl.textContent = "";
  for (const [k, v] of pairs) {
    el("dt", {}, dl, k);
    el("dd", {}, dl, String(v));
  }
}

// Mulberry32: tiny, seeded, good enough for demos.
export function rng(seed) {
  let a = seed >>> 0;
  return function () {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = a;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

export function parseInts(text) {
  return String(text).split(/[\s,;]+/).filter((s) => s.length > 0).map(Number).filter(Number.isFinite)
    .map((x) => Math.trunc(x));
}

// ------------------------------------------------------------------ player

// createPlayer(container, render) builds the controls inside container and
// returns { load(frames, startAt), current(), index() }. render(frame, i, n)
// is called on every change.
export function createPlayer(container, render) {
  container.textContent = "";
  const first = el("button", { type: "button", title: "в началото (Home)" }, container, "⏮");
  const back = el("button", { type: "button", title: "стъпка назад (←)" }, container, "◀");
  const play = el("button", { type: "button", class: "primary", title: "пусни / спри (интервал)" }, container, "▶ пусни");
  const fwd = el("button", { type: "button", title: "стъпка напред (→)" }, container, "▶|");
  const last = el("button", { type: "button", title: "в края (End)" }, container, "⏭");
  const frameLabel = el("span", { class: "frame" }, container, "0 / 0");
  const speedLabel = el("label", {}, container, "скорост");
  const speed = el("input", { type: "range", min: 1, max: 10, value: 5 }, speedLabel);
  el("span", { class: "keys" }, container, "интервал · ← → · Home End");

  let frames = [];
  let i = 0;
  let timer = null;

  function show() {
    frameLabel.textContent = `${frames.length ? i + 1 : 0} / ${frames.length}`;
    back.disabled = first.disabled = i <= 0;
    fwd.disabled = last.disabled = i >= frames.length - 1;
    if (frames.length) render(frames[i], i, frames.length);
  }
  function stop() {
    if (timer) clearTimeout(timer);
    timer = null;
    play.textContent = "▶ пусни";
  }
  function tick() {
    if (i >= frames.length - 1) {
      stop();
      return;
    }
    i += 1;
    show();
    timer = setTimeout(tick, delay());
  }
  function delay() {
    return Math.round(1600 / Math.pow(1.55, Number(speed.value) - 1));
  }
  function toggle() {
    if (timer) {
      stop();
      return;
    }
    if (i >= frames.length - 1) i = 0;
    play.textContent = "⏸ спри";
    show();
    timer = setTimeout(tick, delay());
  }
  function go(j) {
    stop();
    i = Math.max(0, Math.min(frames.length - 1, j));
    show();
  }

  first.addEventListener("click", () => go(0));
  back.addEventListener("click", () => go(i - 1));
  fwd.addEventListener("click", () => go(i + 1));
  last.addEventListener("click", () => go(frames.length - 1));
  play.addEventListener("click", toggle);
  document.addEventListener("keydown", (e) => {
    const t = e.target;
    if (t && (t.tagName === "INPUT" || t.tagName === "SELECT" || t.tagName === "TEXTAREA")) return;
    if (e.key === " ") { e.preventDefault(); toggle(); }
    else if (e.key === "ArrowRight") { e.preventDefault(); go(i + 1); }
    else if (e.key === "ArrowLeft") { e.preventDefault(); go(i - 1); }
    else if (e.key === "Home") { e.preventDefault(); go(0); }
    else if (e.key === "End") { e.preventDefault(); go(frames.length - 1); }
  });

  return {
    load(newFrames, startAt = 0) {
      stop();
      frames = newFrames;
      i = Math.max(0, Math.min(frames.length - 1, startAt));
      show();
    },
    current: () => frames[i],
    index: () => i,
    stop,
  };
}
