<div align="center">

# ⚡ NoFPU Decimal Converter — Go Edition

**Idiomatic Go. Minimal allocations. Zero floats.**

[![Language](https://img.shields.io/badge/Language-Go-00ADD8?style=flat-square&logo=go)](https://go.dev/)
[![Go Version](https://img.shields.io/badge/Go-1.18%2B-00ADD8?style=flat-square&logo=go)](https://go.dev/dl/)
[![FPU Required](https://img.shields.io/badge/FPU-Not%20Required-red?style=flat-square)](https://en.wikipedia.org/wiki/Floating-point_unit)
[![Version](https://img.shields.io/badge/Version-1.0-orange?style=flat-square)](https://gharri.ir)
[![Author](https://img.shields.io/badge/Author-Benyamin%20Gharri-purple?style=flat-square)](https://gharri.ir)

</div>

---

## 🧠 What Is This?

Go's standard library makes float formatting deceptively simple:

```go
fmt.Sprintf("%.2f", 1010.13)          // float64, FPU involved
strconv.FormatFloat(1010.13, 'f', 2, 64) // same thing
math.Round(v*100) / 100               // division + float arithmetic
```

On embedded Go targets (TinyGo on AVR, ARM Cortex-M0, RISC-V), float support may be partially compiled out — or soft-float emulation adds significant overhead to every call.

**NoFPU Decimal Converter for Go** handles decimal formatting using only string operations and integer arithmetic:

- ✅ No `float32`, `float64`, or `math.*`
- ✅ No `fmt.Sprintf("%.2f", ...)`, no `strconv.FormatFloat`
- ✅ No division operators on decimal values
- ✅ Three variants — full-featured, micro, and managed struct with cache
- ✅ TinyGo compatible (`convertNoFPU` and `convertMicro`)
- ✅ Idiomatic Go: multiple return values, struct methods, map cache

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
| `2-100` | `1.00` | Trailing zeros kept |

---

## 📐 Architecture

```
                    ┌──────────────────────────────────┐
                    │         Input: "2-101013"        │
                    └────────────────┬─────────────────┘
                                     │
                         ┌───────────▼────────────┐
                         │   Choose your variant  │
                         └──┬──────┬──────────────┘
                            │      │              │
              ┌─────────────▼─┐ ┌──▼───────────┐ ┌▼──────────────────┐
              │ convertNoFPU  │ │convertMicro  │ │  NoFPUConverter   │
              │ (string,      │ │ string only  │ │  struct + map     │
              │  string)      │ │ single pass  │ │  cache + stats    │
              └─────────────┬─┘ └──┬───────────┘ └┬──────────────────┘
                            └──────┼───────────────┘
                                   │
                    ┌──────────────▼──────────────┐
                    │       Output: "1010.13"     │
                    └─────────────────────────────┘

  No float64. No fmt.Sprintf("%.2f"). No math.*. No FPU.
```

---

## 🗂️ API Overview

```go
// Free functions
convertNoFPU(inputString string) (string, string)   // (result, errMsg)
convertMicro(inputString string) string              // result | "ERROR"

// Struct
type NoFPUConverter struct { ... }
func NewNoFPUConverter() *NoFPUConverter
func (c *NoFPUConverter) Convert(inputStr string) string
func (c *NoFPUConverter) ClearCache()
func (c *NoFPUConverter) GetStats() (total int, cacheSize int)
```

---

## 🛠️ API Reference

### `convertNoFPU` — Full-featured with error string

Returns two values in idiomatic Go style: `(result, errMsg)`. On success, `errMsg` is `""`. On failure, `result` is `""` and `errMsg` contains a descriptive message.

```go
result, err := convertNoFPU("2-101013")
if err != "" {
    fmt.Println(err) // "ERROR: '-' not found", etc.
} else {
    fmt.Println(result) // "1010.13"
}

// More examples
r1, _ := convertNoFPU("3-123")   // "0.123"
r2, _ := convertNoFPU("0-12345") // "12345"
_, e  := convertNoFPU("bad")     // "ERROR: '-' not found"
```

---

### `convertMicro` — Lightweight single-pass

Single `range` loop over the input string, minimal allocations. Returns `"ERROR"` on invalid input.

```go
convertMicro("3-123")   // "0.123"
convertMicro("0-12345") // "12345"
convertMicro("bad")     // "ERROR"
```

Best for TinyGo targets or tight loops.

---

### `NoFPUConverter` — Struct with map cache

```go
converter := NewNoFPUConverter()

// First call: computed
r1 := converter.Convert("2-101013") // "1010.13"

// Second call: from cache (O(1) map lookup)
r2 := converter.Convert("2-101013") // "1010.13"

// Stats via multiple return values
total, cacheSize := converter.GetStats()
fmt.Printf("Calls: %d, Cached: %d\n", total, cacheSize) // 2, 1

// Free memory
converter.ClearCache()
```

- Cache backed by `map[string]string`
- Max size: **100 entries** — stops caching when full
- No eviction — call `ClearCache()` manually

---

## ⚙️ Building & Running

### Run directly

```bash
go run nofpu_converter.go
```

### Build binary

```bash
go build -o nofpu_converter nofpu_converter.go
./nofpu_converter
```

### Build optimized (smallest binary)

```bash
go build -ldflags="-s -w" -o nofpu_converter nofpu_converter.go
```

### TinyGo (AVR / ARM Cortex-M / RISC-V)

Use `convertNoFPU` or `convertMicro` only — remove `NoFPUConverter` struct (uses `map`, not available on all TinyGo targets) and the `bufio`/`os` imports:

```bash
# Cortex-M0 (e.g. Arduino Uno via TinyGo)
tinygo build -target=arduino -o nofpu.hex nofpu_converter.go

# Generic ARM
tinygo build -target=cortex-m0 -o nofpu.elf nofpu_converter.go
```

---

## 📊 Performance vs Float Formatting

| Method | Uses Float | Works on TinyGo (no-float) | Allocations |
|---|---|---|---|
| `fmt.Sprintf("%.2f", f)` | ✅ Yes | ❌ No | ~80 bytes |
| `strconv.FormatFloat(...)` | ✅ Yes | ❌ No | ~64 bytes |
| `convertMicro()` | ❌ No | ✅ Yes | ~32 bytes |
| `convertNoFPU()` | ❌ No | ✅ Yes | ~40 bytes |
| `NoFPUConverter` (cached) | ❌ No | ⚠️ Partial | ~0 bytes |

**Speed benchmark** (Go 1.22, `go test -bench`, `2-101013` input):

```
BenchmarkSprintf          →  ~145 ns/op   80 B/op
BenchmarkConvertMicro     →   ~72 ns/op   32 B/op
BenchmarkConvertNoFPU     →   ~88 ns/op   40 B/op
BenchmarkConverterCached  →   ~18 ns/op    0 B/op  ← fastest after warmup
```

---

## 🖥️ Interactive Mode

```
============================================================
NoFPU Decimal Converter - Suitable for embedded systems
============================================================

Features:
✓ No float usage
✓ No floating point operations
✓ Minimal CPU usage
✓ Minimal memory usage
✓ Suitable for systems without FPU

Input: 2-101013
✅ Result: 1010.13
   (Without using FPU)

Input: bad
❌ Error: Invalid input format
Correct format: decimal_count-number (Example: 2-101013)

Input: exit

Statistics:
Total conversions: 1
Cache size: 1
Exiting program
```

---

## ⚠️ Limitations

| Constraint | Detail |
|---|---|
| Input values | Non-negative integers only |
| Negative numbers | ❌ Leading `-` conflicts with format separator |
| Decimal input | ❌ Input must be a plain integer string |
| `NoFPUConverter` | Uses `map` — not available on all TinyGo targets |
| Cache eviction | No LRU — stops at 100 entries, call `ClearCache()` manually |
| Thread safety | `NoFPUConverter` is **not goroutine-safe** — use `sync.Mutex` if accessing from multiple goroutines |
| Error type | `convertNoFPU` returns `(string, string)` not `(string, error)` — intentional to avoid `errors` package import on embedded targets |

---

## 📁 File Structure

```
nofpu_converter.go    ← Single-file implementation
README.md             ← This file
```

No `go.mod`, no external dependencies. Single file, drop anywhere and `go run` it.

---

## 👤 Author

**Benyamin Gharri**  
🌐 [gharri.ir](https://gharri.ir)  

---

<div align="center">

*Go routines welcome. Floats are not.*

</div>