#ifndef XX
#define XX

#define	BUF_MAX_LEN	512
#define	ASSERT		assert
#define	ERRNUM		2000
#define	TOK_MAX_LEN	128

/*
 * Komunikaty dot.
 */
extern const char *errmsg[];

/*
 * Tagi potrzebne do oznaczenia, w którym miejscu formatu plików
 * jeste¶my. To w ogóle nie powinno byæ potrzebne :-(
 */
#define	FA_FMT_DFA	"<tablica wejsciowa ASD>"
#define FA_FMT_NFA	"<tablica wejsciowa ASN>"
#define FA_FMT_FINAL	"<tablica stanow koncowych>"

#define FA_INSERT_NOTHING	-1
#define	FA_INSERT_EXT	1

/*
 * Stan automatu.
 */
struct FA_State {
	char *name;

	/*
	 * Stan mo¿e byæ:
	 * - pierwszy
	 * - ostatni (docelowy -- s³owo akceptowane)
	 * - obecny (potrzebne do pracy symulatora)
	 */
#define FA_STATE_FIRST (1 << 0)
#define FA_STATE_LAST (1 << 1)
#define FA_STATE_CURR (1 << 2)
	int flag;

	/* Simulator support */
	void *obj;
};

/*
 * S³owo automatu.
 */
struct FA_word {
	char *word;
};

/*
 * Mo¿liwe typy automatu.
 */
typedef enum {
	FA_TYPE_DFA,
	FA_TYPE_NFA
} fa_type_t;

struct tok;

/* 
 * Struktura reprezentuje automat.
 */
struct FA {
	unsigned long	_fa_magic;
#define FA_MAGIC	0x6ba531f0
#define	FA_INIT(h)	do { (h)->_fa_magic = FA_MAGIC; } while (0)
#define FA_ASSERT(h)					\
	do {						\
		assert((h) != NULL);			\
		assert((h)->_fa_magic == FA_MAGIC);	\
	} while (0)

	/* Type of the automata. */
	fa_type_t type;

	/* Number of states */
	struct FA_State *states;
	int nstates;

	/* Temporary variable holding strings representing final states */
	struct tok **finitoks;

	/* Number of final states */
	int nfinistates;

	/* Transition table */
	struct FA_State ***transt;

	/* All tokens and table size */
	struct tok ***toks;
	int toks_rows;
	int toks_cols;

	/* Alphabet and it's size */
	struct FA_word *alpha;
	int nalpha;

	/* Simulator support */
	struct FA_State *state0;
	struct FA_State *trans_from;
	struct FA_State *trans_to;
	int simstep;

};

void FA_free(struct FA *fa);
int FA_create(struct FA **retfa, FILE *fp);
int FA_trans(struct FA *fa, int word);
void FA_dump_dot(struct FA *fa, const char *);
void FA_dump_dot_one(struct FA *fa, const char *ofname);
void FA_dump(struct FA *fa);
void FA_restart(struct FA *fa);
int FA_final(struct FA *fa);
#endif
