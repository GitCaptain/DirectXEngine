#include "Color.h"

using namespace NGraphics;

Color::Color(): color(0) {}

Color::Color(unsigned int val): color(val) {}

Color::Color(BYTE r, BYTE g, BYTE b): Color(r, g, b, 255) {}

Color::Color(const Color& other) : color(other.color) {}

Color::Color(BYTE r, BYTE g, BYTE b, BYTE a) {
    rgba[R] = r;
    rgba[G] = g;
    rgba[B] = b;
    rgba[A] = a;
}


Color& Color::operator=(const Color& other) {
    color = other.color;
    return *this;
}

bool Color::operator==(const Color& other) const {
    return color == other.color;
}

bool Color::operator!=(const Color& other) const {
    return color != other.color;
}

constexpr BYTE Color::getR() const {
    return rgba[R];
}

void Color::setR(BYTE r) {
    rgba[R] = r;
}

constexpr BYTE Color::getG() const {
    return rgba[G];
}

void Color::setG(BYTE g) {
    rgba[G] = g;
}

constexpr BYTE Color::getB() const {
    return rgba[B];
}

void Color::setB(BYTE b) {
    rgba[B] = b;
}

constexpr BYTE Color::getA() const {
    return rgba[A];
}

void Color::setA(BYTE a) {
    rgba[A] = a;
}
