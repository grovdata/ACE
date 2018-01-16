#include "menu/menu.h"

#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/key.h>
#include <ace/managers/joy.h>
#include <ace/managers/game.h>
#include <ace/managers/blit.h>
#include <ace/utils/extview.h>
#include <ace/generic/screen.h>

#include "main.h"
#include "menu/menulist.h"

#include "test/blit.h"
#include "test/copper.h"
#include "test/font.h"
#include "test/blitsmalldest.h"
#include "test/interleaved.h"

static tView *s_pMenuView;
static tVPort *s_pMenuVPort;
static tSimpleBufferManager *s_pMenuBfr;

static tFont *s_pMenuFont;
static tMenuList *s_pMenuList; /// Menu list
static UBYTE s_ubMenuType;     /// Current menu list - see MENU_* macros

void gsMenuCreate(void) {
	// Prepare view & viewport
	s_pMenuView = viewCreate(0,
		TAG_VIEW_GLOBAL_CLUT, 1,
		TAG_DONE
	);
	s_pMenuVPort = vPortCreate(0,
		TAG_VPORT_VIEW, s_pMenuView,
		TAG_VPORT_BPP, SHOWCASE_BPP,
		TAG_DONE
	);
	s_pMenuBfr = simpleBufferCreate(0,
		TAG_SIMPLEBUFFER_VPORT, s_pMenuVPort,
		TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
		TAG_DONE
	);

	// Prepare palette
	s_pMenuVPort->pPalette[0] = 0x000;
	s_pMenuVPort->pPalette[1] = 0xAAA;
	s_pMenuVPort->pPalette[2] = 0x666;
	s_pMenuVPort->pPalette[3] = 0xFFF;
	s_pMenuVPort->pPalette[4] = 0x111;

	// Load font
	s_pMenuFont = fontCreate("data/fonts/silkscreen.fnt");

	// Prepare menu lists
	s_pMenuList = menuListCreate(
		160, 100, 3, 2,
		s_pMenuFont, FONT_HCENTER|FONT_COOKIE|FONT_SHADOW,
		1, 2, 3,
		s_pMenuBfr->pBuffer
	);
	menuShowMain();

	// Display view with its viewports
	viewLoad(s_pMenuView);
}

void gsMenuLoop(void) {
	UBYTE ubSelected;

	if (keyUse(KEY_ESCAPE)) {
		if(s_ubMenuType == MENU_MAIN)
			gameClose();
		else
			menuShowMain();
		return;
	}

	// Menu list nav - up & down
	if(keyUse(KEY_UP) || joyUse(JOY1_UP))
		menuListMove(s_pMenuList, -1);
	else if(keyUse(KEY_DOWN) || joyUse(JOY1_DOWN))
		menuListMove(s_pMenuList, +1);

	// Menu list selection
	else if(keyUse(KEY_RETURN) || joyUse(JOY1_FIRE)) {
		ubSelected = s_pMenuList->ubSelected;
		if(s_pMenuList->pEntries[ubSelected].ubDisplay == MENULIST_ENABLED)
			switch(s_ubMenuType) {
				case MENU_MAIN: menuSelectMain(); return;
				case MENU_TESTS: menuSelectTests(); return;
				case MENU_EXAMPLES: menuSelectExamples(); return;
			}
	}
}

void gsMenuDestroy(void) {
	// Destroy menu list
	menuListDestroy(s_pMenuList);

	// Free font
	fontDestroy(s_pMenuFont);

	// Destroy buffer, view & viewport
	viewDestroy(s_pMenuView);
}

void menuDrawBG() {
	UWORD uwX, uwY;
	UBYTE ubOdd = 0, ubColor;

	// Draw checkerboard
	for(uwY = 0; uwY <= s_pMenuBfr->uBfrBounds.sUwCoord.uwY - 16; uwY += 16) {
		for(uwX = 0; uwX <= s_pMenuBfr->uBfrBounds.sUwCoord.uwX - 16; uwX += 16) {
			if(ubOdd)
				ubColor = 0;
			else
				ubColor = 4;
			blitRect(s_pMenuBfr->pBuffer, uwX, uwY, 16, 16, ubColor);
			ubOdd = !ubOdd;
		}
		ubOdd = !ubOdd;
	}

	// Draw border
	blitRect(s_pMenuBfr->pBuffer, 0,0, s_pMenuBfr->uBfrBounds.sUwCoord.uwX, 1, 1);
	blitRect(
		s_pMenuBfr->pBuffer, 0, s_pMenuBfr->uBfrBounds.sUwCoord.uwY-1,
		s_pMenuBfr->uBfrBounds.sUwCoord.uwX, 1, 1
	);
	blitRect(s_pMenuBfr->pBuffer, 0,0, 1, s_pMenuBfr->uBfrBounds.sUwCoord.uwY, 1);
	blitRect(
		s_pMenuBfr->pBuffer, s_pMenuBfr->uBfrBounds.sUwCoord.uwX-1, 0,
		1, s_pMenuBfr->uBfrBounds.sUwCoord.uwY, 1
	);
}

