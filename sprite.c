#include "huc.h"

#asm
    .zp
    _data_pointer: .ds 2

    .data
    .bank 3
    .org $4000

    ;//items:
        .dw _item_cigarettes

    _spr_cigarettes:
        .incspr "graphics/cigarettes.pcx",0,0,2,1
    _pal_cigarettes:
        .incpal "graphics/cigarettes.pcx"


    _item_cigarettes:
        .dw _pal_cigarettes
        .dw _spr_cigarettes
        .db $80 ;// sprite size
        .dw 188 ;// sprite x
        .dw 167 ;// sprite y
        .db $b9 ;// ctrl_mask FLIP_MAS|SIZE_MAS = 0x88|0x31
        .db $01 ;// ctrl_value NO_FLIP|SZ_32x16;// 0x00|0x01

    .bank 59
    .org $4000

    ;//items:
        .dw _item_yellow_bot

    _spr_yellow_bot:
        .incspr "graphics/yellow-bot.pcx",0,0,1,1
    _pal_yellow_bot:
        .incpal "graphics/yellow-bot.pcx"

    _item_yellow_bot:
        .dw _pal_yellow_bot
        .dw _spr_yellow_bot
        .db $40 ;// sprite size
        .dw 50 ;// sprite x
        .dw 50 ;// sprite y
        .db $b9 ;// ctrl_mask FLIP_MAS|SIZE_MAS = 0x88|0x31
        .db $00 ;// ctrl_value NO_FLIP|SZ_16x16 = 0x00|0x00


    .code
    .macro __load_and_inc_pointer
        lda [\1]
        inc \1
    .endm

    ;// \1 = bank number
    .macro __load_item
        lda \1
        tam #$2 ;// outputs 53 04 -- why 04 when the value here is 2??

        lda $4000
        sta <_data_pointer
        lda $4000+1
        sta <_data_pointer+1

        __load_and_inc_pointer _data_pointer
        sta _palette_address
        __load_and_inc_pointer _data_pointer
        sta _palette_address+1
        __load_and_inc_pointer _data_pointer
        sta _sprite_address
        __load_and_inc_pointer _data_pointer
        sta _sprite_address+1
        __load_and_inc_pointer _data_pointer
        sta _sprite_size
        __load_and_inc_pointer _data_pointer
        sta _sprite_x
        __load_and_inc_pointer _data_pointer
        sta _sprite_x+1
        __load_and_inc_pointer _data_pointer
        sta _sprite_y
        __load_and_inc_pointer _data_pointer
        sta _sprite_y+1
        __load_and_inc_pointer _data_pointer
        sta _sprite_ctrl_mask
        __load_and_inc_pointer _data_pointer
        sta _sprite_ctrl_value
    .endm
#endasm

char bank;
int palette_address;
int sprite_address;
char sprite_array[8];

char sprite_size;
int sprite_x;
int sprite_y;
char sprite_ctrl_mask;
char sprite_ctrl_value;

char dmanager_spr_num;
int dmanager_spr_vaddr;
char dmanager_pal_num;

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

/* must set: dmanager_spr_num, dmanager_spr_vaddr, dmanager_pal_num */
load_sprite(b)
char b;
{
    bank = b;
    #asm
    ;// loads the following variables from the given bank
    ;// palette_address, sprite_address, sprite_size, sprite_x, sprite_y, sprite_ctrl_mask, sprite_ctrl_value
    __load_item _bank
    #endasm

    spr_make(dmanager_spr_num, sprite_x, sprite_y, dmanager_spr_vaddr, sprite_ctrl_mask, sprite_ctrl_value, dmanager_pal_num, 1);

    #asm
    ;// load_palette(dmanager_pal_num, palette data, 1);
    lda _dmanager_pal_num
    sta _al
    lda _bank
    sta _bl
    lda _palette_address
    sta _si
    lda _palette_address+1
    sta _si+1
    __ldwi 1
    stx _cl
    call _load_palette.3

    ;// load_vram(dmanager_spr_vaddr, sprite data, sprite_size);
    ldx _dmanager_spr_vaddr
    lda _dmanager_spr_vaddr+1
    __stw _di
    lda _bank
    sta _bl
    lda _sprite_address
    sta _si
    lda _sprite_address+1
    sta _si+1
    __ldwi _sprite_size
    __stw _cx
    call _load_vram.3
    #endasm

    return sprite_size;
}

empty_sprite_array() {
    char length, i;
    length = sizeof(sprite_array);

    for (i = 0; i < length; i++) {
        /* an element equal to 0 is considered empty */
        sprite_array[i] = 0;
    }
}

main() {
    char last_sprite_size, i;

    empty_sprite_array();
    sprite_array[0] = 3;
    sprite_array[1] = 59;

    init_satb();

    /* initialize disply manager settings */
    dmanager_spr_num = 0;
    dmanager_spr_vaddr = 0x5000;
    dmanager_pal_num = 16;

    /* load sprites in sprite_array */
    for (i = 0; i < sizeof(sprite_array); i++) {
        /* skip empty elements */
        if (sprite_array[i] == 0) {
            continue;
        }

        last_sprite_size = load_sprite(sprite_array[i]);

        /* increment display manager variables */
        dmanager_spr_vaddr += last_sprite_size;
        dmanager_spr_num++;
        dmanager_pal_num++;
    }

    satb_update();
    vsync(0);
}
