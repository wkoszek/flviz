/*
 * FLViz v2015.08.14
 * Copyright (c) 2009-2015 Wojciech A. Koszek <wojciech@koszek.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sysexits.h>
#endif
#include <getopt.h>
#include <ctype.h>

#include "queue.h"
#include "subr\subr.h"
#include "FLViz.h"

#include <gvc.h>
#include <graph.h>


int flag_D = 0;
const char *errmsg[ERRNUM];
#define RE(str)					\
	((errmsg[__LINE__] = (str)) ? -__LINE__ : -__LINE__)

/*
 * Allocate the Finite Automata (FA)
 */
static struct FA *
FA_alloc(void)
{
	struct FA *fa;
	
	fa = (struct FA *)calloc(sizeof(*fa), 1);
	FA_INIT(fa);
	FA_ASSERT(fa);
	return (fa);
}

/*
 * Destruction of FA
 */
void
FA_free(struct FA *fa)
{

	FA_ASSERT(fa);
	memset(fa, 0, sizeof(*fa));
	free(fa);
}

/* Token read from an input file */
struct tok {
	/* Token which had a new line */
	int nl;

	/* Token content */
	char str[TOK_MAX_LEN];

	/* Token's line number */
	int lineno;

	/* Position */
	int pos;
	TAILQ_ENTRY(tok) next;
};
TAILQ_HEAD(toklist, tok);

/*
 * Sensible semantics of checking, whether two NULL-terminated strings
 * are equal.
 */
static int
streq(const char *a, const char *b)
{

	return (strcmp(a, b) == 0);
}

/*
 * Remove last white-spaces, and return first non-blank character in a
 * passed string.
 */
static char *
trim(char *s)
{
	char *sbeg, *send;

	/* Find the end */
	send = strchr(s, '\0');
	ASSERT(send != NULL);
	send--;
	while (isspace(*send)) {
		/* Remove until we find something non-white.. */
		*send = '\0';
		send--;
	}
	/* Return first non-blank, non-white character. */
	sbeg = s;
	while (isspace(*sbeg))
		sbeg++;
	if (sbeg[0] == '\0')
		return (NULL);
	return (sbeg);
}

/*
 * Take a stream of bytes from file and return particular token
 * list (strings separated with whitespaces).
 */
static struct toklist *
tok_enize(FILE *fp)
{
	struct tok *tokp = NULL;
	struct toklist *tl;
	char inbuf[BUF_MAX_LEN];
	char *line = NULL;
	char *tokstr = NULL;
	int lineno = 1;

	/* We're about to return a ready-to-use token list */
	tl = (struct toklist *)calloc(sizeof(*tl), 1);
	ASSERT(tl != NULL);
	TAILQ_INIT(tl);

	for (;;) {
		line = fgets(inbuf, sizeof(inbuf), fp);
		if (line == NULL)
			break;
		line = trim(inbuf);
		if ((line == NULL) || line[0] == '\n' /* (strlen(line) <= 1) */) {
			/* 
			 * That's about 'strlen(line)...' check:
			 *
			 *
			 * This check might be a bit bogus, do to
			 * assumption, that strlen(line)==1 might not
			 * mean only "\n". Check it further later.
			 */
			lineno++;
			continue;
		}

		/* Mark, that we're not sure what's gonna happen. */
		tokp = NULL;

		/* Read token strings */
		while ((tokstr = strsep(&line, " \t")) != NULL) {
			tokp = (struct tok *)calloc(1, sizeof(*tokp));
			ASSERT(tokp != NULL);
			tokp->lineno = lineno;
			tokp->pos = (int)(tokstr - inbuf);
			(void)snprintf(tokp->str, sizeof(tokp->str) - 1, "%s", tokstr);
			TAILQ_INSERT_TAIL(tl, tokp, next);
		}

		/* 
		 * We had something. This must have contained new line
		 */
		if (tokp != NULL)
			tokp->nl = 1;

		lineno++;
	}
	return (tl);
}

/*
 * Print the token list.
 */
