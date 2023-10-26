#include <snes.h>
#include "oam.h"

u8 spriteIndex;
u8 prio;

/*!\brief An array of sprites to manipulate. The format is: [oam id, x, y, hflip, vflip, gfx offset, palette offset, obj size]
*/
u16 *selectedSprites;

/*!\brief Set a sprite in the OAM.
*/
void oamSetSprite() {
    oamSet(
        selectedSprites[0], 
        selectedSprites[1], 
        selectedSprites[2], 
        prio, 
        selectedSprites[3], 
        selectedSprites[4], 
        selectedSprites[5],
        selectedSprites[6]);
    oamSetEx(selectedSprites[0], selectedSprites[7], OBJ_SHOW);
}

/*!\brief Set a sprite {X,Y} position in the OAM.
*/
void oamSetSpriteXY() {
    oamSetXY(
        selectedSprites[0], 
        selectedSprites[1], 
        selectedSprites[2]);
}
