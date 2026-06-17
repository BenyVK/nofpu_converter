//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

/*
 * NoFPU Decimal Converter
 * Developed by: Benyamin Gharri (Gharri.ir)
 * Version: 1.0
 *
 * This converter performs decimal conversion without using Floating Point Unit (FPU)
 * Ideal for embedded systems, microcontrollers, and systems without FPU support
 *
 * Java Port for JDK 25 with modern features
 */

import java.util.*;

// ============================================================
// Main converter class with caching capability
// ============================================================
class BenyaminDecimalConverter {
    private final Map<String, String> cache = new HashMap<>();
    private int totalConversions = 0;
    private final int MAX_CACHE_SIZE = 100;

    /**
     * Convert input to decimal format without using float/FPU
     * Format: decimalCount-number (example: 2-101013) -> "1010.13"
     */
    public String convert(String input) {
        // Check cache using Optional
        var cached = Optional.ofNullable(cache.get(input));
        if (cached.isPresent()) {
            return cached.get();
        }

        // Perform conversion
        var result = convertInternal(input);

        // Store in cache if valid
        if (!result.startsWith("ERROR") && cache.size() < MAX_CACHE_SIZE) {
            cache.put(input, result);
        }

        totalConversions++;
        return result;
    }

    /**
     * Core conversion engine - minimal CPU and memory usage
     * Uses pattern matching
     */
    private String convertInternal(String input) {
        // Remove whitespace
        var clean = input.replaceAll("\\s+", "");

        // Find position of '-'
        var dashPos = clean.indexOf('-');
        if (dashPos == -1) {
            return "ERROR: '-' not found";
        }

        // Extract parts
        var decStr = clean.substring(0, dashPos);
        var numStr = clean.substring(dashPos + 1);

        // Validate decimal count
        if (decStr.isEmpty()) {
            return "ERROR: decimal count is empty";
        }

        // Convert to integer with try-catch
        int decCount;
        try {
            decCount = Integer.parseInt(decStr);
        } catch (NumberFormatException e) {
            return "ERROR: invalid decimal count";
        }

        if (numStr.isEmpty()) {
            return "ERROR: number part is empty";
        }

        // Use switch expression for decision
        return switch (decCount) {
            case 0 -> numStr;
            default -> {
                var paddedNum = padWithZeros(numStr, decCount);
                yield formatResult(paddedNum, decCount);
            }
        };
    }

    /**
     * Add leading zeros if needed
     */
    private String padWithZeros(String numStr, int decCount) {
        if (decCount > numStr.length()) {
            var padding = decCount - numStr.length();
            return "0".repeat(padding) + numStr;
        }
        return numStr;
    }

    /**
     * Format the final result
     */
    private String formatResult(String numStr, int decCount) {
        if (decCount >= numStr.length()) {
            return "0." + numStr;
        } else {
            var splitPos = numStr.length() - decCount;
            return numStr.substring(0, splitPos) + "." + numStr.substring(splitPos);
        }
    }

    /**
     * Clear the cache
     */
    public void clearCache() {
        cache.clear();
    }

    /**
     * Get conversion statistics
     */
    public Map<String, Object> getStats() {
        return Map.of(
                "totalConversions", totalConversions,
                "cacheSize", cache.size()
        );
    }
}

// ============================================================
// Ultra-lightweight version for microcontrollers
// ============================================================
class BenyaminMicroConverter {
    /**
     * Convert with single-pass string traversal - minimal memory allocation
     */
    public static String convert(String input) {
        var decCount = 0;
        var numPart = new StringBuilder();
        var isDec = true;
        var foundDash = false;

        // Single-pass traversal using modern for-each
        for (var ch : input.toCharArray()) {
            switch (ch) {
                case ' ' -> {}
                case '-' -> {
                    foundDash = true;
                    isDec = false;
                }
                default -> {
                    if (isDec) {
                        // Decimal count section
                        if (Character.isDigit(ch)) {
                            decCount = decCount * 10 + (ch - '0');
                        } else {
                            return "ERROR: invalid decimal count";
                        }
                    } else {
                        // Number section
                        if (Character.isDigit(ch)) {
                            numPart.append(ch);
                        } else {
                            return "ERROR: invalid number format";
                        }
                    }
                }
            }
        }

        if (!foundDash || numPart.isEmpty()) {
            return "ERROR: invalid format";
        }

        // Use switch expression
        return switch (decCount) {
            case 0 -> numPart.toString();
            default -> {
                var numStr = padZeros(numPart.toString(), decCount);
                yield formatDecimal(numStr, decCount);
            }
        };
    }

