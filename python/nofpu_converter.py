""" ============================================================
// NoFPU Decimal Converter
// Developed by: Benyamin Gharri (Gharri.ir)
// Version: 1.0
// Python Port
// 
// This converter performs decimal conversion without using 
// Floating Point Unit (FPU)
// Ideal for embedded systems, microcontrollers, and systems 
// without FPU support
// ============================================================ """

"""
Convert to decimal format without using FPU
Suitable for systems without Floating Point Unit
"""

def convert_no_fpu(input_string):
    """
    Convert string to decimal format without using FPU
    - No float
    - No floating point operations
    - Only string and integer operations
    """
    # Remove spaces
    s = ''
    for ch in input_string:
        if ch != ' ':
            s += ch
    
    # Find '-' with simple loop
    dash_pos = -1
    for i in range(len(s)):
        if s[i] == '-':
            dash_pos = i
            break
    
    if dash_pos == -1:
        return None, "ERROR: '-' not found"
    
    # Extract parts
    dec_str = s[:dash_pos]
    num_str = s[dash_pos + 1:]
    
    # Validation
    if not dec_str:
        return None, "ERROR: decimal count is empty"
    
    # Manual conversion of decimal count to integer (without int())
    dec_count = 0
    for ch in dec_str:
        if '0' <= ch <= '9':
            dec_count = dec_count * 10 + (ord(ch) - 48)
        else:
            return None, "ERROR: invalid decimal count"
    
    if not num_str:
        return None, "ERROR: number part is empty"
    
    # If decimal count is zero
    if dec_count == 0:
        return num_str, None
    
    # Padding if needed (with minimal memory)
    if dec_count > len(num_str):
        zeros_needed = dec_count - len(num_str)
        num_str = ('0' * zeros_needed) + num_str
    
    # Separate integer and decimal parts
    if dec_count >= len(num_str):
        result = "0." + num_str
    else:
        split_pos = len(num_str) - dec_count
        result = num_str[:split_pos] + "." + num_str[split_pos:]
    
    return result, None


# Ultra-lightweight version for microcontrollers
def convert_micro(input_string):
    """
    Microcontroller-specific version with minimal memory usage
    """
    result = []
    dec_count = 0
    num_part = ""
    is_dec = True
    found_dash = False
    
    # Single pass through string (no extra memory allocation)
    for ch in input_string:
        if ch == ' ':
            continue
        elif ch == '-':
            found_dash = True
            is_dec = False
        elif is_dec:
            if '0' <= ch <= '9':
                dec_count = dec_count * 10 + (ord(ch) - 48)
            else:
                return "ERROR"
        else:
            if '0' <= ch <= '9':
                num_part += ch
            else:
                return "ERROR"
    
    if not found_dash or not num_part:
        return "ERROR"
    
    if dec_count == 0:
        return num_part
    
    if dec_count > len(num_part):
        num_part = ('0' * (dec_count - len(num_part))) + num_part
    
    if dec_count >= len(num_part):
        return "0." + num_part
    else:
        split_pos = len(num_part) - dec_count
        return num_part[:split_pos] + "." + num_part[split_pos:]


# Management class for systems without FPU
class NoFPUConverter:
    """
    Management class for conversion on systems without FPU
    """
    def __init__(self):
        self.cache = {}
        self.total_conversions = 0
    
    def convert(self, input_str):
        """Convert with result caching"""
        if input_str in self.cache:
            return self.cache[input_str]
        
        result = convert_micro(input_str)
        
        if result != "ERROR" and len(self.cache) < 100:
            self.cache[input_str] = result
        
        self.total_conversions += 1
        return result
    
    def clear_cache(self):
        """Clear cache to free memory"""
        self.cache.clear()
    
    def get_stats(self):
        """Get conversion statistics"""
        return {
            'total': self.total_conversions,
            'cache_size': len(self.cache)
        }


# Main program
if __name__ == "__main__":
    print("=" * 60)
    print("NoFPU Decimal Converter - Suitable for embedded systems")
    print("=" * 60)
    print("\nFeatures:")
    print("✓ No float usage")
    print("✓ No floating point operations")
    print("✓ Minimal CPU usage")
    print("✓ Minimal memory usage")
    print("✓ Suitable for systems without FPU")
    print("\nInput format: decimal_count-number")
    print("Example: 2-101013 -> 1010.13")
    print("Enter 'exit' to quit\n")
    
    converter = NoFPUConverter()
    
    while True:
        user_input = input("Input: ").strip()
        
        if user_input.lower() == 'exit':
            print("\nStatistics:")
            stats = converter.get_stats()
            print(f"Total conversions: {stats['total']}")
            print(f"Cache size: {stats['cache_size']}")
            print("Exiting program")
            break
        
        if not user_input:
            print("Please enter a value\n")
            continue
        
        result = converter.convert(user_input)
        
        if result == "ERROR":
            print("❌ Error: Invalid input format")
            print("Correct format: decimal_count-number (Example: 2-101013)\n")
        else:
            print(f"✅ Result: {result}")
            print(f"   (Without using FPU)\n")