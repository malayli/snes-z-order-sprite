#include <snes.h>
#include "vblank.h"
#include "utils.h"

// ROM

const u16 oamSpriteAddress = 0;
const u16 oamSpriteSize = 0x220;

/*!\brief Handles the interruption of VBlank.
    \note Send information to VRAM via DMA.
*/
void superNintendoVblank() {
    // Refresh the sprite on the screen
    dmaCopyOAram((unsigned char *) &oamMemory, oamSpriteAddress, oamSpriteSize);
}
