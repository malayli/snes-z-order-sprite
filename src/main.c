#include <snes.h>
#include "common/vblank.h"
#include "common/utils.h"
#include "common/input.h"
#include "common/oam.h"
#include "common/palette.h"
#include "common/spriteEngine.h"
#include "helper.h"

// ROM

const u16 spritePalette[] = {
    RGB8(255, 0, 255),
    RGB8(0, 0, 255),
    RGB8(255, 0, 0),
    RGB8(0, 255, 0),
    RGB8(206, 140, 115),
    RGB8(214, 189, 173),
    RGB8(231, 231, 222),
    RGB8(140, 140, 0),
    RGB8(181, 181, 0),
    RGB8(255, 255, 0),
    RGB8(0, 0, 173),
    RGB8(82, 82, 222),
    RGB8(132, 132, 255),
    RGB8(148, 148, 148),
    RGB8(222, 222, 222),
    RGB8(0, 74, 0)
};

const u8 emptyPicture[] = {
    // First part
    0b00000000, 0b00000000, // Bit plane 1 + Bit plane 0
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000,

    // Second part
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000
};

const u8 squareFillPicture[] = {
    // First part
    0b11111111, 0b00000000,
    0b11111111, 0b00000000, 
    0b11111111, 0b00000000, 
    0b11111111, 0b00000000, 
    0b11111111, 0b00000000, 
    0b11111111, 0b00000000, 
    0b11111111, 0b00000000, 
    0b11111111, 0b00000000,

    // Second part
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000
};

const u8 squareFillPicture2[] = {
    // First part
    0b00000000, 0b11111111,
    0b00000000, 0b11111111, 
    0b00000000, 0b11111111, 
    0b00000000, 0b11111111, 
    0b00000000, 0b11111111, 
    0b00000000, 0b11111111, 
    0b00000000, 0b11111111, 
    0b00000000, 0b11111111,

    // Second part
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000
};

const u8 squareFillPicture3[] = {
    // First part
    0b11111111, 0b11111111,
    0b11111111, 0b11111111, 
    0b11111111, 0b11111111, 
    0b11111111, 0b11111111, 
    0b11111111, 0b11111111, 
    0b11111111, 0b11111111, 
    0b11111111, 0b11111111, 
    0b11111111, 0b11111111,

    // Second part
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000
};

#define SPRITES_COUNT 25
#define SPRITES_PROP_COUNT (SPRITES_COUNT<<3)
#define SPRITES_PROP_COUNT2 (SPRITES_PROP_COUNT - 1)
#define SPRITES_PROP_COUNT_PREVIOUS (SPRITES_PROP_COUNT - 8)
#define ITEMSIZE 8
#define ITEM_X_INDEX 1
#define ITEM_Y_INDEX 2
#define ITEM_COLOR_INDEX 5

// RAM

u16 spriteObjIndex;
u16 playerIndex;
u16 bg3TileMap[1024];
u16 bgTileIndex;
u16 objSprites[SPRITES_PROP_COUNT];
u16 objSpriteTemp[8];
s16 i, j;
u8 line;
u8 column;
s16 minIndex;
u16 pivot;
u16 pi;

void bubbleSort() {
    i = 0;

    while (i < SPRITES_PROP_COUNT2) {
        j = 0;

        while (j < SPRITES_PROP_COUNT2 - i) {
            if (j < SPRITES_PROP_COUNT_PREVIOUS && objSprites[j + 2] < objSprites[j + 10]) {
                objSpriteTemp[1] = objSprites[j + 1];
                objSpriteTemp[2] = objSprites[j + 2];
                objSpriteTemp[5] = objSprites[j + 5];

                objSprites[j + 1] = objSprites[j + 9];
                objSprites[j + 2] = objSprites[j + 10];
                objSprites[j + 5] = objSprites[j + 13];

                objSprites[j + 9] = objSpriteTemp[1];
                objSprites[j + 10] = objSpriteTemp[2];
                objSprites[j + 13] = objSpriteTemp[5];
            }

            j+=8;
        }

        i+=8;
    }
}

void insertionSort() {
    for (i = 8; i < SPRITES_PROP_COUNT; i+=8) {
        objSpriteTemp[1] = objSprites[i + 1];
        objSpriteTemp[2] = objSprites[i + 2];
        objSpriteTemp[5] = objSprites[i + 5];
        j = i - 8;

        // Move elements of arr[0..i-1] that are greater than key
        // to one position ahead of their current position
        while (j >= 0 && objSprites[j + 2] < objSpriteTemp[2]) {
            objSprites[j + 8 + 1] = objSprites[j + 1];
            objSprites[j + 8 + 2] = objSprites[j + 2];
            objSprites[j + 8 + 5] = objSprites[j + 5];
            j = j - 8;
        }

        objSprites[j + 8 + 1] = objSpriteTemp[1];
        objSprites[j + 8 + 2] = objSpriteTemp[2];
        objSprites[j + 8 + 5] = objSpriteTemp[5];
    }
}

