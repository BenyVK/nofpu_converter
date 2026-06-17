/* ============================================================
// NoFPU Decimal Converter
// Developed by: Benyamin Gharri (Gharri.ir)
// Version: 1.0
// C Port
//
// This converter performs decimal conversion without using
// Floating Point Unit (FPU)
// Ideal for embedded systems, microcontrollers, and systems
// without FPU support
// ============================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_INPUT 64
#define MAX_OUTPUT 64
#define MAX_CACHE 100

/* ============================================================
// Basic conversion function (with error reporting)
// ============================================================ */

/**
 * Convert to decimal format without using FPU
 * Suitable for systems without Floating Point Unit
 *
 * Parameters:
 *   input: input string (format: decimal_count-number)
 *   output: buffer to store result
 *   output_size: size of output buffer
 *   error_msg: buffer to store error message (can be NULL)
 *
 * Returns: 1 on success, 0 on error
 */
int convert_no_fpu(const char* input, char* output, int output_size, char* error_msg) {
    char s[MAX_INPUT];
    char dec_str[MAX_INPUT];
    char num_str[MAX_INPUT];
    int dec_count = 0;
    int s_len = 0;
    int dec_len = 0;
    int num_len = 0;
    int dash_pos = -1;
    int i;

    // Remove spaces
    for (i = 0; input[i] != '\0' && i < MAX_INPUT - 1; i++) {
        if (input[i] != ' ') {
            s[s_len++] = input[i];
        }
    }
    s[s_len] = '\0';

    // Find '-'
    for (i = 0; i < s_len; i++) {
        if (s[i] == '-') {
            dash_pos = i;
            break;
        }
    }

    if (dash_pos == -1) {
        if (error_msg) strcpy(error_msg, "ERROR: '-' not found");
        return 0;
    }

    // Extract parts
    for (i = 0; i < dash_pos; i++) {
        dec_str[dec_len++] = s[i];
    }
    dec_str[dec_len] = '\0';

    for (i = dash_pos + 1; i < s_len; i++) {
        num_str[num_len++] = s[i];
    }
    num_str[num_len] = '\0';

    // Validation
    if (dec_len == 0) {
        if (error_msg) strcpy(error_msg, "ERROR: decimal count is empty");
        return 0;
    }

    // Manual conversion of decimal count to integer
    dec_count = 0;
    for (i = 0; i < dec_len; i++) {
        if (isdigit(dec_str[i])) {
            dec_count = dec_count * 10 + (dec_str[i] - '0');
        } else {
            if (error_msg) strcpy(error_msg, "ERROR: invalid decimal count");
            return 0;
        }
    }

    if (num_len == 0) {
        if (error_msg) strcpy(error_msg, "ERROR: number part is empty");
        return 0;
    }

    // If decimal count is zero
    if (dec_count == 0) {
        strncpy(output, num_str, output_size - 1);
        output[output_size - 1] = '\0';
        return 1;
    }

    // Padding if needed
    char padded[MAX_INPUT];
    int pad_len = 0;

    if (dec_count > num_len) {
        int zeros_needed = dec_count - num_len;
        for (i = 0; i < zeros_needed; i++) {
            padded[pad_len++] = '0';
        }
    }
    for (i = 0; i < num_len; i++) {
        padded[pad_len++] = num_str[i];
    }

    // Build output
    int out_pos = 0;
    if (dec_count >= pad_len) {
        // Number is less than 1
        output[out_pos++] = '0';
        output[out_pos++] = '.';
        for (i = 0; i < pad_len && out_pos < output_size - 1; i++) {
            output[out_pos++] = padded[i];
        }
    } else {
        int split_pos = pad_len - dec_count;
        for (i = 0; i < split_pos && out_pos < output_size - 1; i++) {
            output[out_pos++] = padded[i];
        }
        if (out_pos < output_size - 1) {
            output[out_pos++] = '.';
        }
        for (i = split_pos; i < pad_len && out_pos < output_size - 1; i++) {
            output[out_pos++] = padded[i];
        }
    }
    output[out_pos] = '\0';

    if (error_msg) error_msg[0] = '\0';
    return 1;
}

/* ============================================================
// Ultra-lightweight version for microcontrollers
// ============================================================ */

/**
 * Ultra-lightweight version for microcontrollers
 * Minimal memory usage, no dynamic allocations
 *
 * Returns: pointer to static buffer (overwrites on next call)
 */
