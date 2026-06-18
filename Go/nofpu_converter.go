package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// ============================================================
// NoFPU Decimal Converter
// Developed by: Benyamin Gharri (Gharri.ir)
// Version: 1.0
// Go Port
// 
// This converter performs decimal conversion without using 
// Floating Point Unit (FPU)
// Ideal for embedded systems, microcontrollers, and systems 
// without FPU support
// ============================================================

// convertNoFPU converts string to decimal format without using FPU
// - No float
// - No floating point operations
// - Only string and integer operations
func convertNoFPU(inputString string) (string, string) {
	// Remove spaces
	s := ""
	for _, ch := range inputString {
		if ch != ' ' {
			s += string(ch)
		}
	}

	// Find '-' with simple loop
	dashPos := -1
	for i, ch := range s {
		if ch == '-' {
			dashPos = i
			break
		}
	}

	if dashPos == -1 {
		return "", "ERROR: '-' not found"
	}

	// Extract parts
	decStr := s[:dashPos]
	numStr := s[dashPos+1:]

	// Validation
	if decStr == "" {
		return "", "ERROR: decimal count is empty"
	}

	// Manual conversion of decimal count to integer (without strconv.Atoi)
	decCount := 0
	for _, ch := range decStr {
		if ch >= '0' && ch <= '9' {
			decCount = decCount*10 + int(ch-'0')
		} else {
			return "", "ERROR: invalid decimal count"
		}
	}

	if numStr == "" {
		return "", "ERROR: number part is empty"
	}

	// If decimal count is zero
	if decCount == 0 {
		return numStr, ""
	}

	// Padding if needed
	if decCount > len(numStr) {
		zerosNeeded := decCount - len(numStr)
		numStr = strings.Repeat("0", zerosNeeded) + numStr
	}

	// Separate integer and decimal parts
	var result string
	if decCount >= len(numStr) {
		result = "0." + numStr
	} else {
		splitPos := len(numStr) - decCount
		result = numStr[:splitPos] + "." + numStr[splitPos:]
	}

	return result, ""
}

// convertMicro is the ultra-lightweight version for microcontrollers
func convertMicro(inputString string) string {
	var result []rune
	decCount := 0
	numPart := ""
	isDec := true
	foundDash := false

	// Single pass through string (no extra memory allocation)
	for _, ch := range inputString {
		if ch == ' ' {
			continue
		} else if ch == '-' {
			foundDash = true
			isDec = false
		} else if isDec {
			if ch >= '0' && ch <= '9' {
				decCount = decCount*10 + int(ch-'0')
			} else {
				return "ERROR"
			}
		} else {
			if ch >= '0' && ch <= '9' {
				numPart += string(ch)
			} else {
				return "ERROR"
			}
		}
	}

	if !foundDash || numPart == "" {
		return "ERROR"
	}

	if decCount == 0 {
		return numPart
	}

	if decCount > len(numPart) {
		numPart = strings.Repeat("0", decCount-len(numPart)) + numPart
	}

	var resultStr string
	if decCount >= len(numPart) {
		resultStr = "0." + numPart
	} else {
		splitPos := len(numPart) - decCount
		resultStr = numPart[:splitPos] + "." + numPart[splitPos:]
	}

	_ = result // just to avoid unused variable warning
	return resultStr
}

// NoFPUConverter is the management class for systems without FPU
type NoFPUConverter struct {
	cache            map[string]string
	totalConversions int
}

// NewNoFPUConverter creates a new converter instance
func NewNoFPUConverter() *NoFPUConverter {
	return &NoFPUConverter{
		cache:            make(map[string]string),
		totalConversions: 0,
	}
}

// Convert performs conversion with result caching
func (c *NoFPUConverter) Convert(inputStr string) string {
	// Check cache
	if val, exists := c.cache[inputStr]; exists {
		return val
	}

	// Convert using micro version
	result := convertMicro(inputStr)

	// Cache if not error and cache size < 100
	if result != "ERROR" && len(c.cache) < 100 {
		c.cache[inputStr] = result
	}

	c.totalConversions++
	return result
}

// ClearCache clears cache to free memory
func (c *NoFPUConverter) ClearCache() {
	c.cache = make(map[string]string)
}

// GetStats gets conversion statistics
func (c *NoFPUConverter) GetStats() (total int, cacheSize int) {
	return c.totalConversions, len(c.cache)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)

	fmt.Println(strings.Repeat("=", 60))
	fmt.Println("NoFPU Decimal Converter - Suitable for embedded systems")
	fmt.Println(strings.Repeat("=", 60))
	fmt.Println("\nFeatures:")
	fmt.Println("✓ No float usage")
	fmt.Println("✓ No floating point operations")
	fmt.Println("✓ Minimal CPU usage")
	fmt.Println("✓ Minimal memory usage")
	fmt.Println("✓ Suitable for systems without FPU")
	fmt.Println("\nInput format: decimal_count-number")
	fmt.Println("Example: 2-101013 -> 1010.13")
	fmt.Println("Enter 'exit' to quit\n")

	converter := NewNoFPUConverter()

	for {
		fmt.Print("Input: ")
		scanner.Scan()
		userInput := strings.TrimSpace(scanner.Text())

		if strings.ToLower(userInput) == "exit" {
			fmt.Println("\nStatistics:")
			total, cacheSize := converter.GetStats()
			fmt.Printf("Total conversions: %d\n", total)
			fmt.Printf("Cache size: %d\n", cacheSize)
			fmt.Println("Exiting program")
			break
		}

		if userInput == "" {
			fmt.Println("Please enter a value\n")
			continue
		}

		result := converter.Convert(userInput)

		if result == "ERROR" {
			fmt.Println("❌ Error: Invalid input format")
			fmt.Println("Correct format: decimal_count-number (Example: 2-101013)\n")
		} else {
			fmt.Printf("✅ Result: %s\n", result)
			fmt.Println("   (Without using FPU)\n")
		}
	}
}