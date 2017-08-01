#ifndef _WARCGA_H_
#define _WARCGA_H_

#include <stdlib.h>

#define NB_CARDS 13 /* Number of cards per color */

struct warcga_card_t;
typedef struct warcga_card_t warcga_card_t;
struct warcga_hand_t;
typedef struct warcga_hand_t warcga_hand_t;

extern int warcga_verbose;

typedef enum {
  ERROR = -1,
  TIE   = 0,
  P1_WINS,
  P2_WINS
} warcga_winner_t;

struct warcga_card_t {
  struct warcga_card_t *next;
  int val;
  char color;
  int shfl_val;
};

struct warcga_hand_t {
  /* play values */
  warcga_card_t *deck;
  warcga_card_t *won;
  /* stats values */
  unsigned int points;
  unsigned int distrib[NB_CARDS];
};

extern int warcga_hand_init(warcga_hand_t *hand, warcga_card_t *deck);

extern size_t warcga_hand_shuffle(warcga_hand_t *hand);

extern void warcga_deck_print(warcga_hand_t *hand);


extern int warcga_hand_myinit(warcga_hand_t *, warcga_hand_t *, warcga_card_t *deck);
#endif /* _WARCGA_H_ */