static void
toklist_print(struct toklist *tl)
{
	struct tok *t;

	TAILQ_FOREACH(t, tl, next)
		printf("'%s' (Line:%d, Pos:%d, State: %s)\n", t->str, t->lineno, t->pos,
		    (t->nl == 1) ? "Last (NL)" : "Normal"
		);
}

/*
 * Return howmany tokens.
 * In case howmany > 1, tokens are concatated together.
 */
static struct tok *
toklist_get(struct toklist *tl, int howmany)
{
	struct tok *ret;
	struct tok *t;
	int i;

	ret = TAILQ_FIRST(tl);
	if (ret == NULL)
		return (NULL);
	TAILQ_REMOVE(tl, ret, next);
	if (howmany <= 1)
		return (ret);
	howmany--;
	for (i = 0; i < howmany; i++) {
		t = TAILQ_FIRST(tl);
		if (t == NULL)
			break;
		TAILQ_REMOVE(tl, t, next);
		/*
		 * strsep() removes spaces
		 */
		strlcat(ret->str, " ", sizeof(ret->str));
		strlcat(ret->str, t->str, sizeof(ret->str));
		ret->lineno = t->lineno;
		ret->nl += t->nl;
		/* Free unused items */
	}
	if (i != howmany)
		return (NULL);
	return (ret);
}

/*
 * Get number from a token.
 */
static struct tok *
toklist_getnum(struct toklist *tl)
{
	struct tok *t = NULL;
	unsigned int i;

	t = toklist_get(tl, 1);
	if (t == NULL)
		return (NULL);
	for (i = 0; i < strlen(t->str); i++)
		if (!isdigit(t->str[i]))
			return (NULL);
	TAILQ_REMOVE(tl, t, next);
	return (t);
}

/*
 * Convert token to integer.
 */
static int
tok_toint(struct tok *t)
{

	ASSERT(t != NULL);
	return (atoi(t->str));
}

/*
 * Old and quite funky input file format.
 */
