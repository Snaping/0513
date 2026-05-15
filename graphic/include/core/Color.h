#pragma once

#include <cstdint>

namespace GEngine {

class Color {
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Cyan;
    static const Color Magenta;
    static const Color Gray;
    static const Color DarkGray;
    static const Color LightGray;

    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }
};

} 
