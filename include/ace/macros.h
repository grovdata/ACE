#ifndef GUARD_ACE_MACROS_H
#define GUARD_ACE_MACROS_H

/**
 * ACE Macros
 * General purpose macros used throughout all code
 * Insert all somewhat universal macros here even if used only by single module
 * so future expansion will keep macro dependencies at bay
 */

/**
 * Block count - determine how many blocks of given size are in given length
 * blockCountFloor - counts only full blocks
 * blockCountCeil - counts also non-filled block
 */
#define blockCountFloor(length, blockSize) (length/blockSize)
#define blockCountCeil(length, blockSize) ((length + blockSize-1)/blockSize)

// Math

#define ABS(x) ((x)<0 ? -(x) : (x))
#define SGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
// #define SGN(x) ((x > 0) - (x < 0)) // Branchless, subtracting is slower?
#define MIN(x,y) ((x)<(y)? (x): (y))
#define MAX(x,y) ((x)>(y)? (x): (y))
#define CLAMP(x, min, max) ((x) < (min)? (min) : ((x) > (max) ? (max) : (x)))

/**
 * Bit value macro - useful for setting & testing bits.
 */
#define BV(x) (1 << (x))
#define BTST(x, b) ((x & BV(b)) != 0)

/**
 *  Checks if given x,y is in specified tRect.
 */
#define inRect(x, y, r) (                     \
	(x) >= r.uwX && (x) <= r.uwX + r.uwWidth && \
	(y) >= r.uwY && (y) <= r.uwY+r.uwHeight     \
)

#define inAbsRect(x, y, ar) (          \
	(x) >= ar.uwX1 && (x) <= ar.uwX2  && \
	(y) >= ar.uwY1 && (y) <= ar.uwY2     \
)

#endif // GUARD_ACE_MACROS_H