int
FA_create(struct FA **retfa, FILE *fp)
{
	struct tok *nstates = NULL;
	struct tok *nalpha = NULL;
	struct tok *nfinistates = NULL;
	struct tok *label = NULL;
	struct FA *fa = NULL;
	struct toklist *tl = NULL;
	struct tok *tmptk = NULL;
	struct FA_State *st;
	struct tok *tk;

	int j;
	int i;
	int ri;
	int ci;

	ASSERT(fp != NULL && ("Input stream is empty!"));

	tl = tok_enize(fp);
	ASSERT(tl != NULL);
	if (flag_D >= 2)
		toklist_print(tl);

	nstates = toklist_getnum(tl);
	if (nstates == NULL)
		return (RE("Couldn't read number of states"));
	nalpha = toklist_getnum(tl);
	if (nalpha == NULL)
		return (RE("Couldn't read number of alphabet letters"));
	nfinistates = toklist_getnum(tl);
	if (nfinistates == NULL)
		return (RE("Couldn't read number of end-states"));

	label = toklist_get(tl, 3);
	if (!streq(label->str, FA_FMT_DFA))
		return (RE("Couldn't read starting label"));
	
	fa = FA_alloc();
	fa->type = FA_TYPE_DFA;
	fa->nstates = tok_toint(nstates);
	fa->nalpha = tok_toint(nalpha);
	fa->nfinistates = tok_toint(nfinistates);

	ASSERT(fa->nfinistates <= fa->nstates);

	fa->alpha = (struct FA_word *)calloc(sizeof(*fa->alpha), fa->nalpha);
	ASSERT(fa->alpha != NULL);

	for (i = 0; i < fa->nalpha; i++) {
		tmptk = toklist_get(tl, 1);
		if (tmptk == NULL)
			return (RE("Lack of alphabet letter"));
		fa->alpha[i].word = strdup(tmptk->str);
	}

	/* Allocate data for states. */
	fa->states = (struct FA_State *)calloc(sizeof(*fa->states),
	    							fa->nstates);
	ASSERT(fa->states);

	/* 
	 * Final states.
	 */
	fa->finitoks = (struct tok **)calloc(sizeof(struct tok *),
							fa->nfinistates);
	ASSERT(fa->finitoks);

	fa->toks_rows = fa->nstates;
	fa->toks_cols = fa->nalpha + 1;

	/* Tokens */
	fa->toks = (struct tok ***)calloc(sizeof(struct tok **), fa->toks_rows);
	ASSERT(fa->toks != NULL);
	for (i = 0; i < fa->nstates; i++) {
		fa->toks[i] = (struct tok **)calloc(sizeof(struct tok *),
								fa->toks_cols);
		ASSERT(fa->toks[i] != NULL);
	}

	/* Transition table */
	fa->transt = (struct FA_State ***)calloc(sizeof(struct FA_State **),
								fa->toks_rows);
	ASSERT(fa->transt != NULL);
	for (i = 0; i < fa->nstates; i++) {
		fa->transt[i] = (struct FA_State **)
			calloc(sizeof(struct FA_State *), fa->toks_cols);
		ASSERT(fa->transt[i] != NULL);
	}

	/* Read the tokens */
	for (ri = 0; ri < fa->toks_rows; ri++) {
		for (ci = 0; ci < fa->toks_cols; ci++) {
			fa->toks[ri][ci] = toklist_get(tl, 1);
			if (fa->toks[ri][ci] == NULL)
				return (RE("Not enough input data"
					" possible input file corruption?"));
		}
	}

	/* Read a marker indicating ending states */
	label = toklist_get(tl, 3);
	if (label == NULL)
		return (RE("Lack of end-state marker"));
	if (!streq(label->str, FA_FMT_FINAL))
		return (RE("End-state marker doesn't match the pattern"));

	/* Read end-state tokens */
	for (i = 0; i < fa->nfinistates; i++) {
		fa->finitoks[i] = toklist_get(tl, 1);
		if (fa->finitoks[i] == NULL)
			return (RE("Some end-states are missing"));
	}

	/* 
	 * Converts first column from tokens to allocated states. I simply
	 * mark which state is what.
	 */
	for (ri = 0; ri < fa->toks_rows; ri++) {
		st = &fa->states[ri];
		ASSERT(st != NULL);
		tk = fa->toks[ri][0];
		ASSERT(tk != NULL);

		st->name = strdup(tk->str);
		ASSERT(st->name != NULL);

		/* 
		 * File doesn't tell which must be the first state :-(
		 * I assumed that we're talking about 1st in the file.
		 * Additionally right after reading, the 1st state is
		 * A becoming a "current state" (for simulation)
		 */
		if (ri == 0)
			st->flag |= (FA_STATE_FIRST | FA_STATE_CURR);

		/* 
		 * Checking whether the state is the end-state. If so, I
		 * need to tag it as the ending state.
		 */
		for (j = 0; j < fa->nfinistates; j++)
			if (streq(fa->finitoks[j]->str, st->name))
				st->flag |= FA_STATE_LAST;
	}

	/*
	 * Resolving transition table.
	 */
	for (ri = 0; ri < fa->toks_rows; ri++) {
		fa->transt[ri][0] = &fa->states[ri];
		for (ci = 1; ci < fa->toks_cols; ci++) {
			/* token[ri][ci] oraz przej¶cie [ri][ci] */
			tk = fa->toks[ri][ci];
			ASSERT(tk != NULL);
			fa->transt[ri][ci] = NULL;

			if (streq(tk->str, "f"))
				/* If empty state, leave NULL */
				continue;

			/* Try to resolve to a correct state */
			for (i = 0; i < fa->nstates; i++) {
				if (streq(fa->states[i].name, tk->str))
					fa->transt[ri][ci] = &fa->states[i];
			}
			if (fa->transt[ri][ci] == NULL)
				return (RE("Transition table has a state"
				    " which doesn't belong to FA. Bad input file"));
		}
	}

	/* 
	 * Support for simulation: 1st state and mark of the last transition
	 */
	fa->state0 = (struct FA_State *)calloc(sizeof(*fa->state0), 1);
	ASSERT(fa->state0 != NULL);
	fa->trans_from = fa->state0;
	fa->trans_to = &fa->states[0];
	fa->simstep = 0;

        FA_ASSERT(fa);
	*retfa = fa;
	return (0);
}

/* 
 * Print out state.
 */
