/* ============================================================
// NoFPU Decimal Converter
// Developed by: Benyamin Gharri (Gharri.ir)
// Version: 1.0
// C++ Port
//
// This converter performs decimal conversion without using
// Floating Point Unit (FPU)
// Ideal for embedded systems, microcontrollers, and systems
// without FPU support
// ============================================================ */

#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>

using namespace std;

/**
 * Convert to decimal format without using FPU
 * Suitable for systems without Floating Point Unit
 */
string convert_no_fpu(const string& input_string, string& error_msg) {
    // Remove spaces
    string s;
    for (char ch : input_string) {
        if (ch != ' ') {
            s += ch;
        }
    }

    // Find '-' with simple loop
    size_t dash_pos = s.find('-');
    if (dash_pos == string::npos) {
        error_msg = "ERROR: '-' not found";
        return "";
    }

    // Extract parts
    string dec_str = s.substr(0, dash_pos);
    string num_str = s.substr(dash_pos + 1);

    // Validation
    if (dec_str.empty()) {
        error_msg = "ERROR: decimal count is empty";
        return "";
    }

    // Manual conversion of decimal count to integer (without stoi())
    int dec_count = 0;
    for (char ch : dec_str) {
        if (isdigit(ch)) {
            dec_count = dec_count * 10 + (ch - '0');
        } else {
            error_msg = "ERROR: invalid decimal count";
            return "";
        }
    }

    if (num_str.empty()) {
        error_msg = "ERROR: number part is empty";
        return "";
    }

    // If decimal count is zero
    if (dec_count == 0) {
        return num_str;
    }

    // Padding if needed (with minimal memory)
    if (dec_count > static_cast<int>(num_str.length())) {
        // Add zeros to the beginning
        int zeros_needed = dec_count - num_str.length();
        num_str = string(zeros_needed, '0') + num_str;
    }

    // Separate integer and decimal parts
    string result;
    if (dec_count >= static_cast<int>(num_str.length())) {
        // Number is less than 1
        result = "0." + num_str;
    } else {
        size_t split_pos = num_str.length() - dec_count;
        result = num_str.substr(0, split_pos) + "." + num_str.substr(split_pos);
    }

    error_msg = "";
    return result;
}

/**
 * Ultra-lightweight version for microcontrollers
 * Minimal memory usage, no dynamic allocations except for result
 */
string convert_micro(const string& input_string) {
    string result;
    string num_part;
    int dec_count = 0;
    bool is_dec = true;
    bool found_dash = false;

    // Single pass through string (minimal memory allocation)
    for (char ch : input_string) {
        if (ch == ' ') {
            continue;
        } else if (ch == '-') {
            found_dash = true;
            is_dec = false;
        } else if (is_dec) {
            // Decimal count section
            if (isdigit(ch)) {
                dec_count = dec_count * 10 + (ch - '0');
            } else {
                return "ERROR";
            }
        } else {
            // Numeric section
            if (isdigit(ch)) {
                num_part += ch;
            } else {
                return "ERROR";
            }
        }
    }

    if (!found_dash || num_part.empty()) {
        return "ERROR";
    }

    // If decimal count is zero
    if (dec_count == 0) {
        return num_part;
    }

    // Padding
    if (dec_count > static_cast<int>(num_part.length())) {
        num_part = string(dec_count - num_part.length(), '0') + num_part;
    }

    // Build output
    if (dec_count >= static_cast<int>(num_part.length())) {
        return "0." + num_part;
    } else {
        size_t split_pos = num_part.length() - dec_count;
        return num_part.substr(0, split_pos) + "." + num_part.substr(split_pos);
    }
}

/**
 * Management class for systems without FPU
 */
class NoFPUConverter {
private:
    unordered_map<string, string> cache;  // Cache for results
    int total_conversions;

public:
    NoFPUConverter() : total_conversions(0) {}

    /**
     * Convert with result caching
     */
    string convert(const string& input_str) {
        // Check cache
        auto it = cache.find(input_str);
        if (it != cache.end()) {
            return it->second;
        }

        // Convert
        string result = convert_micro(input_str);

        // Store in cache (for reuse)
        if (result != "ERROR" && cache.size() < 100) {
            cache[input_str] = result;
        }

        total_conversions++;
        return result;
    }

    /**
     * Clear cache to free memory
     */
    void clear_cache() {
        cache.clear();
    }

    /**
     * Get conversion statistics
     */
    void get_stats(int& total, int& cache_size) const {
        total = total_conversions;
        cache_size = cache.size();
    }
};

/**
 * Embedded system version with fixed arrays (no dynamic memory)
 * For systems with very limited memory
 */
