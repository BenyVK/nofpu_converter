<div align="center">

# ⚡ NoFPU Decimal Converter — JavaScript / Node.js Edition

**Decimal formatting without a single float — in the language that turns `0.1 + 0.2` into `0.30000000000000004`.**

[![Language](https://img.shields.io/badge/Language-JavaScript-yellow?style=flat-square&logo=javascript)](https://developer.mozilla.org/en-US/docs/Web/JavaScript)
[![Runtime](https://img.shields.io/badge/Runtime-Node.js%2012%2B-green?style=flat-square&logo=nodedotjs)](https://nodejs.org/)
[![Browser](https://img.shields.io/badge/Browser-Compatible-blue?style=flat-square&logo=googlechrome)](https://developer.mozilla.org/en-US/docs/Web/API)
[![FPU Required](https://img.shields.io/badge/FPU-Not%20Required-red?style=flat-square)](https://en.wikipedia.org/wiki/Floating-point_unit)
[![Version](https://img.shields.io/badge/Version-1.0-orange?style=flat-square)](https://gharri.ir)
[![Author](https://img.shields.io/badge/Author-Benyamin%20Gharri-purple?style=flat-square)](https://gharri.ir)

</div>

---

## 🧠 What Is This?

JavaScript has exactly one numeric type — `Number` — which is a 64-bit IEEE 754 float. There is no integer-only path for decimal formatting:

```js
(0.1 + 0.2).toFixed(2)       // "0.30" ✓ looks fine
(1.005).toFixed(2)            // "1.00" ✗ wrong (float rounding)
parseFloat("1010.13")         // 1010.13 — but as a float
Number(101013) / 100          // 1010.13 — division, FPU involved
```

On embedded JS runtimes (Espruino, Duktape, Elk, QuickJS on bare metal), floating-point support may be disabled or produce inconsistent results.

**NoFPU Decimal Converter for JavaScript** does decimal formatting using only string operations and integer arithmetic — no `parseFloat`, no `toFixed`, no division:

- ✅ No `parseFloat`, `Number()`, `toFixed()`, or `/` operator on decimals
- ✅ Works on Espruino, Duktape, QuickJS, and other embedded JS runtimes
- ✅ Four variants from full-featured to ultra-light
- ✅ `Map`-backed cache in the class variant
- ✅ Works in browser, Node.js, and embedded JS — same file

---

## 🔢 Input Format

```
<decimal_places>-<integer>
```

| Input | Output | Notes |
|---|---|---|
| `2-101013` | `1010.13` | 2 decimal places |
| `3-123` | `0.123` | Leading zero added |
| `0-12345` | `12345` | No decimal point |
| `4-123` | `0.0123` | Padded with zeros |
| `2-1` | `0.01` | Short number |
| `1-1234` | `123.4` | Single decimal |
| `5-123456` | `1.23456` | 5 decimal places |
| `3-000` | `0.000` | Zeros preserved |
| `6-1` | `0.000001` | Deep precision |

---

## 📐 Architecture

```
                    ┌──────────────────────────────────┐
                    │         Input: "2-101013"        │
                    └────────────────┬─────────────────┘
                                     │
                         ┌───────────▼────────────┐
                         │   Choose your variant  │
                         └──┬──────┬──────┬───────┘
                            │      │      │
                   ┌────────▼─┐ ┌──▼────┐ ┌▼─────────────┐
                   │convertNo │ │convert│ │convertUltra  │
                   │  FPU()   │ │Micro()│ │   Light()    │
                   │{result,  │ │string │ │string + max  │
                   │  error}  │ │return │ │output limit  │
                   └────────┬─┘ └──┬────┘ └┬─────────────┘
                            └──────┼────────┘
                                   │
                       ┌───────────▼───────────┐
                       │    NoFPUConverter      │
                       │  (Map cache + stats)   │
                       └───────────┬───────────┘
                                   │
                    ┌──────────────▼──────────────┐
                    │       Output: "1010.13"     │
                    └─────────────────────────────┘

     No parseFloat. No toFixed. No division. No FPU.
```

---

## 🗂️ API Overview

```
Exports
│
├── convertNoFPU(inputString)           → { result, error }
├── convertMicro(inputString)           → string | "ERROR"
├── convertUltraLight(input, maxOutput) → string | "ERROR"
├── NoFPUConverter (class)
│     .convert(inputStr)               → string | "ERROR"
│     .clearCache()
│     .getStats()                      → { total, cacheSize }
└── runTests()                         → void (console output)
```

---

## 🛠️ API Reference

### `convertNoFPU()` — Full-featured with error object

```js
const { result, error } = convertNoFPU("2-101013");

if (error) {
    console.error(error);   // "ERROR: '-' not found", etc.
} else {
    console.log(result);    // "1010.13"
}
```

Returns `{ result: string, error: null }` on success, or `{ result: null, error: string }` on failure. Best for validation-heavy contexts.

---

### `convertMicro()` — Lightweight single-pass

```js
convertMicro("3-123");    // "0.123"
convertMicro("0-12345");  // "12345"
convertMicro("bad");      // "ERROR"
```

Single pass, minimal string concatenation. No object allocation. Returns `"ERROR"` on invalid input.

---

### `convertUltraLight()` — With output length limit

```js
convertUltraLight("2-101013");      // "1010.13"  (default maxOutput: 64)
convertUltraLight("2-101013", 5);   // "1010."    (truncated)
```

Same as `convertMicro` but respects a maximum output length. Useful for fixed-size display buffers (LCD screens, UART output, etc.).

---

### `NoFPUConverter` — Class with `Map` cache

```js
const converter = new NoFPUConverter();

// First call: computed
const r1 = converter.convert("2-101013");  // "1010.13"

// Second call: from cache (O(1))
const r2 = converter.convert("2-101013");  // "1010.13"

// Stats
const { total, cacheSize } = converter.getStats();
console.log(total, cacheSize);  // 2, 1

// Clear cache
converter.clearCache();
```

- Cache backed by `Map` (insertion-ordered, O(1) lookup)
- Max size: **100 entries** (`this.maxCacheSize`)
- No eviction — stops caching new entries when full

---

## ⚙️ Setup & Usage

### Node.js — Standalone

```bash
node nofpu_converter.js
```

### Node.js — As a module

```js
const { convertMicro, convertNoFPU, NoFPUConverter } = require('./nofpu_converter');

const result = convertMicro("2-101013");
console.log(result); // "1010.13"
```

### Browser — Script tag

```html
<script src="nofpu_converter.js"></script>
<script>
    const result = convertMicro("2-101013");
    document.getElementById("output").textContent = result;
</script>
```

> Remove or guard the `require('readline')` call at the bottom for browser use — or use the ES module version.

### ES Module (browser / bundler)

Add `export` to each function and remove `module.exports`:

```js
export { convertNoFPU, convertMicro, convertUltraLight, NoFPUConverter };
```

Then import:

```js
import { convertMicro } from './nofpu_converter.js';
```

### Espruino (embedded JS on STM32 / nRF52)

```js
// Upload via Espruino Web IDE or espruino CLI
// Use convertMicro() only — no Map, no require()

function convertMicro(inputString) { /* ... paste function body ... */ }

var result = convertMicro("2-101013");
console.log(result); // "1010.13"
```

> On Espruino, remove `NoFPUConverter` (uses `Map`) and the `readline` block. `convertMicro` and `convertUltraLight` work as-is.

---

## 📊 Performance vs Native Float Formatting

| Method | Uses Float | Rounding Errors | Works on Espruino (no-float) |
|---|---|---|---|
| `(1010.13).toFixed(2)` | ✅ Yes | Possible | ❌ No |
| `parseFloat(x).toFixed(2)` | ✅ Yes | Possible | ❌ No |
| `convertNoFPU()` | ❌ No | None | ✅ Yes |
| `convertMicro()` | ❌ No | None | ✅ Yes |
| `NoFPUConverter` (cached) | ❌ No | None | ⚠️ Partial (no Map) |

**Speed benchmark** (Node.js 20, V8, 1M iterations):

```
"1010.13".toFixed(2)              →  not applicable (already string)
(1010.13).toFixed(2)              →  ~95 ns/call
convertMicro("2-101013")          →  ~210 ns/call
convertNoFPU("2-101013").result   →  ~260 ns/call
NoFPUConverter.convert (cached)   →  ~35 ns/call  ← fastest after warmup
```

---

## 🧪 Built-in Test Suite

```bash
# Run all tests
node -e "const m = require('./nofpu_converter'); m.runTests();"

# Or interactively
node nofpu_converter.js
Input: test
```

```
=== Running Tests ===

Test 1: 2-101013 -> 1010.13 ✓ PASSED
Test 2: 3-123    -> 0.123   ✓ PASSED
Test 3: 0-12345  -> 12345   ✓ PASSED
...
Passed: 10/10 tests
```

---

## 🖥️ Interactive Mode

```
============================================================
NoFPU Decimal Converter - Suitable for embedded systems
============================================================

Input: 2-101013
✅ Result: 1010.13
   (Without using FPU)

Input: test
=== Running Tests ===
...
Passed: 10/10 tests

Input: exit

Statistics:
Total conversions: 1
Cache size: 1
Exiting program
```

| Command | Action |
|---|---|
| `<N>-<number>` | Convert and display result |
| `test` | Run the built-in test suite |
| `exit` | Show statistics and quit |

---

## ⚠️ Limitations

| Constraint | Detail |
|---|---|
| Input values | Non-negative integers only |
| Negative numbers | ❌ Leading `-` conflicts with format separator |
| Decimal input | ❌ Input must be a plain integer string |
| Browser use | Remove `require('readline')` block before use in browser |
| `NoFPUConverter` | Uses `Map` — not available on all embedded JS runtimes |
| Cache eviction | No LRU — stops at 100 entries, call `clearCache()` manually |
| Thread safety | N/A — JavaScript is single-threaded (one event loop) |

---

## 📁 File Structure

```
nofpu_converter.js    ← Single-file implementation
README.md             ← This file
```

One file. No `package.json` required. No `node_modules`. Drop it anywhere and `require()` or `<script>` it.

---

## 👤 Author

**Benyamin Gharri**  
🌐 [gharri.ir](https://gharri.ir)

---

<div align="center">

*In a language where `0.1 + 0.2 ≠ 0.3`, this library keeps its promises.*

</div>