#ifndef NAGINATA_V16_H
#define NAGINATA_V16_H

#include <stdint.h>
#include <stdbool.h>
#include "nglist.h"
#include "nglistarray.h"

// Key bit definitions
#define B_Q (1UL << 0)
#define B_W (1UL << 1)
#define B_E (1UL << 2)
#define B_R (1UL << 3)
#define B_T (1UL << 4)
#define B_Y (1UL << 5)
#define B_U (1UL << 6)
#define B_I (1UL << 7)
#define B_O (1UL << 8)
#define B_P (1UL << 9)
#define B_A (1UL << 10)
#define B_S (1UL << 11)
#define B_D (1UL << 12)
#define B_F (1UL << 13)
#define B_G (1UL << 14)
#define B_H (1UL << 15)
#define B_J (1UL << 16)
#define B_K (1UL << 17)
#define B_L (1UL << 18)
#define B_SEMI (1UL << 19)
#define B_Z (1UL << 20)
#define B_X (1UL << 21)
#define B_C (1UL << 22)
#define B_V (1UL << 23)
#define B_B (1UL << 24)
#define B_N (1UL << 25)
#define B_M (1UL << 26)
#define B_COMMA (1UL << 27)
#define B_DOT (1UL << 28)
#define B_SLASH (1UL << 29)
#define B_SPACE (1UL << 30)

#define NONE 0

// User config union
typedef union {
    uint8_t os : 2;
    bool tategaki : true;
} user_config_t;

extern user_config_t naginata_config;

// Function prototypes
int number_of_matches(NGList *keys);
int number_of_candidates(NGList *keys);
void ng_type(NGList *keys);
bool naginata_press(OYAYUBI_EVENT ev);
bool naginata_release(OYAYUBI_EVENT ev);
void nofunc(void);
void handle_naginata_event(OYAYUBI_EVENT ev);

#endif // NAGINATA_V16_H
