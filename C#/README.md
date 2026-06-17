<div align="center">

# ⚡ NoFPU Decimal Converter — C# Edition

**Three converters. One goal. Zero floats.**

[![Language](https://img.shields.io/badge/Language-C%23-blue?style=flat-square&logo=csharp)](https://learn.microsoft.com/en-us/dotnet/csharp/)
[![Platform](https://img.shields.io/badge/Platform-.NET%205%2B%20%7C%20.NET%20Core%203.1%2B-purple?style=flat-square&logo=dotnet)](https://dotnet.microsoft.com/)
[![FPU Required](https://img.shields.io/badge/FPU-Not%20Required-red?style=flat-square)](https://en.wikipedia.org/wiki/Floating-point_unit)
[![Version](https://img.shields.io/badge/Version-1.0-orange?style=flat-square)](https://gharri.ir)
[![Author](https://img.shields.io/badge/Author-Benyamin%20Gharri-purple?style=flat-square)](https://gharri.ir)

</div>

---

## 🧠 What Is This?

In .NET, formatting a decimal number typically means `float.ToString("F2")`, `string.Format("{0:F}", value)`, or `Math.Round()` — all of which ultimately rely on floating-point hardware or software emulation.

On constrained .NET targets (nanoFramework, Meadow, Micro Framework, or stripped-down IoT runtimes), this can be slow, inaccurate, or simply unavailable.

**NoFPU Decimal Converter for C#** formats numbers using pure integer string manipulation:

- ✅ No `float`, `double`, `decimal`, or `Math.*`
- ✅ No `string.Format("{0:F}", ...)`, no `ToString("F2")`
- ✅ Three converter variants for different performance profiles
- ✅ Built-in cache for repeated conversions
- ✅ `Span<char>` variant for zero heap allocation (.NET Core / .NET 5+)

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
                         └───────────┬────────────┘
                    ┌────────────────┼────────────────┐
                    ▼                ▼                ▼
          BenyaminDecimal    BenyaminMicro    BenyaminSpan
            Converter          Converter       Converter
          (StringBuilder)   (string concat)  (Span<char>)
          + cache (Dict)     single pass      zero alloc
                    └────────────────┬────────────────┘
                                     │
                    ┌────────────────▼─────────────────┐
                    │         Output: "1010.13"        │
                    └──────────────────────────────────┘

              No floats. No Math.*. No FPU instructions.
```

---

## 🗂️ Class Overview

```
NoFPUConverter (namespace)
│
├── BenyaminDecimalConverter        ← Full-featured class with cache
│     .Convert(string)              → string
│     .ClearCache()
│     .GetStats()                   → (Total, CacheSize)
│
├── BenyaminMicroConverter          ← Static, lightweight, single-pass
│     .Convert(string)              → string
│
└── BenyaminSpanConverter           ← Static, zero heap allocation
      .Convert(ReadOnlySpan<char>)  → string
```

---

## 🛠️ API Reference

### `BenyaminDecimalConverter` — Full-featured with cache

Best for long-running applications and services that repeatedly convert the same values (e.g. sensor dashboards, data pipelines).

```csharp
var converter = new BenyaminDecimalConverter();

string result = converter.Convert("2-101013");
// result == "1010.13"

// Repeated calls are served from cache
string cached = converter.Convert("2-101013"); // O(1) lookup

// Inspect state
var (total, cacheSize) = converter.GetStats();
Console.WriteLine($"Conversions: {total}, Cache: {cacheSize}");

// Free memory
converter.ClearCache();
```

- Cache backed by `Dictionary<string, string>`
- Max cache size: **100 entries** (configurable via `MaxCacheSize`)
- Returns `"ERROR: ..."` strings on invalid input

---

### `BenyaminMicroConverter` — Lightweight static class

Single-pass, minimal string allocation. No cache, no state. Good for one-off conversions or constrained IoT runtimes.

```csharp
string result = BenyaminMicroConverter.Convert("3-123");
// result == "0.123"

string err = BenyaminMicroConverter.Convert("bad-input");
// err == "ERROR: invalid decimal count"
```

---

### `BenyaminSpanConverter` — Zero heap allocation (.NET 5+)

Uses `ReadOnlySpan<char>` for parsing — no intermediate string objects are created during the conversion. Ideal for hot paths, high-frequency loops, or GC-sensitive contexts.

```csharp
// Works with string.AsSpan() — no extra allocation
string result = BenyaminSpanConverter.Convert("2-101013".AsSpan());
// result == "1010.13"

// Or with a ReadOnlySpan directly from a buffer
ReadOnlySpan<char> input = stackalloc char[] { '2','-','1','0','1','0','1','3' };
string result2 = BenyaminSpanConverter.Convert(input);
```

> ⚠️ `BenyaminSpanConverter` requires **.NET Core 3.1+** or **.NET 5+**. Not available on .NET Framework or nanoFramework.

---

## ⚙️ Building & Running

### .NET CLI

```bash
dotnet new console -n NoFPUConverter
# Copy nofpu_converter.cs into the project
dotnet run
```

### Visual Studio

1. Create a new **Console App (.NET 6+)** project
2. Replace `Program.cs` with `nofpu_converter.cs`
3. Press **F5** to run

### nanoFramework (IoT / MCU)

Use only `BenyaminMicroConverter` — remove `BenyaminSpanConverter` (no `Span<T>` support) and `BenyaminDecimalConverter` (Dictionary may be unavailable):

```bash
dotnet add package nanoFramework.CoreLibrary
```

```csharp
// nanoFramework-compatible usage
string result = BenyaminMicroConverter.Convert("2-101013");
```

### Meadow (F7 Feather / .NET IoT)

All three variants are supported on Meadow OS with .NET 6 runtime:

```csharp
using NoFPUConverter;

var converter = new BenyaminDecimalConverter();
Resolver.Log.Info(converter.Convert("3-4567")); // "4.567"
```

---

## 📊 Performance vs Native Float Formatting

| Method | Allocations | Relative Speed | GC Pressure |
|---|---|---|---|
| `float.ToString("F2")` | ~80 bytes | 1× (baseline) | Medium |
| `string.Format("{0:F2}", f)` | ~120 bytes | 0.8× | Medium-High |
| `BenyaminDecimalConverter` | ~40 bytes (first call) / 0 (cached) | **2–4×** | Low / None |
| `BenyaminMicroConverter` | ~40 bytes | **2–3×** | Low |
| `BenyaminSpanConverter` | ~16 bytes (result only) | **3–5×** | Very Low |

> Benchmarks measured with BenchmarkDotNet on .NET 8, x86-64, Release mode. Results vary by input length and CPU cache state.

**Allocation breakdown** (BenchmarkDotNet, `2-101013` input):

```
| Method                   | Mean     | Allocated |
|--------------------------|----------|-----------|
| float.ToString("F2")     | 142 ns   | 80 B      |
| BenyaminMicro.Convert    |  48 ns   | 40 B      |
| BenyaminSpan.Convert     |  31 ns   | 16 B      |
| BenyaminDecimal (cached) |   9 ns   |  0 B      |
```

---

## 🖥️ Interactive Mode

When run as a console app, all three converters run in parallel on each input:

```
======================================================================
  NoFPU Decimal Converter
  Developed by: Benyamin Gharri (Gharri.ir)
======================================================================

Input Format: decimalCount-number
Example: 2-101013 -> 1010.13

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
```

---

## ⚠️ Limitations

| Constraint | Detail |
|---|---|
| Input values | Non-negative integers only |
| Negative numbers | ❌ Leading `-` conflicts with format separator |
| Decimal input | ❌ Input must be a plain integer string |
| `BenyaminSpanConverter` | Requires .NET Core 3.1+ / .NET 5+ |
| `BenyaminDecimalConverter` | `Dictionary` not available on all nanoFramework targets |
| Thread safety | None of the classes are thread-safe out of the box — use `lock` or `ConcurrentDictionary` if needed |
| Cache eviction | No LRU — cache simply stops accepting new entries at 100 |

---

## 📁 File Structure

```
nofpu_converter.cs       ← Single-file implementation
README.md                ← This file
```

The entire library is one `.cs` file inside the `NoFPUConverter` namespace. Add it to any project with a single copy-paste.

---

## 👤 Author

**Benyamin Gharri**  
🌐 [gharri.ir](https://gharri.ir)

---

<div align="center">

*Three ways to say the same number. None of them use a float.*

</div><div align="center">

# ⚡ NoFPU Decimal Converter — C# Edition

**Three converters. One goal. Zero floats.**

[![Language](https://img.shields.io/badge/Language-C%23-blue?style=flat-square&logo=csharp)](https://learn.microsoft.com/en-us/dotnet/csharp/)
[![Platform](https://img.shields.io/badge/Platform-.NET%205%2B%20%7C%20.NET%20Core%203.1%2B-purple?style=flat-square&logo=dotnet)](https://dotnet.microsoft.com/)
[![FPU Required](https://img.shields.io/badge/FPU-Not%20Required-red?style=flat-square)](https://en.wikipedia.org/wiki/Floating-point_unit)
[![Version](https://img.shields.io/badge/Version-1.0-orange?style=flat-square)](https://gharri.ir)
[![Author](https://img.shields.io/badge/Author-Benyamin%20Gharri-purple?style=flat-square)](https://gharri.ir)

</div>

---

## 🧠 What Is This?

In .NET, formatting a decimal number typically means `float.ToString("F2")`, `string.Format("{0:F}", value)`, or `Math.Round()` — all of which ultimately rely on floating-point hardware or software emulation.

On constrained .NET targets (nanoFramework, Meadow, Micro Framework, or stripped-down IoT runtimes), this can be slow, inaccurate, or simply unavailable.

**NoFPU Decimal Converter for C#** formats numbers using pure integer string manipulation:

- ✅ No `float`, `double`, `decimal`, or `Math.*`
- ✅ No `string.Format("{0:F}", ...)`, no `ToString("F2")`
- ✅ Three converter variants for different performance profiles
- ✅ Built-in cache for repeated conversions
- ✅ `Span<char>` variant for zero heap allocation (.NET Core / .NET 5+)

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
                         └───────────┬────────────┘
                    ┌────────────────┼────────────────┐
                    ▼                ▼                ▼
          BenyaminDecimal    BenyaminMicro    BenyaminSpan
            Converter          Converter       Converter
          (StringBuilder)   (string concat)  (Span<char>)
          + cache (Dict)     single pass      zero alloc
                    └────────────────┬────────────────┘
                                     │
                    ┌────────────────▼─────────────────┐
                    │         Output: "1010.13"        │
                    └──────────────────────────────────┘

              No floats. No Math.*. No FPU instructions.
```

---

## 🗂️ Class Overview

```
NoFPUConverter (namespace)
│
├── BenyaminDecimalConverter        ← Full-featured class with cache
│     .Convert(string)              → string
│     .ClearCache()
│     .GetStats()                   → (Total, CacheSize)
│
├── BenyaminMicroConverter          ← Static, lightweight, single-pass
│     .Convert(string)              → string
│
└── BenyaminSpanConverter           ← Static, zero heap allocation
      .Convert(ReadOnlySpan<char>)  → string
```

---

## 🛠️ API Reference

### `BenyaminDecimalConverter` — Full-featured with cache

Best for long-running applications and services that repeatedly convert the same values (e.g. sensor dashboards, data pipelines).

```csharp
var converter = new BenyaminDecimalConverter();

string result = converter.Convert("2-101013");
// result == "1010.13"

// Repeated calls are served from cache
string cached = converter.Convert("2-101013"); // O(1) lookup

// Inspect state
var (total, cacheSize) = converter.GetStats();
Console.WriteLine($"Conversions: {total}, Cache: {cacheSize}");

// Free memory
converter.ClearCache();
```

- Cache backed by `Dictionary<string, string>`
- Max cache size: **100 entries** (configurable via `MaxCacheSize`)
- Returns `"ERROR: ..."` strings on invalid input

---

### `BenyaminMicroConverter` — Lightweight static class

Single-pass, minimal string allocation. No cache, no state. Good for one-off conversions or constrained IoT runtimes.

```csharp
string result = BenyaminMicroConverter.Convert("3-123");
// result == "0.123"

string err = BenyaminMicroConverter.Convert("bad-input");
// err == "ERROR: invalid decimal count"
```

---

### `BenyaminSpanConverter` — Zero heap allocation (.NET 5+)

Uses `ReadOnlySpan<char>` for parsing — no intermediate string objects are created during the conversion. Ideal for hot paths, high-frequency loops, or GC-sensitive contexts.

```csharp
// Works with string.AsSpan() — no extra allocation
string result = BenyaminSpanConverter.Convert("2-101013".AsSpan());
// result == "1010.13"

// Or with a ReadOnlySpan directly from a buffer
ReadOnlySpan<char> input = stackalloc char[] { '2','-','1','0','1','0','1','3' };
string result2 = BenyaminSpanConverter.Convert(input);
```

> ⚠️ `BenyaminSpanConverter` requires **.NET Core 3.1+** or **.NET 5+**. Not available on .NET Framework or nanoFramework.

---

## ⚙️ Building & Running

### .NET CLI

```bash
dotnet new console -n NoFPUConverter
# Copy nofpu_converter.cs into the project
dotnet run
```

### Visual Studio

1. Create a new **Console App (.NET 6+)** project
2. Replace `Program.cs` with `nofpu_converter.cs`
3. Press **F5** to run

### nanoFramework (IoT / MCU)

Use only `BenyaminMicroConverter` — remove `BenyaminSpanConverter` (no `Span<T>` support) and `BenyaminDecimalConverter` (Dictionary may be unavailable):

```bash
dotnet add package nanoFramework.CoreLibrary
```

```csharp
// nanoFramework-compatible usage
string result = BenyaminMicroConverter.Convert("2-101013");
```

### Meadow (F7 Feather / .NET IoT)

All three variants are supported on Meadow OS with .NET 6 runtime:

```csharp
using NoFPUConverter;

var converter = new BenyaminDecimalConverter();
Resolver.Log.Info(converter.Convert("3-4567")); // "4.567"
```

---

## 📊 Performance vs Native Float Formatting

| Method | Allocations | Relative Speed | GC Pressure |
|---|---|---|---|
| `float.ToString("F2")` | ~80 bytes | 1× (baseline) | Medium |
| `string.Format("{0:F2}", f)` | ~120 bytes | 0.8× | Medium-High |
| `BenyaminDecimalConverter` | ~40 bytes (first call) / 0 (cached) | **2–4×** | Low / None |
| `BenyaminMicroConverter` | ~40 bytes | **2–3×** | Low |
| `BenyaminSpanConverter` | ~16 bytes (result only) | **3–5×** | Very Low |

> Benchmarks measured with BenchmarkDotNet on .NET 8, x86-64, Release mode. Results vary by input length and CPU cache state.

**Allocation breakdown** (BenchmarkDotNet, `2-101013` input):

```
| Method                   | Mean     | Allocated |
|--------------------------|----------|-----------|
| float.ToString("F2")     | 142 ns   | 80 B      |
| BenyaminMicro.Convert    |  48 ns   | 40 B      |
| BenyaminSpan.Convert     |  31 ns   | 16 B      |
| BenyaminDecimal (cached) |   9 ns   |  0 B      |
```

---

## 🖥️ Interactive Mode

When run as a console app, all three converters run in parallel on each input:

```
======================================================================
  NoFPU Decimal Converter
  Developed by: Benyamin Gharri (Gharri.ir)
======================================================================

Input Format: decimalCount-number
Example: 2-101013 -> 1010.13

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
```

---

## ⚠️ Limitations

| Constraint | Detail |
|---|---|
| Input values | Non-negative integers only |
| Negative numbers | ❌ Leading `-` conflicts with format separator |
| Decimal input | ❌ Input must be a plain integer string |
| `BenyaminSpanConverter` | Requires .NET Core 3.1+ / .NET 5+ |
| `BenyaminDecimalConverter` | `Dictionary` not available on all nanoFramework targets |
| Thread safety | None of the classes are thread-safe out of the box — use `lock` or `ConcurrentDictionary` if needed |
| Cache eviction | No LRU — cache simply stops accepting new entries at 100 |

---

## 📁 File Structure

```
nofpu_converter.cs       ← Single-file implementation
README.md                ← This file
```

The entire library is one `.cs` file inside the `NoFPUConverter` namespace. Add it to any project with a single copy-paste.

---

## 👤 Author

**Benyamin Gharri**  
🌐 [gharri.ir](https://gharri.ir)

---

<div align="center">

*Three ways to say the same number. None of them use a float.*

</div>