const char* convert_micro(const char* input) {
    static char result[MAX_OUTPUT];
    static char num_part[MAX_INPUT];
    int dec_count = 0;
    int num_len = 0;
    bool is_dec = true;
    bool found_dash = false;
    int i;

    // Clear buffers
    result[0] = '\0';
    num_part[0] = '\0';

    // Single pass through string
    for (i = 0; input[i] != '\0' && i < MAX_INPUT - 1; i++) {
        if (input[i] == ' ') {
            continue;
        } else if (input[i] == '-') {
            found_dash = true;
            is_dec = false;
        } else if (is_dec) {
            // Decimal count section
            if (isdigit(input[i])) {
                dec_count = dec_count * 10 + (input[i] - '0');
            } else {
                return "ERROR";
            }
        } else {
            // Numeric section
            if (isdigit(input[i])) {
                num_part[num_len++] = input[i];
            } else {
                return "ERROR";
            }
        }
    }
    num_part[num_len] = '\0';

    if (!found_dash || num_len == 0) {
        return "ERROR";
    }

    // If decimal count is zero
    if (dec_count == 0) {
        strcpy(result, num_part);
        return result;
    }

    // Padding
    char padded[MAX_INPUT];
    int pad_len = 0;

    if (dec_count > num_len) {
        int zeros_needed = dec_count - num_len;
        for (i = 0; i < zeros_needed; i++) {
            padded[pad_len++] = '0';
        }
    }
    for (i = 0; i < num_len; i++) {
        padded[pad_len++] = num_part[i];
    }

    // Build output
    int out_pos = 0;
    if (dec_count >= pad_len) {
        result[out_pos++] = '0';
        result[out_pos++] = '.';
        for (i = 0; i < pad_len && out_pos < MAX_OUTPUT - 1; i++) {
            result[out_pos++] = padded[i];
        }
    } else {
        int split_pos = pad_len - dec_count;
        for (i = 0; i < split_pos && out_pos < MAX_OUTPUT - 1; i++) {
            result[out_pos++] = padded[i];
        }
        if (out_pos < MAX_OUTPUT - 1) {
            result[out_pos++] = '.';
        }
        for (i = split_pos; i < pad_len && out_pos < MAX_OUTPUT - 1; i++) {
            result[out_pos++] = padded[i];
        }
    }
    result[out_pos] = '\0';

    return result;
}

/* ============================================================
// Management structure for systems without FPU
// ============================================================ */

/**
 * Cache entry structure
 */
typedef struct {
    char key[MAX_INPUT];
    char value[MAX_OUTPUT];
    bool used;
} CacheEntry;

/**
 * Converter structure
 */
typedef struct {
    CacheEntry cache[MAX_CACHE];
    int total_conversions;
    int cache_size;
} NoFPUConverter;

/**
 * Initialize converter
 */
void init_converter(NoFPUConverter* converter) {
    converter->total_conversions = 0;
    converter->cache_size = 0;
    for (int i = 0; i < MAX_CACHE; i++) {
        converter->cache[i].used = false;
        converter->cache[i].key[0] = '\0';
        converter->cache[i].value[0] = '\0';
    }
}

/**
 * Find in cache
 */
int find_in_cache(NoFPUConverter* converter, const char* input, char* output) {
    for (int i = 0; i < converter->cache_size; i++) {
        if (converter->cache[i].used && strcmp(converter->cache[i].key, input) == 0) {
            strcpy(output, converter->cache[i].value);
            return 1;
        }
    }
    return 0;
}

/**
 * Add to cache
 */
void add_to_cache(NoFPUConverter* converter, const char* input, const char* result) {
    if (strcmp(result, "ERROR") == 0) return;
    if (converter->cache_size >= MAX_CACHE) return;

    strcpy(converter->cache[converter->cache_size].key, input);
    strcpy(converter->cache[converter->cache_size].value, result);
    converter->cache[converter->cache_size].used = true;
    converter->cache_size++;
}

/**
 * Convert with caching
 */
const char* convert_with_cache(NoFPUConverter* converter, const char* input) {
    static char result[MAX_OUTPUT];

    // Check cache
    if (find_in_cache(converter, input, result)) {
        converter->total_conversions++;
        return result;
    }

    // Convert
    const char* conv_result = convert_micro(input);
    strcpy(result, conv_result);

    // Store in cache
    add_to_cache(converter, input, result);

    converter->total_conversions++;
    return result;
}

/**
 * Clear cache
 */
void clear_cache(NoFPUConverter* converter) {
    for (int i = 0; i < converter->cache_size; i++) {
        converter->cache[i].used = false;
        converter->cache[i].key[0] = '\0';
        converter->cache[i].value[0] = '\0';
    }
    converter->cache_size = 0;
}

