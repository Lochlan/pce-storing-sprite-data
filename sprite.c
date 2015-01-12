#include "huc.h"

#incspr(cigarettes, "graphics/cigarettes.pcx", 0, 0, 2, 1);
#incpal(cigarettespal, "graphics/cigarettes.pcx");

spr_make(spriteno, spritex, spritey, spritepattern, ctrl1, ctrl2, sprpal, sprpri)
int spriteno, spritex, spritey, spritepattern, ctrl1, ctrl2, sprpal, sprpri;
{
    spr_set(spriteno);
    spr_x(spritex);
    spr_y(spritey);
    spr_pattern(spritepattern);
    spr_ctrl(ctrl1, ctrl2);
    spr_pal(sprpal);
    spr_pri(sprpri);
}

clear_display() {
    disp_off();
    cls();
    disp_on();
}

main() {
    clear_display();

    init_satb();
    spr_make(0, 188, 167, 0x5000, FLIP_MAS|SIZE_MAS, NO_FLIP|SZ_32x16, 0, 1);
    load_palette(16, cigarettespal, 1);
    load_vram(0x5000, cigarettes, 0x80);
    satb_update();
    vsync(0);
}
