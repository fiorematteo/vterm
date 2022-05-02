#include <vte/vte.h>
#include <regex.h>
#define PCRE2_MULTILINE 0x00000400u

#define CLR_R(x) (((x)&0xff0000) >> 16)
#define CLR_G(x) (((x)&0x00ff00) >> 8)
#define CLR_B(x) (((x)&0x0000ff) >> 0)
#define CLR_16(x) ((double)(x) / 0xff)
#define CLR_GDK(x)                                                             \
    (const GdkRGBA) {                                                          \
        .red = CLR_16(CLR_R(x)), .green = CLR_16(CLR_G(x)),                    \
        .blue = CLR_16(CLR_B(x)), .alpha = 1                                   \
    }

#define PALETTE_16                                                             \
    (const GdkRGBA[]) {                                                        \
        CLR_GDK(0x000000),     /*#000000*/                                     \
            CLR_GDK(0xcd0000), /*#cd0000*/                                     \
            CLR_GDK(0x00cd00), /*#00cd00*/                                     \
            CLR_GDK(0xcdcd00), /*#cdcd00*/                                     \
            CLR_GDK(0x5c5cff), /*#5c5cff*/                                     \
            CLR_GDK(0xcd00cd), /*#cd00cd*/                                     \
            CLR_GDK(0x00cdcd), /*#00cdcd*/                                     \
            CLR_GDK(0xe5e5e5), /*#e5e5e5*/                                     \
                                                                               \
            CLR_GDK(0x7f7f7f), /*#7f7f7f*/                                     \
            CLR_GDK(0xff0000), /*#ff0000*/                                     \
            CLR_GDK(0x00ff00), /*#00ff00*/                                     \
            CLR_GDK(0xffff00), /*#ffff00*/                                     \
            CLR_GDK(0x0000ee), /*#0000ee*/                                     \
            CLR_GDK(0xff00ff), /*#ff00ff*/                                     \
            CLR_GDK(0x00ffff), /*#00ffff*/                                     \
            CLR_GDK(0xffffff)  /*#ffffff*/                                     \
    }

#define DEFAULT_FONT "DejaVuSansMono Nerd Font 11"

struct key_comb {
    int mod_key;
    int key;
};

struct key_comb CTRL_SHIFT_V = {
    .mod_key = (GDK_CONTROL_MASK | GDK_SHIFT_MASK),
    .key = GDK_KEY_V,
};

struct key_comb CTRL_SHIFT_C = {
    .mod_key = (GDK_CONTROL_MASK | GDK_SHIFT_MASK),
    .key = GDK_KEY_C,
};

struct key_comb CTRL_PLUS = {
    .mod_key = (GDK_CONTROL_MASK),
    .key = GDK_KEY_plus,
};

struct key_comb CTRL_MINUS = {
    .mod_key = (GDK_CONTROL_MASK),
    .key = GDK_KEY_minus,
};

