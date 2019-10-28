#pragma once
// Controller actions implementation

#include "devilution.h"

namespace dvl {

// Run after every game logic iteration.
// Handles player and menu movement.
void plrctrls_after_game_logic();

// Runs at the end of CheckCursMove()
// Handles item, object, and monster auto-aim.
void plrctrls_after_check_curs_move();

// Moves the map if active, the cursor otherwise.
void HandleRightStickMotion();

// Whether we're in a dialog menu that the game handles natively with keyboard controls.
bool InGameMenu();

void useBeltPotion(bool mana);

// Talk to towners, click on inv items, attack, etc.
void performPrimaryAction();

// Open chests, doors, pickup items.
void performSecondaryAction();

extern bool newCurHidden;
typedef struct coords {
	int x;
	int y;
} coords;
extern coords speedspellscoords[50];
extern int speedspellcount;

} // namespace dvl