static void
FA_StatePrint(struct FA_State *st)
{
	char desc[8], *dp = desc;

	if (st == NULL) {
		printf("(NUL)\t");
		return;
	}
	memset(desc, 0, sizeof(desc));

	if (st->flag & FA_STATE_FIRST)
		*dp++ = 'F';
	if (st->flag & FA_STATE_LAST)
		*dp++ = 'L';
	if (st->flag & FA_STATE_CURR)
		*dp++ = 'C';
	printf("%s%s\t", st->name, desc);
}

int
FA_trans(struct FA *fa, int word)
{
	struct FA_State *cur;
	struct FA_State *curnew;
	int sti;

	FA_ASSERT(fa);
	/* Use letters of the alphabet as indexes */
	if (word < 0 && word >= fa->nalpha)
		return (RE("Bad letter of the alphabet"));

	/*
	 * We search for current state. sti is a number of a row in our
	 * transition table. We search in states for now.
	 */
	cur = NULL;
	for (sti = 0; sti < fa->nstates; sti++) {
		cur = &fa->states[sti];
		ASSERT(cur != NULL);
		if (cur->flag & FA_STATE_CURR)
			break;
	}
	if (cur == NULL)
		/* Ops.. */
		return (RE("Terrible internal error."));

	/*
	 * We make sure if everything is OK with internal structures. i-th
	 * W element in states is i-th row in the transition table. Also
	 * W first state in the transition table has index 0. Moreover it is
	 * W a pointer to what is in states.
	 */
	ASSERT(cur == fa->transt[sti][0]);

	/*
	 * Our word has been checked in range 0..fa->nalpha. But the
	 * transition table as a first column has a state, so +1 solves the
	 * t problem
	 */
	curnew = fa->transt[sti][word + 1];

	/* 
	 * Hm.. Isn't the state lonely in terms of the state transitions?
	 */
	if (curnew != NULL) {
		cur->flag &= ~FA_STATE_CURR;
		curnew->flag |= FA_STATE_CURR;
		fa->trans_from = cur;
		fa->trans_to = curnew;
	}
	return (0);
}

/*
 * Print FA
 */
void
FA_dump(struct FA *fa)
{
	int ri;
	int ci;
	int ai;

	FA_ASSERT(fa);
	printf("   \t");
	for (ai = 0; ai < fa->nalpha; ai++)
		printf("%s\t", fa->alpha[ai].word);
	printf("\n");
	for (ri = 0; ri < fa->toks_rows; ri++) {
		for (ci = 0; ci < fa->toks_cols; ci++) {
			FA_StatePrint(fa->transt[ri][ci]);
		}
		(void)printf("\n");
	}
}

/*
 * Print in Graphviz format
 */
