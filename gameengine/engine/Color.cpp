#include "Color.h"

Color::Color() : r(0), g(0), b(0), a(255) {}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r(r), g(g), b(b), a(a) {}

Color Color::Red() { return Color(255, 0, 0); }
Color Color::Green() { return Color(0, 255, 0); }
Color Color::Blue() { return Color(0, 0, 255); }
Color Color::White() { return Color(255, 255, 255); }
Color Color::Black() { return Color(0, 0, 0); }
Color Color::Yellow() { return Color(255, 255, 0); }
Color Color::Cyan() { return Color(0, 255, 255); }
Color Color::Magenta() { return Color(255, 0, 255); }
Color Color::Gray() { return Color(128, 128, 128); }
Color Color::Orange() { return Color(255, 165, 0); }
Color Color::Pink() { return Color(255, 192, 203); }
