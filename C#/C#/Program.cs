/*
 * NoFPU Decimal Converter
 * Developed by: Benyamin Gharri (Gharri.ir)
 * Version: 1.0
 * 
 * This converter performs decimal conversion without using Floating Point Unit (FPU)
 * Ideal for embedded systems, microcontrollers, and systems without FPU support
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace NoFPUConverter
{
    /// <summary>
    /// NoFPU Decimal Converter
    /// Developed by: Benyamin Gharri (Gharri.ir)
    /// </summary>
    public class BenyaminDecimalConverter
    {
        private Dictionary<string, string> _cache = new Dictionary<string, string>();
        private int _totalConversions = 0;
        private const int MaxCacheSize = 100;

        /// <summary>
        /// Convert input string to decimal format without using float/FPU
        /// </summary>
        /// <param name="input">Format: decimalCount-number (e.g., 2-101013)</param>
        /// <returns>Formatted decimal string</returns>
        public string Convert(string input)
        {
            // Check cache first
            if (_cache.ContainsKey(input))
                return _cache[input];

            // Perform conversion
            string result = ConvertInternal(input);

            // Store in cache if valid and not full
            if (result != "ERROR" && _cache.Count < MaxCacheSize)
                _cache[input] = result;

            _totalConversions++;
            return result;
        }

        /// <summary>
        /// Core conversion engine - minimal CPU and memory usage
        /// </summary>
        private string ConvertInternal(string input)
        {
            // Remove spaces using StringBuilder (more efficient than string)
            StringBuilder cleaned = new StringBuilder();
            foreach (char ch in input)
            {
                if (ch != ' ')
                    cleaned.Append(ch);
            }
            string s = cleaned.ToString();

            // Find '-' position with simple loop
            int dashPos = -1;
            for (int i = 0; i < s.Length; i++)
            {
                if (s[i] == '-')
                {
                    dashPos = i;
                    break;
                }
            }

            if (dashPos == -1)
                return "ERROR: '-' not found";

            // Extract parts using substring
            string decStr = s.Substring(0, dashPos);
            string numStr = s.Substring(dashPos + 1);

            // Validate decimal count
            if (string.IsNullOrEmpty(decStr))
                return "ERROR: decimal count is empty";

            // Manual conversion to integer (avoid int.Parse to save CPU)
            int decCount = 0;
            foreach (char ch in decStr)
            {
                if (ch >= '0' && ch <= '9')
                    decCount = decCount * 10 + (ch - '0');
                else
                    return "ERROR: invalid decimal count";
            }

            if (string.IsNullOrEmpty(numStr))
                return "ERROR: number part is empty";

            // If decimal count is zero, return as is
            if (decCount == 0)
                return numStr;

            // Padding with leading zeros if needed
            if (decCount > numStr.Length)
            {
                StringBuilder padded = new StringBuilder();
                padded.Append('0', decCount - numStr.Length);
                padded.Append(numStr);
                numStr = padded.ToString();
            }

            // Split integer and decimal parts
            if (decCount >= numStr.Length)
            {
                return "0." + numStr;
            }
            else
            {
                int splitPos = numStr.Length - decCount;
                return numStr.Substring(0, splitPos) + "." + numStr.Substring(splitPos);
            }
        }

        /// <summary>
        /// Clear the conversion cache
        /// </summary>
        public void ClearCache()
        {
            _cache.Clear();
        }

        /// <summary>
        /// Get conversion statistics
        /// </summary>
        public (int Total, int CacheSize) GetStats()
        {
            return (_totalConversions, _cache.Count);
        }
    }

    /// <summary>
    /// Ultra-lightweight version for microcontrollers
    /// Developed by: Benyamin Gharri (Gharri.ir)
    /// </summary>
    public static class BenyaminMicroConverter
    {
        /// <summary>
        /// Convert with single pass through string - minimal memory allocation
        /// </summary>
        public static string Convert(string input)
        {
            int decCount = 0;
            string numPart = "";
            bool isDec = true;
            bool foundDash = false;

            // Single pass through the string
            foreach (char ch in input)
            {
                if (ch == ' ')
                    continue;
                else if (ch == '-')
                {
                    foundDash = true;
                    isDec = false;
                }
                else if (isDec)
                {
                    // Decimal count part
                    if (ch >= '0' && ch <= '9')
                        decCount = decCount * 10 + (ch - '0');
                    else
                        return "ERROR: invalid decimal count";
                }
                else
                {
                    // Number part
                    if (ch >= '0' && ch <= '9')
                        numPart += ch;
                    else
                        return "ERROR: invalid number format";
                }
            }

            if (!foundDash || string.IsNullOrEmpty(numPart))
                return "ERROR: invalid format";

            // If decimal count is zero
            if (decCount == 0)
                return numPart;

            // Padding with zeros
            if (decCount > numPart.Length)
            {
                numPart = new string('0', decCount - numPart.Length) + numPart;
            }

            // Build final result
            if (decCount >= numPart.Length)
            {
                return "0." + numPart;
            }
            else
            {
                int splitPos = numPart.Length - decCount;
                return numPart.Substring(0, splitPos) + "." + numPart.Substring(splitPos);
            }
        }
    }

    /// <summary>
    /// High-performance version using Span for .NET Core
    /// Developed by: Benyamin Gharri (Gharri.ir)
    /// </summary>
    public static class BenyaminSpanConverter
    {
        /// <summary>
        /// Convert using Span for zero memory allocation
        /// </summary>
        public static string Convert(ReadOnlySpan<char> input)
        {
            // Find '-' position
            int dashPos = input.IndexOf('-');
            if (dashPos == -1)
                return "ERROR: '-' not found";

            // Extract parts using Span (no memory allocation)
            ReadOnlySpan<char> decSpan = input.Slice(0, dashPos);
            ReadOnlySpan<char> numSpan = input.Slice(dashPos + 1);

            // Calculate decimal count
            int decCount = 0;
            foreach (char ch in decSpan)
            {
                if (ch >= '0' && ch <= '9')
                    decCount = decCount * 10 + (ch - '0');
                else if (ch != ' ')
                    return "ERROR: invalid decimal count";
            }

            if (numSpan.IsEmpty)
                return "ERROR: number part is empty";

            // Remove spaces from numSpan
            string numStr = numSpan.ToString().Replace(" ", "");

            // If decimal count is zero
            if (decCount == 0)
                return numStr;

            // Padding and build result
            if (decCount > numStr.Length)
            {
                numStr = new string('0', decCount - numStr.Length) + numStr;
            }

            if (decCount >= numStr.Length)
                return "0." + numStr;
            else
            {
                int splitPos = numStr.Length - decCount;
                return numStr.Substring(0, splitPos) + "." + numStr.Substring(splitPos);
            }
        }
    }

    /// <summary>
    /// Main Program
    /// Developed by: Benyamin Gharri (Gharri.ir)
    /// </summary>
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("=".PadRight(70, '='));
            Console.WriteLine("  NoFPU Decimal Converter");
            Console.WriteLine("  Developed by: Benyamin Gharri (Gharri.ir)");
            Console.WriteLine("=".PadRight(70, '='));
            Console.WriteLine("\nFeatures:");
            Console.WriteLine("  ✓ No floating point operations");
            Console.WriteLine("  ✓ No FPU required");
            Console.WriteLine("  ✓ Minimal CPU usage");
            Console.WriteLine("  ✓ Minimal memory allocation");
            Console.WriteLine("  ✓ Perfect for embedded systems");
            Console.WriteLine("\nInput Format: decimalCount-number");
            Console.WriteLine("Example: 2-101013 -> 1010.13");
            Console.WriteLine("Type 'exit' to quit\n");

            // Create converter instance
            var converter = new BenyaminDecimalConverter();

            while (true)
            {
                Console.Write("Input: ");
                string input = Console.ReadLine()?.Trim();

                if (string.IsNullOrEmpty(input))
                {
                    Console.WriteLine("Please enter a value\n");
                    continue;
                }

                if (input.ToLower() == "exit")
                {
                    var stats = converter.GetStats();
                    Console.WriteLine("\nStatistics:");
                    Console.WriteLine($"  Total Conversions: {stats.Total}");
                    Console.WriteLine($"  Cache Size: {stats.CacheSize}");
                    Console.WriteLine("\nThank you for using NoFPU Decimal Converter!");
                    Console.WriteLine("Developed by: Benyamin Gharri (Gharri.ir)");
                    break;
                }

                // Try all three converters
                Console.WriteLine("\n--- Results ---");

                // Method 1: Standard converter
                string result1 = converter.Convert(input);
                Console.WriteLine($"Standard Converter: {result1}");

                // Method 2: Micro converter
                string result2 = BenyaminMicroConverter.Convert(input);
                Console.WriteLine($"Micro Converter:   {result2}");

                // Method 3: Span converter (if available)
                string result3 = BenyaminSpanConverter.Convert(input.AsSpan());
                Console.WriteLine($"Span Converter:    {result3}");

                Console.WriteLine();
            }
        }
    }
}