u16 partition(u16 low, u16 high) {
    pivot = objSprites[high + ITEM_Y_INDEX];
    i = (low - ITEMSIZE);

    for (j = low; j <= high - 1; j+=ITEMSIZE) {
        if (objSprites[j + ITEM_Y_INDEX] > pivot) {
            i+=ITEMSIZE;
            // Swap arr[i] and arr[j]
            objSpriteTemp[ITEM_X_INDEX] = objSprites[i + ITEM_X_INDEX];
            objSpriteTemp[ITEM_Y_INDEX] = objSprites[i + ITEM_Y_INDEX];
            objSpriteTemp[ITEM_COLOR_INDEX] = objSprites[i + ITEM_COLOR_INDEX];

            objSprites[i + ITEM_X_INDEX] = objSprites[j + ITEM_X_INDEX];
            objSprites[i + ITEM_Y_INDEX] = objSprites[j + ITEM_Y_INDEX];
            objSprites[i + ITEM_COLOR_INDEX] = objSprites[j + ITEM_COLOR_INDEX];

            objSprites[j + ITEM_X_INDEX] = objSpriteTemp[ITEM_X_INDEX];
            objSprites[j + ITEM_Y_INDEX] = objSpriteTemp[ITEM_Y_INDEX];
            objSprites[j + ITEM_COLOR_INDEX] = objSpriteTemp[ITEM_COLOR_INDEX];
        }
    }
    // Swap arr[i + 1] and arr[high] (pivot)
    objSpriteTemp[ITEM_X_INDEX] = objSprites[i + ITEM_X_INDEX + ITEMSIZE];
    objSpriteTemp[ITEM_Y_INDEX] = objSprites[i + ITEM_Y_INDEX + ITEMSIZE];
    objSpriteTemp[ITEM_COLOR_INDEX] = objSprites[i + ITEM_COLOR_INDEX + ITEMSIZE];

    objSprites[i + ITEM_X_INDEX + ITEMSIZE] = objSprites[high + ITEM_X_INDEX];
    objSprites[i + ITEM_Y_INDEX + ITEMSIZE] = objSprites[high + ITEM_Y_INDEX];
    objSprites[i + ITEM_COLOR_INDEX + ITEMSIZE] = objSprites[high + ITEM_COLOR_INDEX];

    objSprites[high + ITEM_X_INDEX] = objSpriteTemp[ITEM_X_INDEX];
    objSprites[high + ITEM_Y_INDEX] = objSpriteTemp[ITEM_Y_INDEX];
    objSprites[high + ITEM_COLOR_INDEX] = objSpriteTemp[ITEM_COLOR_INDEX];

    return (i + ITEMSIZE);
}

void quickSort(int low, int high) {
    if (low < high) {
        pi = partition(low, high);
        quickSort(low, pi - ITEMSIZE);
        quickSort(pi + ITEMSIZE, high);
    }
}

void selectionSort() {
    // Traverse the array
    for (i = 0; i < SPRITES_PROP_COUNT - 1; i+=8) {
        // Find the minimum element in the remaining unsorted part of the array
        minIndex = i;
        for (j = i + 8; j < SPRITES_PROP_COUNT; j+=8) {
            if (objSprites[j + 2] > objSprites[minIndex + 2]) {
                minIndex = j;
            }
        }

        // Swap the found minimum element with the element at arr[i]
        objSpriteTemp[1] = objSprites[i + 1];
        objSpriteTemp[2] = objSprites[i + 2];
        objSpriteTemp[5] = objSprites[i + 5];

        objSprites[i + 1] = objSprites[minIndex + 1];
        objSprites[i + 2] = objSprites[minIndex + 2];
        objSprites[i + 5] = objSprites[minIndex + 5];

        objSprites[minIndex + 1] = objSpriteTemp[1];
        objSprites[minIndex + 2] = objSpriteTemp[2];
        objSprites[minIndex + 5] = objSpriteTemp[5];
    }
}

/*!\brief Set all the tiles to 0, set a palette number and a tile priority.
*/
void clearBgTextEx(u16 *tileMap, u8 paletteNumber, u8 priority) {
    for (bgTileIndex=0; bgTileIndex < 1024;) {
        tileMap[bgTileIndex] = 0 | (paletteNumber<<10) | (priority<<13);
        bgTileIndex += 1;
    }
}

