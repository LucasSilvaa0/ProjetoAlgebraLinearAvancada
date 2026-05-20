// mfloat_bind.cpp
#include <emscripten/bind.h>
#include <sstream>
#include <string>
#include "mfloat.cpp" 

using namespace emscripten;

// 1. Pick your specific template parameters here. 
// Example: Base 10, 15 mantissa digits, max exp 100, min exp -100
using MFloat10 = _mfloat<10, 15, 100, -100>;

// 2. Helper function to bridge C++ ostream (cout/<<) to a JavaScript String
std::string mfloatToString(const MFloat10& m) {
    std::ostringstream oss;
    oss << m;
    return oss.str();
}

// Helpers for static methods with default arguments (Embind prefers exact signatures)
MFloat10 makeZeroWrapper() { return MFloat10::makeZero(); }

// 3. Define the WebAssembly bindings
EMSCRIPTEN_BINDINGS(mfloat_module) {
    class_<MFloat10>("MFloat")
        // Constructors
        .constructor<>()
        .constructor<long double>()
        
        // Map C++ operators to JS/TS named methods
        .function("add", &MFloat10::operator+)
        .function("sub", &MFloat10::operator-)
        .function("mul", &MFloat10::operator*)
        .function("div", &MFloat10::operator/)
        
        // Utilities
        .function("toDouble", &MFloat10::toDouble)
        .function("toString", &mfloatToString)
        
        // Properties
        .property("exp", &MFloat10::exp)
        .property("sign", &MFloat10::sign)
        
        // Static Methods
        .class_function("makeNan", &MFloat10::makeNan)
        .class_function("makeInf", &MFloat10::makeInf)
        .class_function("makeZero", &makeZeroWrapper);
}