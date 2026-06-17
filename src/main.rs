// ============================================================
// NoFPU Decimal Converter
// Developed by: Benyamin Gharri (Gharri.ir)
// Version: 1.0
// Rust Port
// 
// This converter performs decimal conversion without using 
// Floating Point Unit (FPU)
// Ideal for embedded systems, microcontrollers, and systems 
// without FPU support
// ============================================================

use std::collections::HashMap;
use std::io::{self, Write};

// ============================================================
// Main Converter Class with Caching Capability
// ============================================================
pub struct BenyaminDecimalConverter {
    cache: HashMap<String, String>,
    total_conversions: u32,
    max_cache_size: usize,
}

impl BenyaminDecimalConverter {
    /// Create a new converter instance
    pub fn new() -> Self {
        BenyaminDecimalConverter {
            cache: HashMap::new(),
            total_conversions: 0,
            max_cache_size: 100,
        }
    }

    /// Convert input to decimal format without using float/FPU
    /// Format: decimalCount-number (example: 2-101013) -> "1010.13"
    pub fn convert(&mut self, input: &str) -> String {
        // Check cache
        if let Some(cached) = self.cache.get(input) {
            return cached.clone();
        }

        // Perform conversion
        let result = self.convert_internal(input);

        // Store in cache if valid
        if result != "ERROR" && self.cache.len() < self.max_cache_size {
            self.cache.insert(input.to_string(), result.clone());
        }

        self.total_conversions += 1;
        result
    }

    /// Main conversion engine - minimal CPU and memory usage
    fn convert_internal(&self, input: &str) -> String {
        // Remove whitespace
        let s: String = input.chars().filter(|&c| c != ' ').collect();

        // Find position of '-'
        let dash_pos = match s.find('-') {
            Some(pos) => pos,
            None => return "ERROR: '-' not found".to_string(),
        };

        // Extract parts
        let dec_str = &s[0..dash_pos];
        let num_str = &s[dash_pos + 1..];

        // Validate decimal count
        if dec_str.is_empty() {
            return "ERROR: decimal count is empty".to_string();
        }

        // Convert to integer
        let dec_count = match dec_str.parse::<usize>() {
            Ok(count) => count,
            Err(_) => return "ERROR: invalid decimal count".to_string(),
        };

        if num_str.is_empty() {
            return "ERROR: number part is empty".to_string();
        }

        // If decimal count is zero, return number as is
        if dec_count == 0 {
            return num_str.to_string();
        }

        let mut num_str = num_str.to_string();

        // Add leading zeros if needed
        if dec_count > num_str.len() {
            let padding = dec_count - num_str.len();
            num_str = format!("{}{}", "0".repeat(padding), num_str);
        }

        // Split integer and decimal parts
        if dec_count >= num_str.len() {
            format!("0.{}", num_str)
        } else {
            let split_pos = num_str.len() - dec_count;
            format!("{}.{}", &num_str[0..split_pos], &num_str[split_pos..])
        }
    }

    /// Clear the cache
    pub fn clear_cache(&mut self) {
        self.cache.clear();
    }

    /// Get conversion statistics
    pub fn get_stats(&self) -> (u32, usize) {
        (self.total_conversions, self.cache.len())
    }
}

impl Default for BenyaminDecimalConverter {
    fn default() -> Self {
        Self::new()
    }
}

// ============================================================
// Ultra-Lightweight Version for Microcontrollers
// ============================================================
pub struct BenyaminMicroConverter;