/**
 * Get statistics
 */
void get_stats(NoFPUConverter* converter, int* total, int* cache_size) {
    *total = converter->total_conversions;
    *cache_size = converter->cache_size;
}

/* ============================================================
// Extremely lightweight version for very small microcontrollers
// No cache, minimal memory usage, single pass
// ============================================================ */

/**
 * Extremely lightweight version for very small microcontrollers
 * Uses only static buffers, no dynamic allocation
 * Optimized for size and speed
 *
 * Returns: 1 on success, 0 on error
 */
int convert_ultra_light(const char* input, char* output, int output_size) {
    static char num_part[MAX_INPUT];
    int dec_count = 0;
    int num_len = 0;
    bool is_dec = true;
    bool found_dash = false;
    int i;
    int out_pos = 0;

    // Single pass through string
    for (i = 0; input[i] != '\0' && i < MAX_INPUT - 1; i++) {
        if (input[i] == ' ') {
            continue;
        } else if (input[i] == '-') {
            found_dash = true;
            is_dec = false;
        } else if (is_dec) {
            if (input[i] >= '0' && input[i] <= '9') {
                dec_count = dec_count * 10 + (input[i] - '0');
            } else {
                return 0;
            }
        } else {
            if (input[i] >= '0' && input[i] <= '9') {
                num_part[num_len++] = input[i];
            } else {
                return 0;
            }
        }
    }
    num_part[num_len] = '\0';

    if (!found_dash || num_len == 0) {
        return 0;
    }

    // If decimal count is zero
    if (dec_count == 0) {
        strncpy(output, num_part, output_size - 1);
        output[output_size - 1] = '\0';
        return 1;
    }

    // Handle padding directly without extra buffer
    char padded[MAX_INPUT];
    int pad_len = 0;

    if (dec_count > num_len) {
        int zeros_needed = dec_count - num_len;
        for (i = 0; i < zeros_needed; i++) {
            padded[pad_len++] = '0';
        }
    }
    for (i = 0; i < num_len; i++) {
        padded[pad_len++] = num_part[i];
    }

    // Build output
    if (dec_count >= pad_len) {
        output[out_pos++] = '0';
        output[out_pos++] = '.';
        for (i = 0; i < pad_len && out_pos < output_size - 1; i++) {
            output[out_pos++] = padded[i];
        }
    } else {
        int split_pos = pad_len - dec_count;
        for (i = 0; i < split_pos && out_pos < output_size - 1; i++) {
            output[out_pos++] = padded[i];
        }
        if (out_pos < output_size - 1) {
            output[out_pos++] = '.';
        }
        for (i = split_pos; i < pad_len && out_pos < output_size - 1; i++) {
            output[out_pos++] = padded[i];
        }
    }
    output[out_pos] = '\0';
    return 1;
}

/* ============================================================
// Test and demonstration functions
// ============================================================ */

/**
 * Run test cases
 */
void run_tests(void) {
    printf("\n=== Running Tests ===\n\n");

    struct {
        const char* input;
        const char* expected;
    } tests[] = {
        {"2-101013", "1010.13"},
        {"3-123", "0.123"},
        {"0-12345", "12345"},
        {"4-123", "0.0123"},
        {"2-1", "0.01"},
        {"1-1234", "123.4"},
        {"5-123456", "1.23456"},
        {"2-100", "1.00"},
        {NULL, NULL}
    };

    char output[MAX_OUTPUT];
    char error_msg[MAX_INPUT];
    int passed = 0;
    int total = 0;

    for (int i = 0; tests[i].input != NULL; i++) {
        total++;
        int result = convert_no_fpu(tests[i].input, output, MAX_OUTPUT, error_msg);
        int success = (result == 1 && strcmp(output, tests[i].expected) == 0);

        printf("Test %d: %s -> ", i + 1, tests[i].input);
        if (result) {
            printf("%s", output);
            if (success) {
                printf(" ✓ PASSED\n");
                passed++;
            } else {
                printf(" ✗ FAILED (expected: %s)\n", tests[i].expected);
            }
        } else {
            printf("ERROR: %s\n", error_msg);
        }
    }

    printf("\nPassed: %d/%d tests\n", passed, total);
    printf("====================\n\n");
}

/* ============================================================
// Main program
// ============================================================ */

