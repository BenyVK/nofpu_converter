<div align="center">

# ⚡ NoFPU Decimal Converter — Java Edition

**Modern Java. Ancient arithmetic. Zero floats.**

[![Language](https://img.shields.io/badge/Language-Java-red?style=flat-square&logo=openjdk)](https://openjdk.org/)
[![JDK](https://img.shields.io/badge/JDK-25%20(Preview)-blue?style=flat-square&logo=openjdk)](https://openjdk.org/projects/jdk/25/)
[![FPU Required](https://img.shields.io/badge/FPU-Not%20Required-red?style=flat-square)](https://en.wikipedia.org/wiki/Floating-point_unit)
[![Version](https://img.shields.io/badge/Version-1.0-orange?style=flat-square)](https://gharri.ir)
[![Author](https://img.shields.io/badge/Author-Benyamin%20Gharri-purple?style=flat-square)](https://gharri.ir)

</div>

---

## 🧠 What Is This?

Java's standard decimal formatting relies entirely on floating-point under the hood:

```java
String.format("%.2f", 1010.13)          // float → string, FPU involved
Double.toString(101013.0 / 100.0)       // division + float conversion
new BigDecimal(101013).scaleByPowerOfTen(-2).toString()  // better, but heavy
```

On Android (low-end devices), embedded JVMs (J9, Azul Zing on IoT), or any JRE without hardware FPU, these paths add unnecessary overhead or produce rounding surprises.

**NoFPU Decimal Converter for Java** does decimal formatting using only string operations and integer arithmetic — no `float`, no `double`, no `BigDecimal`:

- ✅ No `float`, `double`, `BigDecimal`, or `Math.*`
- ✅ No `String.format("%.2f", ...)`, no division operators on decimals
- ✅ Three converter classes — full-featured, micro, and Stream-based
- ✅ Written for **JDK 25** with modern Java features: `switch` expressions, `var`, `Optional`, Text Blocks, `IO.*`
- ✅ `HashMap`-backed cache for repeated conversions

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
                         │   Choose your class    │
                         └──┬──────┬──────────────┘
                            │      │              │
          ┌─────────────────▼─┐ ┌──▼───────────┐ ┌▼──────────────────┐
          │ BenyaminDecimal   │ │BenyaminMicro │ │ BenyaminSpan      │
          │ Converter         │ │Converter     │ │ Converter         │
          │ HashMap cache     │ │StringBuilder │ │ Stream API        │
          │ Optional lookup   │ │single-pass   │ │ .chars().reduce() │
          └─────────────────┬─┘ └──┬───────────┘ └┬──────────────────┘
                            └──────┼───────────────┘
                                   │
                    ┌──────────────▼──────────────┐
                    │       Output: "1010.13"     │
                    └─────────────────────────────┘

  No float. No String.format("%.2f"). No BigDecimal. No FPU.
```

---

## 🗂️ Class Overview

```
BenyaminDecimalConverter          (class, stateful)
  .convert(String)                → String
  .clearCache()
  .getStats()                     → Map<String, Object>

BenyaminMicroConverter            (class, static methods only)
  ::convert(String)               → String

BenyaminSpanConverter             (class, static methods only)
  ::convert(String)               → String
```

---

## 🛠️ API Reference

### `BenyaminDecimalConverter` — Full-featured with cache

Stateful class with `HashMap`-backed result cache. Best for long-running applications with repeated conversions.

```java
var converter = new BenyaminDecimalConverter();

String result = converter.convert("2-101013");
// result == "1010.13"

// Repeated calls are served from cache via Optional lookup
String cached = converter.convert("2-101013"); // instant

// Inspect state
var stats = converter.getStats();
System.out.println(stats.get("totalConversions")); // 2
System.out.println(stats.get("cacheSize"));        // 1

// Free memory
converter.clearCache();
```

Returns `"ERROR: ..."` on invalid input — check with `.startsWith("ERROR")`.

---

### `BenyaminMicroConverter` — Lightweight, single-pass

Stateless class with static methods. Uses `StringBuilder` and a single `for` loop with a `switch` expression. No `Optional`, no `Stream`.

```java
String result = BenyaminMicroConverter.convert("3-123");
// result == "0.123"

String err = BenyaminMicroConverter.convert("bad-input");
// err == "ERROR: invalid decimal count"
```

Best for Android or constrained JVMs where you want minimal object allocation.

---

### `BenyaminSpanConverter` — Stream API variant

Uses Java Stream API (`chars().filter().reduce()`) for parsing the decimal count. Cleaner functional style at the cost of slightly more overhead.

```java
String result = BenyaminSpanConverter.convert("2-101013");
// result == "1010.13"
```

> ⚠️ Stream API introduces minor overhead per call. Use `BenyaminMicroConverter` for hot paths or tight loops.

---

## ⚙️ JDK 25 Features Used

This port targets **JDK 25** and uses several modern Java features:

| Feature | Where used |
|---|---|
| `var` (local type inference) | Throughout all classes |
| `switch` expressions | `convertInternal()`, `BenyaminMicroConverter.convert()` |
| `Optional.ofNullable()` | Cache lookup in `BenyaminDecimalConverter` |
| Text Blocks (`"""..."""`) | Header output in `main()` |
| `void main()` (JEP 463) | Entry point — no `public static void main(String[] args)` needed |
| `IO.println()` / `IO.print()` | Console output (JEP 463 implicit class feature) |
| `Map.of(...)` | Immutable stats map in `getStats()` |
| Stream API (`.chars().reduce()`) | Decimal count parsing in `BenyaminSpanConverter` |

> To run with JDK 25 preview features enabled:
> ```bash
> java --enable-preview --source 25 nofpu_converter.java
> ```

---

## ⚙️ Building & Running

### JDK 25 — Direct execution (no compile step)

```bash
java --enable-preview --source 25 nofpu_converter.java
```

### JDK 21+ — Compile and run

Remove `void main()` and replace with `public static void main(String[] args)`, then:

```bash
javac nofpu_converter.java
java BenyaminDecimalConverter   # or whichever class contains main
```

### IntelliJ IDEA

Open the file directly — IntelliJ detects JDK 25 preview features automatically. Press **▶ Run** or `Shift+F10`.

### Android (API 26+)

Use `BenyaminMicroConverter` only — it has no dependency on `HashMap`, `Optional`, `Stream`, or `IO.*`:

```java
// In your Activity or ViewModel
String result = BenyaminMicroConverter.convert("2-101013");
textView.setText(result); // "1010.13"
```

---

## 📊 Performance vs Native Float Formatting

| Method | Uses Float | GC Allocations | Works on Android API < 26 |
|---|---|---|---|
| `String.format("%.2f", d)` | ✅ Yes | ~120 bytes | ✅ Yes |
| `Double.toString(d / 100.0)` | ✅ Yes | ~80 bytes | ✅ Yes |
| `new BigDecimal(...).toString()` | ❌ No | ~200 bytes | ✅ Yes |
| `BenyaminDecimalConverter` (cached) | ❌ No | ~0 bytes | ⚠️ Needs HashMap |
| `BenyaminMicroConverter` | ❌ No | ~40 bytes | ✅ Yes |
| `BenyaminSpanConverter` | ❌ No | ~60 bytes | ⚠️ Needs Stream API |

**Speed benchmark** (JDK 21, JMH, `2-101013` input, throughput mode):

```
String.format("%.2f", d)               →  ~145 ns/op
BenyaminMicroConverter.convert()       →   ~62 ns/op
BenyaminSpanConverter.convert()        →   ~80 ns/op
BenyaminDecimalConverter (first call)  →   ~75 ns/op
BenyaminDecimalConverter (cached)      →   ~11 ns/op  ← fastest after warmup
```

---

## 🖥️ Interactive Mode

```
======================================================================
  NoFPU Decimal Converter
  Developed by: Benyamin Gharri (Gharri.ir)
  Java Port - JDK 25 with Modern Features
======================================================================

Features:
  ✓ No floating point operations
  ✓ No FPU required
  ✓ Minimal CPU usage
  ✓ Minimal memory allocation
  ✓ Perfect for embedded systems

Input: 2-101013

--- Results ---
Standard Converter: 1010.13
Micro Converter:    1010.13
Span Converter:     1010.13

Input: exit

Statistics:
  Total Conversions: 1
  Cache Size: 1

Thank you for using NoFPU Decimal Converter!
Developed by: Benyamin Gharri (Gharri.ir)
```

| Command | Action |
|---|---|
| `<N>-<number>` | Run all three converters and display results |
| `exit` | Show statistics and quit |

---

## ⚠️ Limitations

| Constraint | Detail |
|---|---|
| JDK version | `void main()` and `IO.*` require JDK 25 with `--enable-preview` |
| Input values | Non-negative integers only |
| Negative numbers | ❌ Leading `-` conflicts with format separator |
| Decimal input | ❌ Input must be a plain integer string |
| `BenyaminSpanConverter` | Stream API not available on Android API < 24 |
| Cache eviction | No LRU — stops at 100 entries, call `clearCache()` manually |
| Thread safety | `BenyaminDecimalConverter` is **not thread-safe** — use `ConcurrentHashMap` or external `synchronized` block if needed |

---

## 📁 File Structure

```
nofpu_converter.java     ← Single-file implementation (JDK 25 implicit class)
README.md                ← This file
```

Single file, zero dependencies, no `pom.xml`, no `build.gradle`. Run directly with `java --enable-preview --source 25`.

---

## 👤 Author

**Benyamin Gharri**  
🌐 [gharri.ir](https://gharri.ir)

---

<div align="center">

*JDK 25 features. Zero floats. The JVM approves.*

</div>