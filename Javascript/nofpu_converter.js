/* ============================================================
// NoFPU Decimal Converter
// Developed by: Benyamin Gharri (Gharri.ir)
// Version: 1.0
// JavaScript/Node.js Port
// 
// This converter performs decimal conversion without using 
// Floating Point Unit (FPU)
// Ideal for embedded systems, microcontrollers, and systems 
// without FPU support
// ============================================================ */

/**
 * Convert to decimal format without using FPU
 * Suitable for systems without Floating Point Unit
 * 
 * @param {string} inputString - Input string (format: decimal_count-number)
 * @returns {Object} - { result: string, error: string|null }
 */
function convertNoFPU(inputString) {
    // Remove spaces
    let s = '';
    for (let ch of inputString) {
        if (ch !== ' ') {
            s += ch;
        }
    }
    
    // Find '-' with simple loop
    let dashPos = -1;
    for (let i = 0; i < s.length; i++) {
        if (s[i] === '-') {
            dashPos = i;
            break;
        }
    }
    
    if (dashPos === -1) {
        return { result: null, error: "ERROR: '-' not found" };
    }
    
    // Extract parts
    let decStr = s.substring(0, dashPos);
    let numStr = s.substring(dashPos + 1);
    
    // Validation
    if (decStr.length === 0) {
        return { result: null, error: "ERROR: decimal count is empty" };
    }
    
    // Manual conversion of decimal count to integer (without parseInt)
    let decCount = 0;
    for (let ch of decStr) {
        if (ch >= '0' && ch <= '9') {
            decCount = decCount * 10 + (ch.charCodeAt(0) - 48);
        } else {
            return { result: null, error: "ERROR: invalid decimal count" };
        }
    }
    
    if (numStr.length === 0) {
        return { result: null, error: "ERROR: number part is empty" };
    }
    
    // If decimal count is zero
    if (decCount === 0) {
        return { result: numStr, error: null };
    }
    
    // Padding if needed
    if (decCount > numStr.length) {
        let zerosNeeded = decCount - numStr.length;
        numStr = '0'.repeat(zerosNeeded) + numStr;
    }
    
    // Separate integer and decimal parts
    let result;
    if (decCount >= numStr.length) {
        // Number is less than 1
        result = "0." + numStr;
    } else {
        let splitPos = numStr.length - decCount;
        result = numStr.substring(0, splitPos) + "." + numStr.substring(splitPos);
    }
    
    return { result: result, error: null };
}

/**
 * Ultra-lightweight version for microcontrollers
 * Minimal memory usage, single pass through string
 * 
 * @param {string} inputString - Input string
 * @returns {string} - Result or "ERROR"
 */
function convertMicro(inputString) {
    let numPart = '';
    let decCount = 0;
    let isDec = true;
    let foundDash = false;
    
    // Single pass through string
    for (let ch of inputString) {
        if (ch === ' ') {
            continue;
        } else if (ch === '-') {
            foundDash = true;
            isDec = false;
        } else if (isDec) {
            // Decimal count section
            if (ch >= '0' && ch <= '9') {
                decCount = decCount * 10 + (ch.charCodeAt(0) - 48);
            } else {
                return "ERROR";
            }
        } else {
            // Numeric section
            if (ch >= '0' && ch <= '9') {
                numPart += ch;
            } else {
                return "ERROR";
            }
        }
    }
    
    if (!foundDash || numPart.length === 0) {
        return "ERROR";
    }
    
    // If decimal count is zero
    if (decCount === 0) {
        return numPart;
    }
    
    // Padding
    if (decCount > numPart.length) {
        numPart = '0'.repeat(decCount - numPart.length) + numPart;
    }
    
    // Build output
    if (decCount >= numPart.length) {
        return "0." + numPart;
    } else {
        let splitPos = numPart.length - decCount;
        return numPart.substring(0, splitPos) + "." + numPart.substring(splitPos);
    }
}

/**
 * Management class for systems without FPU
 */
class NoFPUConverter {
    constructor() {
        this.cache = new Map();  // Cache for results
        this.totalConversions = 0;
        this.maxCacheSize = 100;
    }
    
    /**
     * Convert with result caching
     * @param {string} inputStr - Input string
     * @returns {string} - Result or "ERROR"
     */
    convert(inputStr) {
        // Check cache
        if (this.cache.has(inputStr)) {
            this.totalConversions++;
            return this.cache.get(inputStr);
        }
        
        // Convert
        let result = convertMicro(inputStr);
        
        // Store in cache (for reuse)
        if (result !== "ERROR" && this.cache.size < this.maxCacheSize) {
            this.cache.set(inputStr, result);
        }
        
        this.totalConversions++;
        return result;
    }
    
    /**
     * Clear cache to free memory
     */
    clearCache() {
        this.cache.clear();
    }
    
    /**
     * Get conversion statistics
     * @returns {Object} - Statistics object
     */
    getStats() {
        return {
            total: this.totalConversions,
            cacheSize: this.cache.size
        };
    }
}

