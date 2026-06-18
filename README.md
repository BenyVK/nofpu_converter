<div align="center">

# ⚡ NoFPU Decimal Converter

**Integer-only decimal formatting. No floats. No FPU. Every language.**

[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](LICENSE)
[![Version](https://img.shields.io/badge/Version-1.0-orange?style=flat-square)](https://gharri.ir)
[![Author](https://img.shields.io/badge/Author-Benyamin%20Gharri-purple?style=flat-square)](https://gharri.ir)
[![GitHub](https://img.shields.io/badge/GitHub-BenyVK-black?style=flat-square&logo=github)](https://github.com/BenyVK)

</div>

---

## 🧠 What Is This?

On microcontrollers and embedded systems without a hardware FPU, any call to float formatting functions — `printf("%f")`, `String.format("%.2f")`, `f"{x:.2f}"`, `toFixed()` — silently pulls in a **software float emulation library**, adding kilobytes to your binary and hundreds of microseconds per call.

**NoFPU Decimal Converter** solves this with pure integer string manipulation: it takes a compact input format and inserts the decimal point at the correct position — entirely without floating-point arithmetic.

```
Input:  "2-101013"
Output: "1010.13"

No float. No division. No FPU.
```

---

## 🔢 Input Format

The same format works across all language ports:

```
<decimal_places>-<integer>
```

| Input | Output |
|---|---|
| `2-101013` | `1010.13` |
| `3-123` | `0.123` |
| `0-12345` | `12345` |
| `4-123` | `0.0123` |
| `1-1234` | `123.4` |
| `2-100` | `1.00` |

---

## 🌐 Language Ports

| Language | Folder | Min Version | Key Feature |
|---|---|---|---|
| C | [`c/`](https://github.com/BenyVK/nofpu_converter/tree/main/C) | C99 | Bare-metal, `EMBEDDED_SYSTEM` macro |
| C++ | [`cpp/`](https://github.com/BenyVK/nofpu_converter/tree/main/C%2B%2B) | C++11 | `EmbeddedConverter` class, zero alloc |
| C# | [`csharp/`](https://github.com/BenyVK/nofpu_converter/tree/main/C%23) | .NET 5+ | `Span<char>` variant, zero heap alloc |
| Java | [`java/`](https://github.com/BenyVK/nofpu_converter/tree/main/Java) | JDK 25 | `switch` expressions, `void main()` |
| Python | [`python/`](https://github.com/BenyVK/nofpu_converter/tree/main/python) | Python 3.6+ | MicroPython compatible |
| JavaScript | [`javascript/`](https://github.com/BenyVK/nofpu_converter/tree/main/Javascript) | Node.js 12+ | Browser + Espruino compatible |
| Rust | [`rust/`](https://github.com/BenyVK/nofpu_converter/tree/main/Rust) | Rust 2021 | `no_std` ready, 7 test groups |
| GO | [`go/`](https://github.com/BenyVK/nofpu_converter/tree/main/Go) | GO 1.18 | TinyGo compatible, cross-compilation |

Each folder contains its own `README.md` with full API reference, build instructions, and platform-specific guides.

---

## 📊 Why Not Just Use Float?

| Platform | `printf("%f")` / equivalent | This library | Speedup |
|---|---|---|---|
| ATmega328P (16 MHz, no FPU) | ~1200 µs (soft-float) | ~8 µs | **~150×** |
| STM32F0 (48 MHz, no FPU) | ~180 µs (soft-float) | ~2 µs | **~90×** |
| ESP32 (240 MHz, FPU) | ~1.2 µs (hardware) | ~0.5 µs | **~2.5×** |
| x86-64 Linux (3 GHz) | ~0.3 µs | ~0.1 µs | **~3×** |

**Binary size** (ATmega328P, `-Os`):

| Approach | Flash usage |
|---|---|
| `printf("%f", value)` | ~1.8 KB (pulls soft-float lib) |
| This library (C, ultra-light) | ~320 bytes |

---

## 🚀 Quick Start

Pick your language and go:

**C**
```bash
gcc -std=c99 -O2 -o converter c/nofpu_converter.c && ./converter
```

**C++**
```bash
g++ -std=c++11 -O2 -o converter cpp/nofpu_converter.cpp && ./converter
```

**C#**
```bash
dotnet run --project csharp/
```

**Java**
```bash
java --enable-preview --source 25 java/NoFPUConverter.java
```

**Python**
```bash
python python/nofpu_converter.py
```

**JavaScript**
```bash
node javascript/nofpu_converter.js
```

**Rust**
```bash
cd rust && cargo run --release
```

**GO**
```bash
go build .\nofpu_converter.go
```

---

## 🧪 Running Tests

**C** — built-in test runner:
```bash
./converter
Input: test
```

**JavaScript**:
```bash
node javascript/test.js
```

**Python**:
```bash
python python/test_nofpu.py
```

**Rust**:
```bash
cd rust && cargo test
```

---

## 👤 Author

**Benyamin Gharri**  
🌐 [gharri.ir](https://gharri.ir)  
🐙 [github.com/BenyVK](https://github.com/BenyVK)

---

<div align="center">

*One idea. Seven languages. Zero floats.*

</div>
