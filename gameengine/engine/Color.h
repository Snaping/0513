#pragma once

struct Color {
    unsigned char r, g, b, a;

    Color();
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    static Color Red();
    static Color Green();
    static Color Blue();
    static Color White();
    static Color Black();
    static Color Yellow();
    static Color Cyan();
    static Color Magenta();
    static Color Gray();
    static Color Orange();
    static Color Pink();
};