class EmbeddedConverter {
private:
    static const int MAX_INPUT = 64;
    static const int MAX_OUTPUT = 64;

public:
    /**
     * Convert using fixed arrays (no dynamic allocation)
     * Returns true on success, false on error
     */
    bool convert_fixed(const char* input, char* output, int output_size) {
        char dec_str[MAX_INPUT];
        char num_str[MAX_INPUT];
        int dec_count = 0;
        int num_len = 0;
        int dec_len = 0;
        bool found_dash = false;
        bool is_dec = true;

        // Parse input (single pass)
        for (int i = 0; input[i] != '\0' && i < MAX_INPUT; i++) {
            if (input[i] == ' ') {
                continue;
            } else if (input[i] == '-') {
                found_dash = true;
                is_dec = false;
                continue;
            }

            if (is_dec) {
                if (isdigit(input[i])) {
                    dec_str[dec_len++] = input[i];
                } else {
                    return false;
                }
            } else {
                if (isdigit(input[i])) {
                    num_str[num_len++] = input[i];
                } else {
                    return false;
                }
            }
        }

        dec_str[dec_len] = '\0';
        num_str[num_len] = '\0';

        if (!found_dash || dec_len == 0 || num_len == 0) {
            return false;
        }

        // Convert dec_str to integer
        dec_count = 0;
        for (int i = 0; i < dec_len; i++) {
            dec_count = dec_count * 10 + (dec_str[i] - '0');
        }

        if (dec_count == 0) {
            // Copy num_str to output
            int i = 0;
            while (i < num_len && i < output_size - 1) {
                output[i] = num_str[i];
                i++;
            }
            output[i] = '\0';
            return true;
        }

        // Handle padding
        char padded[MAX_INPUT];
        int pad_len = 0;
        if (dec_count > num_len) {
            int zeros_needed = dec_count - num_len;
            for (int i = 0; i < zeros_needed; i++) {
                padded[pad_len++] = '0';
            }
        }
        for (int i = 0; i < num_len; i++) {
            padded[pad_len++] = num_str[i];
        }

        // Build output
        int out_pos = 0;
        if (dec_count >= pad_len) {
            // Number is less than 1
            output[out_pos++] = '0';
            output[out_pos++] = '.';
            for (int i = 0; i < pad_len && out_pos < output_size - 1; i++) {
                output[out_pos++] = padded[i];
            }
        } else {
            int split_pos = pad_len - dec_count;
            for (int i = 0; i < split_pos && out_pos < output_size - 1; i++) {
                output[out_pos++] = padded[i];
            }
            if (out_pos < output_size - 1) {
                output[out_pos++] = '.';
            }
            for (int i = split_pos; i < pad_len && out_pos < output_size - 1; i++) {
                output[out_pos++] = padded[i];
            }
        }
        output[out_pos] = '\0';
        return true;
    }
};

/**
 * Main program
 */
int main() {
    cout << "============================================================" << endl;
    cout << "NoFPU Decimal Converter - Suitable for embedded systems" << endl;
    cout << "============================================================" << endl;
    cout << endl;
    cout << "Features:" << endl;
    cout << "No float usage" << endl;
    cout << "No floating point operations" << endl;
    cout << "Minimal CPU usage" << endl;
    cout << "Minimal memory usage" << endl;
    cout << "Suitable for systems without FPU" << endl;
    cout << endl;
    cout << "Input format: decimal_count-number" << endl;
    cout << "Example: 2-101013 -> 1010.13" << endl;
    cout << "Enter 'exit' to quit" << endl;
    cout << endl;

    // Create converter instance
    NoFPUConverter converter;
    string user_input;

    while (true) {
        cout << "Input: ";
        getline(cin, user_input);

        // Remove leading/trailing spaces
        size_t start = user_input.find_first_not_of(" \t");
        if (start != string::npos) {
            user_input = user_input.substr(start);
        }
        size_t end = user_input.find_last_not_of(" \t");
        if (end != string::npos) {
            user_input = user_input.substr(0, end + 1);
        }

        if (user_input == "exit") {
            cout << endl;
            cout << "Statistics:" << endl;
            int total, cache_size;
            converter.get_stats(total, cache_size);
            cout << "Total conversions: " << total << endl;
            cout << "Cache size: " << cache_size << endl;
            cout << "Exiting program" << endl;
            break;
        }

        if (user_input.empty()) {
            cout << "Please enter a value" << endl << endl;
            continue;
        }

        // Convert
        string result = converter.convert(user_input);

        if (result == "ERROR") {
            cout << "Error: Invalid input format" << endl;
            cout << "Correct format: decimal_count-number (Example: 2-101013)" << endl << endl;
        } else {
            cout << "Result: " << result << endl;
            cout << "   (Without using FPU)" << endl << endl;
        }
    }

    return 0;
}