/******************************************************* Main menu definition */

void menuShowMain(void) {
	// Draw BG
	menuDrawBG();
	fontDrawStr(
		s_pMenuBfr->pBuffer, s_pMenuFont,
		s_pMenuBfr->uBfrBounds.sUwCoord.uwX >> 1, 80,
		"ACE Showcase", 1, FONT_COOKIE|FONT_CENTER|FONT_SHADOW
	);

	// Prepare new list
	s_pMenuList->sCoord.sUwCoord.uwX = s_pMenuBfr->uBfrBounds.sUwCoord.uwX >> 1;
	s_pMenuList->sCoord.sUwCoord.uwY = 100;
	menuListResetEntries(s_pMenuList, 3);
	menuListSetEntry(s_pMenuList, 0, MENULIST_ENABLED, "Tests");
	menuListSetEntry(s_pMenuList, 1, MENULIST_DISABLED, "Examples");
	menuListSetEntry(s_pMenuList, 2, MENULIST_ENABLED, "Quit");
	s_ubMenuType = MENU_MAIN;

	// Redraw list
	menuListDraw(s_pMenuList);
}

void menuSelectMain(void) {
	switch(s_pMenuList->ubSelected) {
		case 0:
			menuShowTests();
			break;
		case 1:
			menuShowExamples();
			break;
		case 2:
			gameClose();
			return;
	}
}

/******************************************************* Test menu definition */

void menuShowTests(void) {
	// Draw BG
	menuDrawBG();
	fontDrawStr(
		s_pMenuBfr->pBuffer, s_pMenuFont,
		s_pMenuBfr->uBfrBounds.sUwCoord.uwX >> 1, 80,
		"Tests", 1, FONT_COOKIE|FONT_CENTER|FONT_SHADOW
	);

	// Prepare new list
	s_pMenuList->sCoord.sUwCoord.uwX = s_pMenuBfr->uBfrBounds.sUwCoord.uwX >> 1;
	s_pMenuList->sCoord.sUwCoord.uwY = 100;
	menuListResetEntries(s_pMenuList, 6);
	menuListSetEntry(s_pMenuList, 0, MENULIST_ENABLED, "Back");
	menuListSetEntry(s_pMenuList, 1, MENULIST_ENABLED, "Blits");
	menuListSetEntry(s_pMenuList, 2, MENULIST_ENABLED, "Fonts");
	menuListSetEntry(s_pMenuList, 3, MENULIST_ENABLED, "Copper");
	menuListSetEntry(s_pMenuList, 4, MENULIST_ENABLED, "Blits with small dst");
	menuListSetEntry(s_pMenuList, 5, MENULIST_ENABLED, "Interleaved bitmaps");
	s_ubMenuType = MENU_TESTS;

	// Redraw list
	menuListDraw(s_pMenuList);
}

void menuSelectTests(void) {
	switch(s_pMenuList->ubSelected) {
		case 0:
			menuShowMain();
			break;
		case 1:
			gameChangeState(gsTestBlitCreate, gsTestBlitLoop, gsTestBlitDestroy);
			break;
		case 2:
			gameChangeState(gsTestFontCreate, gsTestFontTableLoop, gsTestFontDestroy);
			break;
		case 3:
			gameChangeState(gsTestCopperCreate, gsTestCopperLoop, gsTestCopperDestroy);
			break;
		case 4:
			gameChangeState(
				gsTestBlitSmallDestCreate, gsTestBlitSmallDestLoop,
				gsTestBlitSmallDestDestroy
			);
			break;
		case 5:
			gameChangeState(
				gsTestInterleavedCreate, gsTestInterleavedLoop, gsTestInterleavedDestroy
			);
			break;
	}
}

/*************************************************** Examples menu definition */

void menuShowExamples(void) {
	// Draw BG
	menuDrawBG();
	fontDrawStr(
		s_pMenuBfr->pBuffer, s_pMenuFont,
		s_pMenuBfr->uBfrBounds.sUwCoord.uwX >> 1, 80,
		"Examples", 1, FONT_COOKIE|FONT_CENTER|FONT_SHADOW
	);

	// Prepare new list
	s_pMenuList->sCoord.sUwCoord.uwX = s_pMenuBfr->uBfrBounds.sUwCoord.uwX >> 1;
	s_pMenuList->sCoord.sUwCoord.uwY = 100;
	menuListResetEntries(s_pMenuList, 1);
	menuListSetEntry(s_pMenuList, 0, MENULIST_ENABLED, "Back");
	s_ubMenuType = MENU_EXAMPLES;

	// Redraw list
	menuListDraw(s_pMenuList);
}

void menuSelectExamples(void) {
	switch(s_pMenuList->ubSelected) {
		case 0:
			menuShowMain();
			break;
	}
}