impl BenyaminMicroConverter {
    /// Convert with single-pass string traversal - minimal memory allocation
    pub fn convert(input: &str) -> String {
        let mut dec_count = 0;
        let mut num_part = String::new();
        let mut is_dec = true;
        let mut found_dash = false;

        // Single-pass traversal
        for ch in input.chars() {
            if ch == ' ' {
                continue;
            } else if ch == '-' {
                found_dash = true;
                is_dec = false;
            } else if is_dec {
                // Decimal count section
                if ch.is_ascii_digit() {
                    dec_count = dec_count * 10 + (ch as u32 - '0' as u32);
                } else {
                    return "ERROR: invalid decimal count".to_string();
                }
            } else {
                // Number section
                if ch.is_ascii_digit() {
                    num_part.push(ch);
                } else {
                    return "ERROR: invalid number format".to_string();
                }
            }
        }

        if !found_dash || num_part.is_empty() {
            return "ERROR: invalid format".to_string();
        }

        // If decimal count is zero
        if dec_count == 0 {
            return num_part;
        }

        let dec_count = dec_count as usize;

        // Add leading zeros
        let num_part = if dec_count > num_part.len() {
            let padding = dec_count - num_part.len();
            format!("{}{}", "0".repeat(padding), num_part)
        } else {
            num_part
        };

        // Build final result
        if dec_count >= num_part.len() {
            format!("0.{}", num_part)
        } else {
            let split_pos = num_part.len() - dec_count;
            format!("{}.{}", &num_part[0..split_pos], &num_part[split_pos..])
        }
    }
}

// ============================================================
// High-Performance Version Using String Slices (Span)
// ============================================================
pub struct BenyaminSpanConverter;

impl BenyaminSpanConverter {
    /// Convert using string slices - minimal memory allocation
    pub fn convert(input: &str) -> String {
        // Find position of '-'
        let dash_pos = match input.find('-') {
            Some(pos) => pos,
            None => return "ERROR: '-' not found".to_string(),
        };

        // Extract parts using slices
        let dec_span = &input[0..dash_pos];
        let num_span = &input[dash_pos + 1..];

        // Calculate decimal count
        let mut dec_count = 0;
        for ch in dec_span.chars() {
            if ch == ' ' {
                continue;
            } else if ch.is_ascii_digit() {
                dec_count = dec_count * 10 + (ch as u32 - '0' as u32);
            } else {
                return "ERROR: invalid decimal count".to_string();
            }
        }

        if num_span.is_empty() {
            return "ERROR: number part is empty".to_string();
        }

        // Remove whitespace from number part
        let num_str: String = num_span.chars().filter(|&c| c != ' ').collect();
        let dec_count = dec_count as usize;

        // If decimal count is zero
        if dec_count == 0 {
            return num_str;
        }

        // Add leading zeros and build result
        let num_str = if dec_count > num_str.len() {
            let padding = dec_count - num_str.len();
            format!("{}{}", "0".repeat(padding), num_str)
        } else {
            num_str
        };

        if dec_count >= num_str.len() {
            format!("0.{}", num_str)
        } else {
            let split_pos = num_str.len() - dec_count;
            format!("{}.{}", &num_str[0..split_pos], &num_str[split_pos..])
        }
    }
}

