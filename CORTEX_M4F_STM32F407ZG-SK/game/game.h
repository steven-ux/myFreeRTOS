#ifndef GAME_H
#define GAME_H

#include <inttypes.h>

extern void GAME_init();

extern void
EventHandler1();

extern void
EventHandler2();

extern void
EventHandler3();

extern void
GAME_Update();

extern void
GAME_Render();

// Mode
extern uint8_t demoMode;


#endif
