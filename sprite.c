#include "huc.h"

/* #incspr(cigarettes, "graphics/cigarettes.pcx", 0, 0, 2, 1);
#incpal(cigarettespal, "graphics/cigarettes.pcx"); */
#asm
    .data
    _spr_cigarettes:
        .incspr "graphics/cigarettes.pcx",0,0,2,1
    _pal_cigarettes:
        .incpal "graphics/cigarettes.pcx"
    .code
#endasm


char palette_bank;
int palette_address;
char sprite_bank;
int sprite_address;
char sprite_size;
int sprite_x;
int sprite_y;
char sprite_ctrl_mask;
char sprite_ctrl_value;

char dmanager_spr_num;
int dmanager_spr_vaddr;


spr_make(num, x, y, vaddr, ctrl_mask, ctrl_value, pal, pri)
char num; int x, y, vaddr; char ctrl_mask, ctrl_value, pal, pri;
{
    spr_set(num);
    spr_x(x);
    spr_y(y);
    spr_pattern(vaddr);
    spr_ctrl(ctrl_mask, ctrl_value);
    spr_pal(pal);
    spr_pri(pri);
}

clear_display() {
    disp_off();
    cls();
    disp_on();
}

main() {
    clear_display();

    #asm
    lda #bank(_pal_cigarettes)
    sta _palette_bank
    lda #low(_pal_cigarettes)
    sta _palette_address
    lda #high(_pal_cigarettes)
    sta _palette_address+1

    lda #bank(_spr_cigarettes)
    sta _sprite_bank
    lda #low(_spr_cigarettes)
    sta _sprite_address
    lda #high(_spr_cigarettes)
    sta _sprite_address+1
    lda #$80
    sta _sprite_size

    lda #low(188)
    sta _sprite_x
    lda #high(188)
    sta _sprite_x+1
    lda #low(167)
    sta _sprite_y
    lda #high(167)
    sta _sprite_y+1

    lda #$b9 ;// FLIP_MAS|SIZE_MAS = 0x88|0x31
    sta _sprite_ctrl_mask
    lda #$01 ;// NO_FLIP|SZ_32x16;// 0x00|0x01
    sta _sprite_ctrl_value

    lda #0;
    sta _dmanager_spr_num;
    lda #low(0x5000)
    sta _dmanager_spr_vaddr
    lda #high(0x5000)
    sta _dmanager_spr_vaddr+1
    #endasm

    init_satb();
    spr_make(dmanager_spr_num, sprite_x, sprite_y, dmanager_spr_vaddr, sprite_ctrl_mask, sprite_ctrl_value, 0, 1);

    /*load_palette(16, cigarettespal, 1);*/
    #asm
    __ldwi 16
    stx _al
    ;//__farptr _pal_cigarettes,_bl,_si
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

    /*load_vram(0x5000, cigarettes, 0x80);*/
    #asm
    ;//__ldwi $5000
    ldx _dmanager_spr_vaddr
    lda _dmanager_spr_vaddr+1
    __stw _di
    ;//__farptr _spr_cigarettes,_bl,_si
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
