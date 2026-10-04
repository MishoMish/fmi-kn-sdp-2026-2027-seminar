// Shunting-yard and RPN evaluation (week 08), as frames. Pure.
// Integers, + - * / ^ and brackets. "/" truncates like C++; "^" is
// right-associative. No unary minus (the week's tasks have none either).

const PREC = { "+": 1, "-": 1, "*": 2, "/": 2, "^": 3 };
const RIGHT = { "^": true };

export function tokenize(text) {
  const out = [];
  let i = 0;
  while (i < text.length) {
    const c = text[i];
    if (c === " " || c === "\t") { i++; continue; }
    if (c >= "0" && c <= "9") {
      let j = i;
      while (j < text.length && text[j] >= "0" && text[j] <= "9") j++;
      out.push({ kind: "num", value: Number(text.slice(i, j)), text: text.slice(i, j) });
      i = j;
    } else if (c in PREC) {
      out.push({ kind: "op", text: c });
      i++;
    } else if (c === "(" || c === ")") {
      out.push({ kind: c, text: c });
      i++;
    } else if (c === "−") {
      out.push({ kind: "op", text: "-" });
      i++;
    } else {
      throw new Error(`непознат символ „${c}“ на позиция ${i + 1}`);
    }
  }
  return out;
}

function apply(op, a, b) {
  switch (op) {
    case "+": return a + b;
    case "-": return a - b;
    case "*": return a * b;
    case "/":
      if (b === 0) throw new Error("деление на нула");
      return Math.trunc(a / b);
    case "^":
      if (b < 0) throw new Error("отрицателен степенен показател");
      return a ** b;
    default: throw new Error(op);
  }
}

// Frames of the conversion, then of the evaluation. Each frame:
// { phase, tokens, pos, output, stack, values, explain, error? }
export function shuntingFrames(text) {
  let tokens;
  try {
    tokens = tokenize(text);
  } catch (e) {
    return { frames: [{ phase: "error", tokens: [], pos: -1, output: [], stack: [], values: [], explain: e.message, error: true }],
      rpn: null, value: null };
  }
  const frames = [];
  const output = [];
  const stack = [];
  const snap = (pos, explain, extra = {}) =>
    frames.push({ phase: "convert", tokens, pos, output: output.slice(), stack: stack.slice(), values: [], explain, ...extra });
  snap(-1, "Вход: инфиксен запис. Числата отиват направо на изхода; операторите чакат в стека.");
  const fail = (pos, msg) => {
    snap(pos, msg, { error: true });
    return { frames, rpn: null, value: null };
  };
  for (let p = 0; p < tokens.length; p++) {
    const t = tokens[p];
    if (t.kind === "num") {
      output.push(t.text);
      snap(p, `Число ${t.text} → направо на изхода.`);
    } else if (t.kind === "op") {
      const popped = [];
      while (stack.length) {
        const top = stack[stack.length - 1];
        if (top === "(") break;
        const stronger = PREC[top] > PREC[t.text];
        const equalLeft = PREC[top] === PREC[t.text] && !RIGHT[t.text];
        if (!stronger && !equalLeft) break;
        output.push(stack.pop());
        popped.push(top);
      }
      stack.push(t.text);
      const why = popped.length
        ? `Изкарваме ${popped.map((x) => "`" + x + "`").join(", ")} (по-силни${popped.some((x) => PREC[x] === PREC[t.text]) ? " или равни и `" + t.text + "` е ляво-асоциативен" : ""}), после слагаме \`${t.text}\`.`
        : `Върхът на стека не е по-силен${RIGHT[t.text] ? " (`^` е дясно-асоциативен: равен приоритет не изкарва)" : ""} → слагаме \`${t.text}\` в стека.`;
      snap(p, `Оператор \`${t.text}\`: ${why}`);
    } else if (t.kind === "(") {
      stack.push("(");
      snap(p, "`(` → в стека: стена, през която операторите не минават.");
    } else {
      const popped = [];
      while (stack.length && stack[stack.length - 1] !== "(") {
        const x = stack.pop();
        output.push(x);
        popped.push(x);
      }
      if (!stack.length) return fail(p, "Грешка: `)` без съответна `(`.");
      stack.pop();
      snap(p, `\`)\` → изкарваме${popped.length ? " " + popped.map((x) => "`" + x + "`").join(", ") : " нищо"} до \`(\` и махаме скобата.`);
    }
  }
  const rest = [];
  while (stack.length) {
    const x = stack.pop();
    if (x === "(") return fail(tokens.length, "Грешка: `(` без съответна `)`.");
    output.push(x);
    rest.push(x);
  }
  snap(tokens.length, rest.length ? `Край на входа: изкарваме ${rest.map((x) => "`" + x + "`").join(", ")}. Постфиксният запис е готов.`
    : "Край на входа. Постфиксният запис е готов.");
  const rpn = output.slice();

  // evaluation
  const values = [];
  const evalSnap = (pos, explain, extra = {}) =>
    frames.push({ phase: "eval", tokens, pos: tokens.length, output: rpn, rpnPos: pos, stack: [], values: values.slice(), explain, ...extra });
  evalSnap(-1, "Пресмятане на постфиксния запис: стек от числа.");
  for (let p = 0; p < rpn.length; p++) {
    const x = rpn[p];
    if (x in PREC) {
      if (values.length < 2) {
        evalSnap(p, `Грешка: \`${x}\` няма два операнда.`, { error: true });
        return { frames, rpn, value: null };
      }
      const b = values.pop();
      const a = values.pop();
      let r;
      try {
        r = apply(x, a, b);
      } catch (e) {
        evalSnap(p, `Грешка: ${e.message}.`, { error: true });
        return { frames, rpn, value: null };
      }
      values.push(r);
      evalSnap(p, `\`${x}\`: вадим ${b} (десният!) и ${a}; ${a} ${x} ${b} = ${r}.`);
    } else {
      values.push(Number(x));
      evalSnap(p, `${x} → в стека.`);
    }
  }
  if (values.length !== 1) {
    evalSnap(rpn.length, "Грешка: в стека не остана точно едно число.", { error: true });
    return { frames, rpn, value: null };
  }
  evalSnap(rpn.length, `Резултат: ${values[0]}.`, { done: true });
  return { frames, rpn, value: values[0] };
}
