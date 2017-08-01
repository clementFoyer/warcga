#include <stdlib.h>
#include <stdio.h>
#include "warcga.h"

#include <assert.h>

void warcga_hand_print(warcga_hand_t *hand)
{
  size_t nb_cards = 0;
  for (warcga_card_t *c = hand->deck; c; c = c->next) ++nb_cards;
  printf("#cards: %2zu\n", nb_cards);
  for(warcga_card_t*c=hand->deck; c; c=c->next)
    printf("%2d%s", c->val, c->next?",":"");
  printf("\n");
}

int warcga_hand_init(warcga_hand_t *hand, warcga_card_t *deck)
{
  hand->deck = NULL;
  hand->won = deck;
  hand->points = NB_CARDS * (NB_CARDS+1);
  /* Starting with 2 card of each value */
  for(size_t i = 0; i < NB_CARDS; ++i)
    hand->distrib[i] = 2;
  /* Shuffle cards */
  warcga_hand_shuffle(hand);
  return EXIT_SUCCESS;
}

int warcga_hand_myinit(warcga_hand_t *hand, warcga_hand_t *hand2, warcga_card_t *deck)
{
  hand->deck = deck;
  warcga_hand_print(hand);
  hand->won = NULL;
  hand->points = NB_CARDS * (NB_CARDS+1);
  /* Starting with 2 card of each value */
  for(size_t i = 0; i < NB_CARDS; ++i)
    hand->distrib[i] = 2;
    
  hand2->deck = &deck[2*NB_CARDS+1];
  hand2->won = NULL;
  hand2->points = NB_CARDS * (NB_CARDS+1);
  /* Starting with 2 card of each value */
  for(size_t i = 0; i < NB_CARDS; ++i)
    hand2->distrib[i] = 2;

  deck[4*NB_CARDS-1].next = &deck[2*NB_CARDS];
  deck[2*NB_CARDS].next=NULL;
  warcga_hand_print(hand2);
  return EXIT_SUCCESS;
}

static warcga_card_t *qs_shuffle_cards(warcga_card_t *deck, size_t count)
{
  /* Edge cases */
  if (2 > count)
    return deck;
  /* quick sort based on card->shfl_val */
  warcga_card_t *p = deck;
  deck = deck->next; --count;
  warcga_card_t *crt = deck, *next = deck->next;
  warcga_card_t *sub = p, *sup_end = p;
  size_t sub_size = 0, sup_size = 0;
  for (size_t i = 0; i < count; ++i) {
    next = crt->next;
    if (p->shfl_val > crt->shfl_val) {
      crt->next = sub;
      sub = crt;
      ++sub_size;
    } else {
      sup_end->next = crt;
      sup_end = crt;
      ++sup_size;
    }
    crt = next;
  }
  sup_end->next = crt; /* Relink to the end of the hand's deck */
  p->next = qs_shuffle_cards(p->next, sup_size);
  return qs_shuffle_cards(sub, sub_size);
}

size_t warcga_hand_shuffle(warcga_hand_t *hand)
{
  if (!hand->won) return 0;
  /* Set random for placement */
  size_t nb_cards = 0;
  for (warcga_card_t *card = hand->won; card; card = card->next) {
    card->shfl_val = rand();
    ++nb_cards;
  }
  /* Actually shuffle card from given randomization */
  hand->deck = qs_shuffle_cards(hand->won, nb_cards);
  hand->won = NULL;
  assert(nb_cards >= 2);
  return nb_cards;
}