/*!\brief Load a black background on BG3.
*/
void initBg3Black() {
    bgSetMapPtr(BG2, 0x0000 + 2048, SC_32x32);
    bgSetGfxPtr(BG2, 0x5000);
    clearBgTextEx((u16 *)bg3TileMap, PAL0, 0);
    WaitForVBlank();
    setPaletteColor(PAL0, blackColor);
    dmaCopyVram((u8 *)bg3TileMap, 0x1000, 32*32*2);
    dmaCopyVram((u8 *)emptyPicture, 0x5000, 32);
}

void initForegroundPalette(u8 *source, u16 tilePaletteNumber) {
    dmaCopyCGram(source, 128 + (tilePaletteNumber<<4), 32);
}

int main(void) {
    consoleInit();

    nmiSet(superNintendoVblank);

    initBg3Black();

    setMode(BG_MODE1, BG3_MODE1_PRORITY_HIGH);
    bgSetDisable(BG0);
    bgSetDisable(BG1);
    bgSetEnable(BG2);
    bgSetDisable(BG3);

    // Workaround for SNES
    bgSetScroll(BG0, 0, -1);
    bgSetScroll(BG1, 0, -1);
    bgSetScroll(BG2, 0, -1);

    WaitForVBlank();

    playerIndex = 0;
    prio = 2;

    initSpriteEngine(OBJ_SIZE8_L16);

    initForegroundPalette((u8 *)spritePalette, PAL0);
    dmaCopyVram((u8 *)squareFillPicture, oamAddressOrigin, 32);
    dmaCopyVram((u8 *)squareFillPicture2, oamAddressOrigin + 0x10, 32);
    dmaCopyVram((u8 *)squareFillPicture3, oamAddressOrigin + 0x20, 32);

    WaitForVBlank();

    spriteIndex = 0;
    spriteObjIndex = 0;
    line = 0;
    column = 0;
    while(spriteIndex < SPRITES_COUNT) {
        objSprites[spriteObjIndex] = spriteIndex<<2;
        objSprites[spriteObjIndex + 3] = 0;
        objSprites[spriteObjIndex + 4] = 0;
        objSprites[spriteObjIndex + 6] = PAL0;
        objSprites[spriteObjIndex + 7] = OBJ_SMALL;

        if (spriteIndex == 0) {
            playerIndex = spriteObjIndex;
            objSprites[spriteObjIndex + 1] = 80;
            objSprites[spriteObjIndex + 2] = 148;
            objSprites[spriteObjIndex + 5] = 0;
            line += 16;

        } else {
            objSprites[spriteObjIndex + 1] = 64 + column;
            objSprites[spriteObjIndex + 2] = line;    
            objSprites[spriteObjIndex + 5] = spriteIndex%2 + 1;

            if (spriteIndex % 8 == 0) {
                column += 16;
                line = 16;

            } else {
                line += 16;
            }
        }

        selectedSprites = objSprites + spriteObjIndex;
        oamSetSprite();

        spriteObjIndex += 8;
        spriteIndex += 1;
    }

    setFadeEffect(FADE_IN);
	WaitForVBlank();

    while(1) {
        scanPads();
        pad0 = padsCurrent(0);
        padDown0 = padsDown(0);

        if (pad0 & KEY_RIGHT) {
            objSprites[playerIndex + 1] += 1;
            selectedSprites = objSprites + playerIndex;
            oamSetSpriteXY();
        }
        
        if (pad0 & KEY_LEFT) {
            objSprites[playerIndex + 1] -= 1;
            selectedSprites = objSprites + playerIndex;
            oamSetSpriteXY();
        }

        if (pad0 & KEY_UP) {
            objSprites[playerIndex + 2] -= 1;
            selectedSprites = objSprites + playerIndex;
            oamSetSpriteXY();
        }

        if (pad0 & KEY_DOWN) {
            objSprites[playerIndex + 2] += 1;
            selectedSprites = objSprites + playerIndex;
            oamSetSpriteXY();
        }

        //bubbleSort();
        insertionSort();
        //selectionSort();
        //quickSort(0, SPRITES_PROP_COUNT - ITEMSIZE);

        spriteIndex = 0;
        while(spriteIndex < SPRITES_COUNT) {
            selectedSprites = objSprites + (spriteIndex<<3);
            selectedSprites[0] = spriteIndex<<2;
            oamSetSprite();

            if (selectedSprites[5] == 0) {
                playerIndex = spriteIndex<<3;
            }

            spriteIndex += 1;
        }

        WaitForVBlank();
    }

    return 0;
}