void
FA_dump_dot_fn(struct FA *fa, const char *ofname, int inc_step)
{
	struct FA_State *st_from = NULL;
	struct FA_State *st_to = NULL;
	struct FA_State *st = NULL;
	GVC_t *gvc = NULL;
	graph_t *g = NULL;
	Agedge_t *e = NULL;
	Agnode_t *e_from = NULL;
	Agnode_t *e_to = NULL;
	Agnode_t *state0 = NULL;
	Agnode_t **nodes = NULL;
	Agnode_t *node = NULL;
	char *argv[4] = { NULL };
	char buf[4096];
	int argc;
	int i;
	int ri, ci, ai;

	FA_ASSERT(fa);
        ASSERT(ofname != NULL);
        ASSERT(inc_step == FA_INSERT_EXT ||
               inc_step == FA_INSERT_NOTHING);

	memset(buf, 0, sizeof(buf));
	if (inc_step == FA_INSERT_EXT)
		(void)snprintf(buf, sizeof(buf) - 1, "-o%s.%d", ofname,
		    fa->simstep);
	else
		(void)snprintf(buf, sizeof(buf) - 1, "-o%s", ofname);

	argv[0] = strdup("FLViz.exe");
	argv[1] = strdup(buf);
	argv[2] = strdup("-Tsvg");
	argv[3] = NULL;
	argc = (sizeof(argv) / sizeof(argv[0])) - 1;

	gvc = gvContext();
	ASSERT(gvc != NULL);
	gvParseArgs(gvc, argc, argv);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf) - 1, "flvizstep%d", fa->simstep);
	g = agopen(buf, AGDIGRAPHSTRICT);

	/* Ustawianie atrybutów dla grafu */
	agraphattr(g, "size", "8,8");
	agraphattr(g, "rankdir", "LR");

	/* Stan 0 */
	state0 = agnode(g, "state0");
	agsafeset(state0, "label", "", "");
	agsafeset(state0, "width", "0.1", "");
	agsafeset(state0, "height", "0.1", "");
	agsafeset(state0, "shape", "box", "");
	agsafeset(state0, "style", "filled", "");
	agsafeset(state0, "fillcolor", "white", "");
	agsafeset(state0, "color", "white", "");

	nodes = (Agnode_t **)calloc(sizeof(Agnode_t *), fa->nstates);
	ASSERT(nodes != NULL);

	for (i = 0; i < fa->nstates; i++) {
		st = &fa->states[i];
		node = agnode(g, st->name);
		agsafeset(node, "label", st->name, "");
		agsafeset(node, "width", "0.5", "");
		agsafeset(node, "height", "0.5", "");
		agsafeset(node, "shape", "circle", "");
		if (st->flag & FA_STATE_LAST)
			agsafeset(node, "shape", "doublecircle", "");
		agsafeset(node, "style", "filled", "");
		agsafeset(node, "fillcolor", "gray", "");
		if (st->flag & FA_STATE_CURR)
			agsafeset(node, "fillcolor", "green", "");
		st->obj = node;
		nodes[i] = node;
	}

	e = agedge(g, state0, (Agnode_t *)fa->states[0].obj);
	agsafeset(e, "label", "START", "");
	if (fa->trans_from == fa->state0 && fa->trans_to == &fa->states[0])
		agsafeset(e, "color", "green", "");

	for (ri = 0; ri < fa->nstates; ri++) {
		st_from = fa->transt[ri][0];
		e_from = (Agnode_t *)st_from->obj;
		for (ai = 0; ai < fa->nalpha; ai++) {
			ci = ai + 1;
			st_to = fa->transt[ri][ci];
			if (st_to == NULL)
				continue;
			e_to = (Agnode_t *)st_to->obj;
			e = agedge(g, e_from, e_to);
			agsafeset(e, "label", fa->alpha[ai].word, "");
			if (fa->trans_from == st_from && fa->trans_to == st_to)
				agsafeset(e, "color", "green", "");
		}
	}

	/* Compute a layout using layout engine from command line args */
	gvLayoutJobs(gvc, g);

	/* Write the graph according to -T and -o options */
	gvRenderJobs(gvc, g);

	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);
	
	fa->simstep++;
}

/*
 * Output to the file with a simulation step name.
 */
void
FA_dump_dot(struct FA *fa, const char *ofname)
{

	FA_dump_dot_fn(fa, ofname, FA_INSERT_EXT);
}

/*
 * Output to 1 file.
 */
void
FA_dump_dot_one(struct FA *fa, const char *ofname)
{

	FA_dump_dot_fn(fa, ofname, FA_INSERT_NOTHING);
}

/*
 * Restart the FA for simulation.
 */
void
FA_restart(struct FA *fa)
{
	struct FA_State *st;
	int i;

	FA_ASSERT(fa);

	/*
	 * Remove marker for current state from all states.
	 */
	for (i = 0; i < fa->nstates; i++) {
		st = &fa->states[i];
		ASSERT(st != NULL);
		st->flag &= ~FA_STATE_CURR;
	}

	/* Mark 1st state as the current state */
	st = &fa->states[0];
	st->flag = FA_STATE_CURR;

	/* In the first state generate correct arrow */
	fa->trans_from = fa->state0;
	fa->trans_to = st;
}

/*
 * Are we in the last state?
 */
int
FA_final(struct FA *fa)
{
	struct FA_State *st;
	int i;

	FA_ASSERT(fa);
	for (i = 0; i < fa->nstates; i++) {
		st = &fa->states[i];
		/* Is it the last state? */
		if (st->flag & FA_STATE_LAST && st->flag & FA_STATE_CURR)
			return (1);
	}
	return (0);
}
