#ifndef GUARD_ACE_MANAGER_TIMER_H
#define GUARD_ACE_MANAGER_TIMER_H

#ifdef AMIGA
#include <clib/exec_protos.h> // Amiga typedefs
#include <exec/interrupts.h>  // struct Interrupt
#include <hardware/intbits.h> // INTB_VERTB
#endif // AMIGA

#include <ace/types.h>
#include <ace/managers/memory.h>
#include <ace/utils/custom.h>

/* Types */

/**
 * Timer manager structure
 * ulGameTicks should be used by all timers, as it won't update during pause
 * Game pause should be implemented as setting ubPause to 1
 * and still calling timerProcess during pause loop
 */
typedef struct {
	struct Interrupt *pInt;        /// Must be PUBLIC memory
	ULONG ulGameTicks;             /// Actual ticks passed in game
	ULONG ulLastTime;              /// Internal - used to update ulGameTicks
	volatile UWORD uwFrameCounter; /// Incremented by VBlank interrupt
	UBYTE ubPaused;                /// 1: pause on
} tTimerManager;

/* Globals */
extern tTimerManager g_sTimerManager;

/* Functions */
void timerCreate(void);
void timerDestroy(void);

ULONG timerGet(void);
ULONG timerGetPrec(void);
ULONG timerGetDelta(
	IN ULONG ulStart,
	IN ULONG ulStop
);

BYTE timerPeek(
	IN ULONG *pTimer,
	IN ULONG ulTimerDelay
);

BYTE timerCheck(
	IN ULONG *pTimer,
	IN ULONG ulTimerDelay
);

void timerProcess(void);

void timerFormatPrec(
	OUT char *szBfr,
	IN ULONG ulPrecTime
);

/**
 *  Min number of us to wait: 5. It is recommended to wait for multiples of 5us.
 */
void timerWaitUs(
	IN UWORD uwUsCnt
);

#endif