    private static String padZeros(String numStr, int decCount) {
        if (decCount > numStr.length()) {
            var padding = decCount - numStr.length();
            return "0".repeat(padding) + numStr;
        }
        return numStr;
    }

    private static String formatDecimal(String numStr, int decCount) {
        if (decCount >= numStr.length()) {
            return "0." + numStr;
        } else {
            var splitPos = numStr.length() - decCount;
            return numStr.substring(0, splitPos) + "." + numStr.substring(splitPos);
        }
    }
}

// ============================================================
// High-performance version using Stream API
// ============================================================
class BenyaminSpanConverter {
    /**
     * Convert using Stream API and modern Java methods
     */
    public static String convert(String input) {
        // Find position of '-'
        var dashPos = input.indexOf('-');
        if (dashPos == -1) {
            return "ERROR: '-' not found";
        }

        // Extract parts
        var decPart = input.substring(0, dashPos);
        var numPart = input.substring(dashPos + 1);

        // Calculate decimal count using Stream
        var decCount = decPart.chars()
                .filter(ch -> ch != ' ')
                .map(ch -> ch - '0')
                .reduce(0, (acc, digit) -> acc * 10 + digit);

        if (numPart.isEmpty()) {
            return "ERROR: number part is empty";
        }

        // Remove whitespace from number part
        var numStr = numPart.replaceAll("\\s+", "");

        // Use switch expression
        return switch (decCount) {
            case 0 -> numStr;
            default -> {
                var padded = padZeros(numStr, decCount);
                yield formatDecimal(padded, decCount);
            }
        };
    }

    private static String padZeros(String numStr, int decCount) {
        if (decCount > numStr.length()) {
            var padding = decCount - numStr.length();
            return "0".repeat(padding) + numStr;
        }
        return numStr;
    }

    private static String formatDecimal(String numStr, int decCount) {
        if (decCount >= numStr.length()) {
            return "0." + numStr;
        } else {
            var splitPos = numStr.length() - decCount;
            return numStr.substring(0, splitPos) + "." + numStr.substring(splitPos);
        }
    }
}

// ============================================================
// Main program using void main() and IO (JDK 25)
// ============================================================
void main() {
    // Display header using IO and Text Blocks
    IO.println("""
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

Input Format: decimalCount-number
Example: 2-101013 -> 1010.13
Type 'exit' to quit
""");

    // Create converter instance
    var converter = new BenyaminDecimalConverter();
    var scanner = new Scanner(System.in);

    // Main program loop
    do {
        IO.print("Input: ");
        var input = scanner.nextLine().trim();

        if (input.isEmpty()) {
            IO.println("Please enter a value\n");
            continue;
        }

        // Exit program
        if (input.equalsIgnoreCase("exit")) {
            var stats = converter.getStats();
            IO.println("\nStatistics:");
            IO.println("  Total Conversions: " + stats.get("totalConversions"));
            IO.println("  Cache Size: " + stats.get("cacheSize"));
            IO.println("\nThank you for using NoFPU Decimal Converter!");
            IO.println("Developed by: Benyamin Gharri (Gharri.ir)");
            break;
        }

        // Test all three methods
        IO.println("\n--- Results ---");

        // Method 1: Standard converter
        var result1 = converter.convert(input);
        IO.println("Standard Converter: " + result1);

        // Method 2: Micro converter
        var result2 = BenyaminMicroConverter.convert(input);
        IO.println("Micro Converter:   " + result2);

        // Method 3: Span converter
        var result3 = BenyaminSpanConverter.convert(input);
        IO.println("Span Converter:    " + result3);

        IO.println();

    } while (true);

    scanner.close();
}