/**
 * Extremely lightweight version for very small systems
 * No caching, minimal memory usage
 * 
 * @param {string} input - Input string
 * @param {number} maxOutput - Maximum output length (optional)
 * @returns {string} - Result or "ERROR"
 */
function convertUltraLight(input, maxOutput = 64) {
    let numPart = '';
    let decCount = 0;
    let isDec = true;
    let foundDash = false;
    
    // Single pass
    for (let ch of input) {
        if (ch === ' ') continue;
        if (ch === '-') {
            foundDash = true;
            isDec = false;
            continue;
        }
        if (isDec) {
            if (ch >= '0' && ch <= '9') {
                decCount = decCount * 10 + (ch.charCodeAt(0) - 48);
            } else {
                return "ERROR";
            }
        } else {
            if (ch >= '0' && ch <= '9') {
                if (numPart.length < maxOutput) {
                    numPart += ch;
                }
            } else {
                return "ERROR";
            }
        }
    }
    
    if (!foundDash || numPart.length === 0) {
        return "ERROR";
    }
    
    if (decCount === 0) {
        return numPart;
    }
    
    if (decCount > numPart.length) {
        numPart = '0'.repeat(decCount - numPart.length) + numPart;
    }
    
    if (decCount >= numPart.length) {
        return "0." + numPart;
    } else {
        let splitPos = numPart.length - decCount;
        return numPart.substring(0, splitPos) + "." + numPart.substring(splitPos);
    }
}

/**
 * Run test cases
 */
function runTests() {
    console.log('\n=== Running Tests ===\n');
    
    const tests = [
        { input: '2-101013', expected: '1010.13' },
        { input: '3-123', expected: '0.123' },
        { input: '0-12345', expected: '12345' },
        { input: '4-123', expected: '0.0123' },
        { input: '2-1', expected: '0.01' },
        { input: '1-1234', expected: '123.4' },
        { input: '5-123456', expected: '1.23456' },
        { input: '2-100', expected: '1.00' },
        { input: '3-000', expected: '0.000' },
        { input: '6-1', expected: '0.000001' }
    ];
    
    let passed = 0;
    let total = tests.length;
    
    for (let i = 0; i < tests.length; i++) {
        const test = tests[i];
        const result = convertNoFPU(test.input);
        const success = result.result === test.expected;
        
        process.stdout.write(`Test ${i + 1}: ${test.input} -> `);
        if (result.error) {
            console.log(`ERROR: ${result.error}`);
        } else {
            process.stdout.write(`${result.result}`);
            if (success) {
                console.log(' ✓ PASSED');
                passed++;
            } else {
                console.log(` ✗ FAILED (expected: ${test.expected})`);
            }
        }
    }
    
    console.log(`\nPassed: ${passed}/${total} tests`);
    console.log('====================\n');
}

/**
 * Main program
 */
function main() {
    const readline = require('readline');
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    
    console.log('============================================================');
    console.log('NoFPU Decimal Converter - Suitable for embedded systems');
    console.log('============================================================');
    console.log('');
    console.log('Features:');
    console.log('✓ No float usage');
    console.log('✓ No floating point operations');
    console.log('✓ Minimal CPU usage');
    console.log('✓ Minimal memory usage');
    console.log('✓ Suitable for systems without FPU');
    console.log('');
    console.log('Input format: decimal_count-number');
    console.log('Example: 2-101013 -> 1010.13');
    console.log('Enter "exit" to quit');
    console.log('Enter "test" to run tests');
    console.log('');
    
    const converter = new NoFPUConverter();
    
    function promptUser() {
        rl.question('Input: ', (userInput) => {
            userInput = userInput.trim();
            
            if (userInput.toLowerCase() === 'exit') {
                console.log('\nStatistics:');
                const stats = converter.getStats();
                console.log(`Total conversions: ${stats.total}`);
                console.log(`Cache size: ${stats.cacheSize}`);
                console.log('Exiting program');
                rl.close();
                return;
            }
            
            if (userInput.toLowerCase() === 'test') {
                runTests();
                promptUser();
                return;
            }
            
            if (userInput.length === 0) {
                console.log('Please enter a value\n');
                promptUser();
                return;
            }
            
            // Convert
            const result = converter.convert(userInput);
            
            if (result === 'ERROR') {
                console.log('❌ Error: Invalid input format');
                console.log('Correct format: decimal_count-number (Example: 2-101013)\n');
            } else {
                console.log(`✅ Result: ${result}`);
                console.log('   (Without using FPU)\n');
            }
            
            promptUser();
        });
    }
    
    promptUser();
}

/**
 * Export for Node.js modules
 */
module.exports = {
    convertNoFPU,
    convertMicro,
    convertUltraLight,
    NoFPUConverter,
    runTests
};

/**
 * Run main if this file is executed directly
 */
if (require.main === module) {
    main();
}