// ============================================================
// Main Program
// ============================================================
fn main() {
    // Display header
    println!("{}", "=".repeat(70));
    println!("  NoFPU Decimal Converter");
    println!("  Developed by: Benyamin Gharri (Gharri.ir)");
    println!("  Rust Port");
    println!("{}", "=".repeat(70));
    println!("\nFeatures:");
    println!("  ✓ No floating point operations");
    println!("  ✓ No FPU required");
    println!("  ✓ Minimal CPU usage");
    println!("  ✓ Minimal memory allocation");
    println!("  ✓ Perfect for embedded systems");
    println!("\nInput Format: decimalCount-number");
    println!("Example: 2-101013 -> 1010.13");
    println!("Type 'exit' to quit\n");

    // Create converter instance
    let mut converter = BenyaminDecimalConverter::new();

    // Main program loop
    loop {
        print!("Input: ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        let input = input.trim();

        if input.is_empty() {
            println!("Please enter a value\n");
            continue;
        }

        if input.to_lowercase() == "exit" {
            let (total, cache_size) = converter.get_stats();
            println!("\nStatistics:");
            println!("  Total Conversions: {}", total);
            println!("  Cache Size: {}", cache_size);
            println!("\nThank you for using NoFPU Decimal Converter!");
            println!("Developed by: Benyamin Gharri (Gharri.ir)");
            break;
        }

        // Test all three methods
        println!("\n--- Results ---");
        
        // Method 1: Standard converter
        let result1 = converter.convert(input);
        println!("Standard Converter: {}", result1);
        
        // Method 2: Micro converter
        let result2 = BenyaminMicroConverter::convert(input);
        println!("Micro Converter:   {}", result2);
        
        // Method 3: Span converter
        let result3 = BenyaminSpanConverter::convert(input);
        println!("Span Converter:    {}", result3);
        
        println!();
    }
}

// ============================================================
// Unit Tests
// ============================================================
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_standard_converter() {
        let mut converter = BenyaminDecimalConverter::new();
        assert_eq!(converter.convert("2-101013"), "1010.13");
        assert_eq!(converter.convert("0-123"), "123");
        assert_eq!(converter.convert("3-45"), "0.045");
        assert_eq!(converter.convert("5-123"), "0.00123");
        assert_eq!(converter.convert("2-1"), "0.01");
    }

    #[test]
    fn test_micro_converter() {
        assert_eq!(BenyaminMicroConverter::convert("2-101013"), "1010.13");
        assert_eq!(BenyaminMicroConverter::convert("0-123"), "123");
        assert_eq!(BenyaminMicroConverter::convert("3-45"), "0.045");
        assert_eq!(BenyaminMicroConverter::convert("5-123"), "0.00123");
        assert_eq!(BenyaminMicroConverter::convert("2-1"), "0.01");
    }

    #[test]
    fn test_span_converter() {
        assert_eq!(BenyaminSpanConverter::convert("2-101013"), "1010.13");
        assert_eq!(BenyaminSpanConverter::convert("0-123"), "123");
        assert_eq!(BenyaminSpanConverter::convert("3-45"), "0.045");
        assert_eq!(BenyaminSpanConverter::convert("5-123"), "0.00123");
        assert_eq!(BenyaminSpanConverter::convert("2-1"), "0.01");
    }

    #[test]
    fn test_error_handling() {
        let mut converter = BenyaminDecimalConverter::new();
        assert!(converter.convert("invalid").starts_with("ERROR"));
        assert!(converter.convert("2-").starts_with("ERROR"));
        assert!(converter.convert("-123").starts_with("ERROR"));
        assert!(converter.convert("abc-123").starts_with("ERROR"));
    }

    #[test]
    fn test_caching() {
        let mut converter = BenyaminDecimalConverter::new();
        let result1 = converter.convert("2-101013");
        let result2 = converter.convert("2-101013");
        assert_eq!(result1, result2);
        assert_eq!(converter.get_stats().1, 1);
    }

    #[test]
    fn test_with_spaces() {
        let mut converter = BenyaminDecimalConverter::new();
        assert_eq!(converter.convert("2 - 101013"), "1010.13");
        assert_eq!(BenyaminMicroConverter::convert("2 - 101013"), "1010.13");
        assert_eq!(BenyaminSpanConverter::convert("2 - 101013"), "1010.13");
        assert_eq!(converter.convert("2  -  101013"), "1010.13");
    }

    #[test]
    fn test_large_numbers() {
        let mut converter = BenyaminDecimalConverter::new();
        assert_eq!(converter.convert("3-123456789"), "123456.789");
        assert_eq!(converter.convert("6-123"), "0.000123");
        assert_eq!(converter.convert("10-12345"), "0.0000012345");
    }

    #[test]
    fn test_zero_decimal() {
        let mut converter = BenyaminDecimalConverter::new();
        assert_eq!(converter.convert("0-123456"), "123456");
        assert_eq!(converter.convert("0-1"), "1");
        assert_eq!(converter.convert("0-0"), "0");
    }
}