int main() {
    printf("============================================================\n");
    printf("NoFPU Decimal Converter - Suitable for embedded systems\n");
    printf("============================================================\n");
    printf("\n");
    printf("Features:\n");
    printf("No float usage\n");
    printf("No floating point operations\n");
    printf("Minimal CPU usage\n");
    printf("Minimal memory usage\n");
    printf("Suitable for systems without FPU\n");
    printf("\n");
    printf("Input format: decimal_count-number\n");
    printf("Example: 2-101013 -> 1010.13\n");
    printf("Enter 'exit' to quit\n");
    printf("Enter 'test' to run tests\n");
    printf("\n");

    // Create converter instance
    NoFPUConverter converter;
    init_converter(&converter);

    char user_input[MAX_INPUT];
    char result[MAX_OUTPUT];

    while (1) {
        printf("Input: ");
        fgets(user_input, MAX_INPUT, stdin);

        // Remove newline
        size_t len = strlen(user_input);
        if (len > 0 && user_input[len - 1] == '\n') {
            user_input[len - 1] = '\0';
        }

        // Remove leading/trailing spaces
        char* start = user_input;
        while (*start == ' ' || *start == '\t') start++;

        char* end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }

        if (strcmp(start, "exit") == 0) {
            printf("\n");
            printf("Statistics:\n");
            int total, cache_size;
            get_stats(&converter, &total, &cache_size);
            printf("Total conversions: %d\n", total);
            printf("Cache size: %d\n", cache_size);
            printf("Exiting program\n");
            break;
        }

        if (strcmp(start, "test") == 0) {
            run_tests();
            continue;
        }

        if (strlen(start) == 0) {
            printf("Please enter a value\n\n");
            continue;
        }

        // Convert
        const char* conv_result = convert_with_cache(&converter, start);

        if (strcmp(conv_result, "ERROR") == 0) {
            printf("Error: Invalid input format\n");
            printf("Correct format: decimal_count-number (Example: 2-101013)\n\n");
        } else {
            printf("Result: %s\n", conv_result);
            printf("   (Without using FPU)\n\n");
        }
    }

    return 0;
}

/* ============================================================
// Example usage for embedded systems (without main)
// ============================================================ */

/**
 * Example function for embedded systems with no stdlib
 * This is a minimal version that can be used in bare-metal systems
 */
#ifdef EMBEDDED_SYSTEM

// Minimal implementation without stdlib
int convert_embedded(const char* input, char* output) {
    char num_part[MAX_INPUT];
    int dec_count = 0;
    int num_len = 0;
    int is_dec = 1;
    int found_dash = 0;
    int i;

    for (i = 0; input[i] != '\0' && i < MAX_INPUT - 1; i++) {
        if (input[i] == ' ') continue;
        if (input[i] == '-') {
            found_dash = 1;
            is_dec = 0;
            continue;
        }
        if (is_dec) {
            if (input[i] >= '0' && input[i] <= '9') {
                dec_count = dec_count * 10 + (input[i] - '0');
            } else {
                return 0;
            }
        } else {
            if (input[i] >= '0' && input[i] <= '9') {
                num_part[num_len++] = input[i];
            } else {
                return 0;
            }
        }
    }

    if (!found_dash || num_len == 0) return 0;

    if (dec_count == 0) {
        for (i = 0; i < num_len && i < MAX_OUTPUT - 1; i++) {
            output[i] = num_part[i];
        }
        output[i] = '\0';
        return 1;
    }

    // Simple formatting (no decimal point, just place decimal at correct position)
    char padded[MAX_INPUT];
    int pad_len = 0;

    if (dec_count > num_len) {
        int zeros_needed = dec_count - num_len;
        for (i = 0; i < zeros_needed; i++) {
            padded[pad_len++] = '0';
        }
    }
    for (i = 0; i < num_len; i++) {
        padded[pad_len++] = num_part[i];
    }

    int out_pos = 0;
    if (dec_count >= pad_len) {
        output[out_pos++] = '0';
        output[out_pos++] = '.';
        for (i = 0; i < pad_len && out_pos < MAX_OUTPUT - 1; i++) {
            output[out_pos++] = padded[i];
        }
    } else {
        int split_pos = pad_len - dec_count;
        for (i = 0; i < split_pos && out_pos < MAX_OUTPUT - 1; i++) {
            output[out_pos++] = padded[i];
        }
        if (out_pos < MAX_OUTPUT - 1) {
            output[out_pos++] = '.';
        }
        for (i = split_pos; i < pad_len && out_pos < MAX_OUTPUT - 1; i++) {
            output[out_pos++] = padded[i];
        }
    }
    output[out_pos] = '\0';
    return 1;
}
#endif
