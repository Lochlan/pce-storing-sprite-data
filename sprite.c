#include "huc.h"

/* #incspr(cigarettes, "graphics/cigarettes.pcx", 0, 0, 2, 1);
#incpal(cigarettespal, "graphics/cigarettes.pcx"); */
#asm
    .data
    _cigarettes:
        .incspr "graphics/cigarettes.pcx",0,0,2,1
    _cigarettespal:
        .incpal "graphics/cigarettes.pcx"
    .code
#endasm


int sprite_address;
char sprite_bank;
int sprite_video_address;
char sprite_size;

int palette_address;
int palette_bank;


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

    #asm
    lda #bank(_cigarettespal)
    sta _palette_bank
    lda #low(_cigarettespal)
    sta _palette_address
    lda #high(_cigarettespal)
    sta _palette_address+1
    #endasm

    /*load_palette(16, cigarettespal, 1);*/
    #asm
    __ldwi 16
    stx _al
    ;//__farptr _cigarettespal,_bl,_si
    lda _palette_bank
    sta _bl
    lda _palette_address
    sta _si
    lda _palette_address+1
    sta _si+1
    __ldwi 1
    stx _cl
    call _load_palette.3
    #endasm

    sprite_video_address = 0x5000;
    #asm
    lda #bank(_cigarettes)
    sta _sprite_bank
    lda #low(_cigarettes)
    sta _sprite_address
    lda #high(_cigarettes)
    sta _sprite_address+1
    #endasm
    sprite_size = 0x80;

    /*load_vram(0x5000, cigarettes, 0x80);*/
    #asm
    ;//__ldwi $5000
    ldx _sprite_video_address
    lda _sprite_video_address+1
    __stw _di
    ;//__farptr _cigarettes,_bl,_si
    lda _sprite_bank
    sta _bl
    lda _sprite_address
    sta _si
    lda _sprite_address+1
    sta _si+1
    __ldwi _sprite_size
    __stw _cx
    call _load_vram.3
    #endasm

    satb_update();
    vsync(0);
}
