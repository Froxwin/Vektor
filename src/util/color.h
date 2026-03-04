#ifndef COLOR_H_
#define COLOR_H_

typedef struct VektorColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} VektorColor;

static VektorColor vektor_color_blank = (VektorColor){0, 0, 0, 0};

static inline VektorColor vektor_color_new(unsigned char r, unsigned char g,
                                           unsigned char b, unsigned char a) {
    return (VektorColor){r, g, b, a};
}

static inline VektorColor vektor_color_solid(unsigned char r, unsigned char g,
                                             unsigned char b) {
    return (VektorColor){r, g, b, 255};
}

#endif // COLOR_H_
