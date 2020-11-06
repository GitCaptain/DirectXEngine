#pragma once

namespace Colors {

    using BYTE = unsigned char;

    class Color {
    public:
        Color();
        Color(unsigned int val);
        Color(BYTE r, BYTE g, BYTE b);
        Color(BYTE r, BYTE g, BYTE b, BYTE a);
        Color(const Color& other);

        Color& operator=(const Color& other);
        bool operator==(const Color& other) const;
        bool operator!=(const Color& other) const;

        constexpr BYTE getR() const;
        void setR(BYTE r);
        constexpr BYTE getG() const;
        void setG(BYTE g);
        constexpr BYTE getB() const;
        void setB(BYTE b);
        constexpr BYTE getA() const;
        void setA(BYTE a);

    private:

        enum rgba_order{R, G, B, A};
        union {
            BYTE rgba[4];
            unsigned int color;
        };
    };

    const Color UnloadedTextureColor(100, 100, 100);
    const Color UnhandledTextureColor(250, 0, 0);
}