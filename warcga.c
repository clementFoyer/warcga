/* #define _POSIX_SOURCE 1 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "warcga.h"

#include <assert.h>
/* The strongest card wins (ace above all). In case of tie, one extra card is drawn face dawn, then a new drawing is made to settle the round. In case of a new tie, the cards keeps getting stacked with the same rule. Aces can be taken by 2. If, in case of a tie, one of the player does not have enough cards to settle the round, the other player uses his own until the situation is resolved. The first player to not have any card looses. The cards that are won are stacked asside and shuffle when a player run out of cards in his deck. 
*/
static void warcga_exchange(warcga_hand_t *)__attribute__((unused));
static void warcga_exchange(warcga_hand_t *win_hand  __attribute__((unused))) {return;}

int warcga_verbose = 0;

/* h1 and h2 MUST have at least one card available */
static warcga_winner_t warcga_round(warcga_hand_t *h1, warcga_hand_t *h2)
{
  warcga_winner_t winner = TIE;
  warcga_card_t *c1 = h1->deck, *c2 = h2->deck;
  warcga_card_t *stack1 = c1, *stack2 = c2, **win_stack;
  do {
    if ((c2->val && c1->val > c2->val)
	|| (!c2->val && (NB_CARDS - 1) != c1->val)
	|| (!c1->val && (NB_CARDS - 1) == c2->val)) {
      /* p1 wins */
      winner = P1_WINS;
      win_stack = &h1->won;
    } else if ((c1->val && c2->val > c1->val)
	       || (!c1->val && (NB_CARDS - 1) != c2->val)
	       || (!c2->val && (NB_CARDS - 1) == c1->val)) {
      /* p2 wins */
      winner = P2_WINS;
      win_stack = &h2->won;
    } else {
      /* tie */
      if (c1->next && c1->next->next && c2->next && c2->next->next) {
	/* enough cards for P1 et P2 */
	c1 = c1->next->next;
	c2 = c2->next->next;
      } else if (c1->next && c1->next->next && c2->next && !c2->next->next) {
	/* enough cards for P1, one short for P2 */
	c1 = c1->next->next;
	c2 = c2->next;
	if (warcga_hand_shuffle(h2)) {
	  /* more than one card retrieved from P2's shuffle */
	  c2 = c2->next = h2->deck;
	} else if (c1->next) {
	  /* Need one card for P1's deck */
	  c2->next = c1->next;
	  c1->next = c2->next->next;
	  c2->next->next = NULL;
	  c2 = c2->next;
	} else if (warcga_hand_shuffle(h1)) {
	  /* more than one card retrieved from P1's shuffle */
	  c1->next = h1->deck;
	  c2->next = c1->next;
	  c1->next = c2->next->next;
	  c2->next->next = NULL;
	  c2 = c2->next;
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else if (c1->next && c1->next->next && !c2->next) {
	/* enough cards for P1, two short for P2 */
	c1 = c1->next->next;
	if (warcga_hand_shuffle(h2)) {
	  /* more than one card retrieved from P2's shuffle */
	  c2->next = h2->deck;
	  c2 = c2->next->next;
	} else if (c1->next && c1->next->next) {
	  /* Need two cards for P1's deck */
	  c2->next = c1->next;
	  c2 = c1->next->next;
	  c1->next = c2->next;
	  c2->next = NULL;
	} else if (warcga_hand_shuffle(h1)) {
	  /* more than one card retrieved from P1's shuffle */
	  if (c1->next) {
	    /* There still was one card available before reshuffle */
	    c1->next->next = h1->deck;
	  } else {
	    c1->next = h1->deck;
	  }
	  c2->next = c1->next;
	  c2 = c1->next->next;
	  c1->next = c2->next;
	  c2->next = NULL;
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else if (c1->next && !c1->next->next && c2->next && c2->next->next) {
	/* enough cards for P2, one short for P1 */
	c1 = c1->next;
	c2 = c2->next->next;
	if (warcga_hand_shuffle(h1)) {
	  /* more than one card retrieved from P1's shuffle */
	  c1 = c1->next = h1->deck;
	} else if (c2->next) {
	  /* Need one card for P2's deck */
	  c1->next = c2->next;
	  c2->next = c1->next->next;
	  c1->next->next = NULL;
	  c1 = c1->next;
	} else if (warcga_hand_shuffle(h2)) {
	  /* more than one card retrieved from P2's shuffle */
	  c1->next = h2->deck;
	  c1->next = c2->next;
	  c2->next = c1->next->next;
	  c1->next->next = NULL;
	  c1 = c1->next;
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else if (!c1->next && c2->next && c2->next->next) {
	/* enough cards for P2, two short for P1 */
	c2 = c2->next->next;
	if (warcga_hand_shuffle(h1)) {
	  /* more than one card retrieved from P1's shuffle */
	  c1->next = h1->deck;
	  c1 = c1->next->next;
	} else if (c2->next && c2->next->next) {
	  /* Need two cards for P2's deck */
	  c1->next = c2->next;
	  c1 = c2->next->next;
	  c2->next = c1->next;
	  c1->next = NULL;
	} else if (warcga_hand_shuffle(h2)) {
	  /* more than one card retrieved from P2's shuffle */
	  if (c2->next) {
	    /* There still was one card available before reshuffle */
	    c2->next->next = h2->deck;
	  } else {
	    c2->next = h2->deck;
	  }
	  c1->next = c2->next;
	  c1 = c2->next->next;
	  c2->next = c1->next;
	  c1->next = NULL;
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else if (c1->next && !c1->next->next && c2->next && !c2->next->next) {
	/* one card short for both P1 and P2 */
	size_t got1 = warcga_hand_shuffle(h1);
	size_t got2 = warcga_hand_shuffle(h2);
	if (got1 && got2) {
	  c1 = c1->next->next = h1->deck;
	  c2 = c2->next->next = h2->deck;
	} else if (got1) {
	  /* P2 needs one card from P1's deck */
	  c1 = c1->next->next = h1->deck;
	  c2 = c2->next->next = c1->next;
	  c1->next = c2->next;
	  c2->next = NULL;
	} else if (got2) {
	  /* P1 needs one card from P2's deck */
	  c2 = c2->next->next = h2->deck;
	  c1 = c1->next->next = c2->next;
	  c2->next = c1->next;
	  c1->next = NULL;
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else if (c1->next && !c1->next->next && !c2->next) {
	/* one card short for P1, two for P2 */
	size_t got1 = warcga_hand_shuffle(h1);
	size_t got2 = warcga_hand_shuffle(h2);
	if (got1 && got2) {
	  c1 = c1->next->next = h1->deck;
	  c2->next = h2->deck;
	  c2 = c2->next->next;
	} else if (got1) {
	  c1 = c1->next->next = h1->deck;
	  if (c1->next && c1->next->next) {
	    /* P1 can provide for P2 */
	    c2->next = c1->next;
	    c2 = c1->next->next;
	    c1->next = c2->next;
	    c2->next = NULL;
	  } else {
	    fprintf(stderr, "WARN: that's some pretty bad luck to all "
		    "run out of cards.\nP1 WINNER and fuck off\n");
	    winner = P1_WINS;
	    win_stack = &h1->won;
	  }
	} else if (got2) {
	  c2 = c2->next->next = h2->deck;
	  if (c2->next) {
	    /* P2 can provide for P1 */
	    c1->next = c2->next;
	    c1 = c2->next;
	    c2->next = c1->next;
	    c1->next = NULL;
	  } else {
	    fprintf(stderr, "WARN: that's some pretty bad luck to all "
		    "run out of cards.\nP2 WINNER and fuck off\n");
	    winner = P2_WINS;
	    win_stack = &h2->won;
	  }
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else if (!c1->next && c2->next && !c2->next->next) {
	/* one card short for P2, two for P1 */
	size_t got1 = warcga_hand_shuffle(h1);
	size_t got2 = warcga_hand_shuffle(h2);
	if (got1 && got2) {
	  c2 = c2->next->next = h2->deck;
	  c1->next = h1->deck;
	  c1 = c1->next->next;
	} else if (got2) {
	  c2 = c2->next->next = h2->deck;
	  if (c2->next && c2->next->next) {
	    /* P2 can provide for P1 */
	    c1->next = c2->next;
	    c1 = c2->next->next;
	    c2->next = c1->next;
	    c1->next = NULL;
	  } else {
	    fprintf(stderr, "WARN: that's some pretty bad luck to all "
		    "run out of cards.\nP2 WINNER and fuck off\n");
	    winner = P2_WINS;
	    win_stack = &h2->won;
	  }
	} else if (got1) {
	  c1->next = h1->deck;
	  c1 = c1->next->next;
	  if (c1->next) {
	    /* P1 can provide for P2 */
	    c2->next = c1->next;
	    c2 = c1->next;
	    c1->next = c2->next;
	    c2->next = NULL;
	  } else {
	    fprintf(stderr, "WARN: that's some pretty bad luck to all "
		    "run out of cards.\nP1 WINNER and fuck off\n");
	    winner = P1_WINS;
	    win_stack = &h1->won;
	  }
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      } else {
	/* Both P1 and P2 ran out of cards */
	size_t got1 = warcga_hand_shuffle(h1);
	size_t got2 = warcga_hand_shuffle(h2);
	if (got1 && got2) {
	  c1->next = h1->deck;
	  c1 = c1->next->next;
	  c2->next = h2->deck;
	  c2 = c2->next->next;
	} else if (got1) {
	  c1->next = h1->deck;
	  c1 = c1->next->next;
	  if (c1->next && c1->next->next) {
	    /* P1 can provide for P2 */
	    c2->next = c1->next;
	    c2 = c1->next->next;
	    c1->next = c2->next;
	    c2->next = NULL;
	  } else {
	    fprintf(stderr, "WARN: that's some pretty bad luck to all "
		    "run out of cards.\nP1 WINNER and fuck off\n");
	    winner = P1_WINS;
	    win_stack = &h1->won;
	  }
	} else if (got2) {
	  c2->next = h2->deck;
	  c2 = c2->next->next;
	  if (c2->next && c2->next->next) {
	    /* P2 can provide for P1 */
	    c1->next = c2->next;
	    c1 = c2->next->next;
	    c2->next = c1->next;
	    c1->next = NULL;
	  } else {
	    fprintf(stderr, "WARN: that's some pretty bad luck to all "
		    "run out of cards.\nP2 WINNER and fuck off\n");
	    winner = P2_WINS;
	    win_stack = &h2->won;	    
	  }
	} else {
	  fprintf(stderr, "ERROR: you should never have come to this place...\n");
	  return ERROR;
	}
      }
    }
  } while (TIE == winner);
  /* Someone has won. The player gathers all the cards, stack by stack */
  h1->deck = c1->next; c1->next = NULL;
  h2->deck = c2->next; c2->next = NULL;
  if ((rand()>>5)%2) { /* p1's stack below */
    c2->next = stack1; c1->next = *win_stack; *win_stack = stack2;
  } else { /* p2's stack below */
    c1->next = stack2; c2->next = *win_stack; *win_stack = stack1;
  }
  return winner;
}

static void usage(const char *name);
static void version(void);

int main(int argc, char **argv)
{
  long long unsigned int nb_iter = 1, nb_rounds = 0;
  /* Deal with args */
  char **args = argv + 1;
  --argc;
  while (argc) {
    if (!strncmp("-n", *args, 2)) {
      ++args; --argc;
      if (1 > sscanf(*args, "%llu", &nb_iter)) {
	fprintf(stderr, "ERROR: Number of iterations requested cannot be read.\n");
	return EXIT_FAILURE;
      }
    } else if (!strncmp("-v", *args, 2) || !strncmp("--verbose", *args, 9)) {
      warcga_verbose = 1;
    } else if (!strncmp("-V", *args, 2)) {
      warcga_verbose = 0;
    } else if (!strncmp("-h", *args, 2) || !strncmp("-?", *args, 2)
	       || !strncmp("--help", *args, 6)) {
      usage(*argv);
      return EXIT_SUCCESS;
    } else if (!strncmp("--version", *args, 9)) {
      version();
      return EXIT_SUCCESS;
    } else {
      fprintf(stderr, "ERROR: unknown option \"%s\"\n", *args);
      usage(*argv);
      return EXIT_FAILURE;
    }
    --argc;
    ++args;
  }
  /* Initialize games */
  unsigned int seed = time(NULL);
  srand(seed);
  warcga_card_t *deck = (warcga_card_t *)malloc(sizeof(warcga_card_t[52]));
  warcga_hand_t black, red;
  /* Initialize statistics parameters */
  long long unsigned int black_victories = 0, red_victories = 0,
    max_rounds = 0, min_rounds = (long long unsigned int)-1, total_rounds = 0;
  const long long unsigned int total_nb_iter = nb_iter;
  while (nb_iter--) {
    for (unsigned char c = 0; c < NB_CARDS; ++c) {
      deck[           c].val      = c;
      deck[           c].shfl_val = rand();
      deck[           c].next     = &deck[           c+1];
      deck[           c].color    = 'b';
      deck[  NB_CARDS+c].val      = c;
      deck[  NB_CARDS+c].shfl_val = rand();
      deck[  NB_CARDS+c].next     = &deck[  NB_CARDS+c+1];
      deck[  NB_CARDS+c].color    = 'b';
      deck[2*NB_CARDS+c].val      = c;
      deck[2*NB_CARDS+c].shfl_val = rand();
      deck[2*NB_CARDS+c].next     = &deck[2*NB_CARDS+c+1];
      deck[2*NB_CARDS+c].color    = 'r';
      deck[3*NB_CARDS+c].val      = c;
      deck[3*NB_CARDS+c].shfl_val = rand();
      deck[3*NB_CARDS+c].next     = &deck[3*NB_CARDS+c+1];
      deck[3*NB_CARDS+c].color    = 'r';
    }
    deck[2*NB_CARDS-1].next = NULL; /* end of player 1 deck */
    deck[4*NB_CARDS-1].next = NULL; /* end of player 2 deck */
    /* Initial shuffle of each player */
    warcga_hand_init(&black, deck);
    warcga_hand_init(&red, &deck[2*NB_CARDS]);
    //warcga_hand_myinit(&black, &red, deck);
    /* Play the game */
    warcga_winner_t win = TIE;
    nb_rounds = 0;
    do {
      ++nb_rounds;
      win = warcga_round(&black, &red);
      if (!black.deck) warcga_hand_shuffle(&black);
      if (!red.deck) warcga_hand_shuffle(&red);
      assert(ERROR != win);
      if ((P1_WINS == win && red.deck) || (P2_WINS == win && black.deck))
	win = TIE;
    } while (TIE == win);
    if (warcga_verbose)
    printf("%s wins in %llu rounds !\n",
	   P1_WINS == win ? "P1 (black)" : "P2 (red)", nb_rounds);
    /* do stats */
    if (P1_WINS == win) ++black_victories;
    else ++red_victories;
    if (nb_rounds < min_rounds) min_rounds = nb_rounds;
    if (nb_rounds > max_rounds) max_rounds = nb_rounds;
    total_rounds += nb_rounds;
  }
  nb_iter = total_nb_iter;
  printf("total iterations: %llu\n"
	 "total black victories: %8llu (%2f%%)\n"
	 "total   red victories: %8llu (%2f%%)\n"
	 "max rounds: %llu \t min rounds: %llu \t average: %4.3f\n",
	 nb_iter,
	 black_victories, (double)(((double)black_victories*100)/((double)nb_iter)),
	 red_victories, (double)(((double)red_victories*100)/((double)nb_iter)),
	 max_rounds, min_rounds, (double)((double)total_rounds/(double)nb_iter));
	 
  /* Clean and free */
  free(deck);  
  return EXIT_SUCCESS;
}



static void usage(const char *name) {
  printf("usage: %s [[-n <nb_iter>]|[-h|--help|-?]|[--version]]\n"
	 "\t-n\tnumber of game to be simulated\n"
	 "\t-h|--help|-?\tprints this screen\n"
	 "`t--version\tprints version and copyrights\n", name);
}
static void version(void) {
  printf("War Card Game Analyzer 0.1\n"
	 "C'est logiciel libre, vous êtes libre de le modifier et "
	 "de le redistribuer.\nCe logiciel n'est accompagné d'"
	 "ABSOLUMENT AUCUNE GARANTIE, dans les limites autorisées "
	 "par la loi applicable.\n\nÉcrit par Clément Foyer.\n");
}
