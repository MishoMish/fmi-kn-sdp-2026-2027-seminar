// Loads weeks/<id>/slides.md into reveal.js.
//
// slides.md is written to read well on GitHub too, so paths inside it are
// relative to the week folder. Here we rewrite them: images point to the
// week folder on this site, links to .md / source files point to GitHub.
(async function () {
  const params = new URLSearchParams(location.search);
  const weekId = params.get("w");
  const slidesEl = document.getElementById("slides");

  if (!weekId || !/^[0-9]{2}-[a-z0-9-]+$/.test(weekId)) {
    location.replace("../");
    return;
  }

  function fail(message) {
    slidesEl.innerHTML =
      '<section><h2>Слайдовете не се заредиха</h2><p>' + message + "</p>" +
      '<p class="small">Отвори ги през GitHub Pages или локален сървър ' +
      "(<code>python3 -m http.server</code> в корена на repo-то), не директно като файл.</p></section>";
    Reveal.initialize({ hash: true });
  }

  let meta, md;
  try {
    const [metaRes, mdRes] = await Promise.all([
      fetch("weeks.json"),
      fetch("../weeks/" + weekId + "/slides.md"),
    ]);
    if (!mdRes.ok) throw new Error("slides.md: HTTP " + mdRes.status);
    meta = await metaRes.json();
    md = await mdRes.text();
  } catch (err) {
    fail(String(err.message || err));
    return;
  }

  const seminar = meta.seminars.find((s) => s.weeks.includes(weekId));
  const sourceRoot = meta.repo + "/blob/" + meta.branch;
  const treeRoot = meta.repo + "/tree/" + meta.branch;
  const githubExt = /\.(md|cpp|h|hpp|txt|json|cmake|py|csv)$/i;

  function fixUrl(url) {
    if (/^(https?:|mailto:|#|data:|\/)/i.test(url)) return url;
    const [pathPart, hash = ""] = url.split("#");
    const path = new URL(pathPart, "https://x/weeks/" + weekId + "/").pathname;
    const suffix = hash ? "#" + hash : "";
    if (path.endsWith("/")) return treeRoot + path + suffix;
    if (githubExt.test(path)) return sourceRoot + path + suffix;
    return ".." + path + suffix;
  }

  // marked (inside reveal's markdown plugin) would eat backslashes and
  // underscores inside $...$, so escape them before it sees the text.
  function protectMath(tex) {
    return tex
      .replace(/\\([!-\/:-@\[-`{-~])/g, "\\\\$1")
      .replace(/_/g, "\\_")
      .replace(/\*/g, "\\*")
      .replace(/</g, "\\lt ")
      .replace(/>/g, "\\gt ");
  }

  function transformProse(text) {
    return text
      .split(/(`[^`\n]*`)/)
      .map((part, i) => {
        if (i % 2 === 1) return part; // inline code
        return part
          .replace(/\$\$([\s\S]+?)\$\$/g, (m, tex) => "$$" + protectMath(tex) + "$$")
          .replace(/(^|[^$\\])\$([^$\n]+?)\$/g, (m, pre, tex) => pre + "$" + protectMath(tex) + "$")
          .replace(/(\]\()([^)\s]+)(\))/g, (m, a, url, c) => a + fixUrl(url) + c)
          .replace(/(\s(?:src|href)=")([^"]+)(")/g, (m, a, url, c) => a + fixUrl(url) + c);
      })
      .join("");
  }

  const transformed = md
    .split(/(^```[\s\S]*?^```)/m)
    .map((part, i) => (i % 2 === 1 ? part : transformProse(part)))
    .join("");

  const section = document.createElement("section");
  section.setAttribute("data-markdown", "");
  section.setAttribute("data-separator", "^\\r?\\n---\\r?\\n$");
  section.setAttribute("data-separator-vertical", "^\\r?\\n--\\r?\\n$");
  const template = document.createElement("textarea");
  template.setAttribute("data-template", "");
  template.textContent = transformed;
  section.appendChild(template);
  slidesEl.appendChild(section);

  const footer = document.getElementById("footer");
  if (seminar) {
    document.title = "Семинар " + seminar.n + " · " + seminar.title;
    footer.textContent = meta.course + " · Семинар " + seminar.n + " · " + seminar.title;
  }

  Reveal.initialize({
    hash: true,
    width: 1280,
    height: 720,
    margin: 0.05,
    slideNumber: "c/t",
    transition: "fade",
    transitionSpeed: "fast",
    center: false,
    pdfSeparateFragments: false,
    plugins: [RevealMarkdown, RevealHighlight, RevealMath.KaTeX, RevealNotes, RevealZoom],
  });

  Reveal.on("ready", updateFooter);
  Reveal.on("slidechanged", updateFooter);
  function updateFooter() {
    const first = Reveal.getIndices().h === 0;
    footer.hidden = first || !seminar;
  }
})();
