/*
 * FLViz v2015.08.14
 * Copyright (c) 2009-2015 Wojciech A. Koszek <wojciech@koszek.com>
 *
 * Przedstawiony ni�ej format danych nie jest wymy�lony przeze mnie.
 * Akceptuj� fakt, �e kolega wymy�li� go na szybko w celu stworzenia
 * programu, jednak na d�u�sz� met� uwa�am, �e �w format jest b��dny.
 * Zrobi�bym to zupe�nie inaczej, ale... Oto format:
 * --
 *
 * 5 3 2
 *
 * <tablica wejsciowa ASD>
 * 0 1 e
 * 1 f f 2
 * 2 3 f f
 * 3 f f 2
 * 4 f 5 f
 * 5 f f f
 * 
 * <tablica stanow koncowych>
 * 3 4
 * 
 * 
 * pierwsza linia to: 5 liczba stanow, 3 liczba liter w alfabecie, 2
 * liczba stanow koncowych
 * slowo kluczowe <tablica wejsciowa ASD> dla automatu ASD
 * slowo kluczowe <tablica wejsciowa ASN> dla automatu ASN
 * pierwsza linia po slowie kluczowym: litery alfabetu
 * kolejne: id_stanu przejscia_dla_kolejnych_liter_alfabetu
 * 
 * slowo kluczowe <tablica stanow koncowych>
 * identyfikatory stanow oddzielone spacjami
 * 
 * f to fi - stan pusty
 * stany indeksujemy od zera
 *
 * --
 * M�j komentarz i opis programu:
 *
 * Pierwsze linia powoduje 3 problemy -- sprawdzenie, czy wszystkie 3
 * warunki s� spe�nione (ilo�� stan�w, stan�w liter w alfabecie, ilo��
 * stan�w ko�cowych).
 *
 * Nast�pne przysparzaj� ogromnej ilo�ci problem�w :-/
 *
 * Najpierw musz� wczyta� pierwsz� KOLUMN� �eby dowiedzie� si�, jakie s�
 * stany. Dopiero potem musz� zaj�� si� "rozwi�zaniem" kolumn od 1 wzwy�
 * �eby sprawdzi�, czy stany w umieszczone w tych kolumnach rzeczywi�cie
 * nale�� do automatu, czy s� na przyk�ad efektem b��dnego wpisania
 * pliku wej�ciowego.
 *
 * Dane wczytuj� jak w kompilatorze -- najpierw na list� "generalnych"
 * kawa�k�w zwanych tokenami. Jeden token to ci�g znak�w w kt�rym nie ma
 * spacji. Spacje/tabulatory s�u�� jako separatory token�w.
 * Spacje/tabulatory *nie* pojawiaj� si� w tre�ci token�w. Przyk�ad:
 *
 * 1 2 3 wojtek koszek
 *
 * Tokeny to "1", "2", "3", "wojtek", "koszek".
 *
 * Przy pomocy tok_enize(FILE *fp), gdzie fp to wska�nik na otwarty
 * plik, uzyskuj� list� takich token�w z pliku. Wielko�� listy zale�y od
 * wielko�ci pliku i ilo�ci pami�ci RAM w komputerze. Nie ma
 * programowego ograniczenia co do wielko�ci listy.
 *
 * Pobieram 3 pierwsze warto�ci do token�w. Pobieram z nich warto�ci
 * typu integer (tok_getnum()). Sprawdzam znacznik automatu.
 *
 * Wczytuj� znaczniki alfabetu -- najpierw jako tokeny, potem od razu
 * jako s�owa. Mam nalpha liter i ka�da z nich mo�e by� liczb�/ci�giem
 * znak�w.
 * 
 * Nast�pnie alokuj� pust�, dwuwymiarow� tablic� wska�nik�w do token�w.
 * Ta swojego rodzaju pusta, dwuwymiarowa macierz wskazywa� mi b�dzie na
 * te kawa�ki tekstu, kt�re opisuj� mi automat.
 * 
 * Uk�adam w�a�nie co wczytane tokeny w taki spos�b, �eby odzorowywa�y
 * mi tablic� przej�c automatu.
 *
 * Sprawdzam znacznik ko�ca pliku i wczytuj� tokeny reprezentuj�ce moje
 * stany ko�cowe.
 *
 * Nast�pnie konwertuj� tokeny z pierwszej kolumny na w�a�ciwe struktury
 * stan�w automatu oraz zaznaczam, kt�ry stan jest pierwszy/ostatni.
 * Zaznaczam, �e dla N stan�w alokuj� N struktur. Nie wi�cej.
 *
 * Na potrzebny symulacji stan pierwszy od razu staje si� stanem
 * obecnym.
 *
 * Nast�pnie przetwarzam reszt� kolumn dot. token�w, tworz�c z nich
 * tablic� przej�� "Transition Table" (pole transt struktury FA).
 * transt jest dwuwymiarow� tablic� WSKA�NIK�w, gdzie stan w wierszu R i
 * kolumnie K odpowiada tokenowi z wiersza R i kolumny K wcze�niej
 * wspomnianej 2 wymiarowej tablicy token�w.
 *
 * Dla ka�dego tokena sprawdzam nast�puj�ce warunki:
 * - je�eli pole ma warto�� "f", wska�nik w tablicy przej�� zyskuje
 *   warto�� NULL
 *
 * - je�eli pole nie zawiera nazwy kt�regokolwiek stanu z automatu
 *   oznacza to, �e wyst�pi� b��d i przerywam dalsze przetwarzanie.
 *
 * - je�eli pole zawiera poprawn� nazw� stanu, to tablica przej��
 *   zaczyna wskazywa� na ten stan, na kt�ry nast�pi�oby przej�cie.
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
 * Alokacja automatu.
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
 * Destrukcja automatu.
 */
void
FA_free(struct FA *fa)
{

	FA_ASSERT(fa);
	memset(fa, 0, sizeof(*fa));
	free(fa);
}

/* Token wczytany z pliku wej�ciowego */
struct tok {
	/* Token zawiera�by now� lini� */
	int nl;

	/* String tokena */
	char str[TOK_MAX_LEN];

	/* Linia, z kt�rej wczytali�my token */
	int lineno;

	/* Pozycja */
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
		return (RE("Nie mog�em wczyta� ilo�ci stan�w"));
	nalpha = toklist_getnum(tl);
	if (nalpha == NULL)
		return (RE("Nie mog�em wczyta� ilo�ci liter w alfabecie"));
	nfinistates = toklist_getnum(tl);
	if (nfinistates == NULL)
		return (RE("Nie mog�em wczyta� ilo�ci stan�w ko�cowych"));

	label = toklist_get(tl, 3);
	if (!streq(label->str, FA_FMT_DFA))
		return (RE("Brak etykiety pocz�tkowej"));
	
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
			return (RE("Brak litery alfabetu"));
		fa->alpha[i].word = strdup(tmptk->str);
	}

	/* Allocate data for states. */
	fa->states = (struct FA_State *)calloc(sizeof(*fa->states), fa->nstates);
	ASSERT(fa->states);

	/* 
	 * Final states.
	 */
	fa->finitoks = (struct tok **)calloc(sizeof(struct tok *), fa->nfinistates);
	ASSERT(fa->finitoks);

	fa->toks_rows = fa->nstates;
	fa->toks_cols = fa->nalpha + 1;

	/* Tokeny */
	fa->toks = (struct tok ***)calloc(sizeof(struct tok **), fa->toks_rows);
	ASSERT(fa->toks != NULL);
	for (i = 0; i < fa->nstates; i++) {
		fa->toks[i] = (struct tok **)calloc(sizeof(struct tok *), fa->toks_cols);
		ASSERT(fa->toks[i] != NULL);
	}

	/* Tablica przej�� */
	fa->transt = (struct FA_State ***)calloc(sizeof(struct FA_State **), fa->toks_rows);
	ASSERT(fa->transt != NULL);
	for (i = 0; i < fa->nstates; i++) {
		fa->transt[i] = (struct FA_State **)calloc(sizeof(struct FA_State *), fa->toks_cols);
		ASSERT(fa->transt[i] != NULL);
	}

	/* Wczytuj� tokeny */
	for (ri = 0; ri < fa->toks_rows; ri++) {
		for (ci = 0; ci < fa->toks_cols; ci++) {
			fa->toks[ri][ci] = toklist_get(tl, 1);
			if (fa->toks[ri][ci] == NULL)
				return (RE("Niewystarczaj�ca ilo�� danych wej�ciowych"
					" prawopodobne uszkodzenie pliku"));
		}
	}

	/* Wczytuj� znacznik stan�w ko�cowych */
	label = toklist_get(tl, 3);
	if (label == NULL)
		return (RE("Brak znacznika stan�w ko�cowych"));
	if (!streq(label->str, FA_FMT_FINAL))
		return (RE("Znacznik stan�w ko�cowych nie pasuje do wzorca"));

	/* Wczytuj� tokeny stan�w ko�cowych */
	for (i = 0; i < fa->nfinistates; i++) {
		fa->finitoks[i] = toklist_get(tl, 1);
		if (fa->finitoks[i] == NULL)
			return (RE("Brak niekt�rych stan�w ko�cowych"));
	}

	/* 
	 * Konwertuj� pierwsz� kolumn� z token�w na ju� zaalokowane
	 * stany. Teraz po prostu zaznaczam kt�ry stan jest czym.
	 */
	for (ri = 0; ri < fa->toks_rows; ri++) {
		st = &fa->states[ri];
		ASSERT(st != NULL);
		tk = fa->toks[ri][0];
		ASSERT(tk != NULL);

		st->name = strdup(tk->str);
		ASSERT(st->name != NULL);

		/* 
		 * Plik nie m�wi, kt�ry ma by� stanem pierwszym :-(
		 * Zak�adam, �e chodzi o pierwszy stan. Jednocze�nie,
		 * tu� po wczytaniu, stan pierwszy staje si� stanem
		 * "obecnym" (potrzebne do symulacji).
		 */
		if (ri == 0)
			st->flag |= (FA_STATE_FIRST | FA_STATE_CURR);

		/* 
		 * Sprawdzam, czy obecnie obs�ugiwany stan znajduje si�
		 * na li�cie dot. stan�w ko�cowych. Je�eli tak, to
		 * oznacza, �e musz� zaznaczy� go jako ko�cowy.
		 */
		for (j = 0; j < fa->nfinistates; j++)
			if (streq(fa->finitoks[j]->str, st->name))
				st->flag |= FA_STATE_LAST;
	}

	/*
	 * W�a�ciwe rozwini�cie tabeli przej�� 
	 */
	for (ri = 0; ri < fa->toks_rows; ri++) {
		fa->transt[ri][0] = &fa->states[ri];
		for (ci = 1; ci < fa->toks_cols; ci++) {
			/* token[ri][ci] oraz przej�cie [ri][ci] */
			tk = fa->toks[ri][ci];
			ASSERT(tk != NULL);
			fa->transt[ri][ci] = NULL;

			if (streq(tk->str, "f"))
				/* Je�eli pusty to zostaw NULL */
				continue;

			/* Spr�buj rozwin�� go na poprawny stan */
			for (i = 0; i < fa->nstates; i++) {
				if (streq(fa->states[i].name, tk->str))
					fa->transt[ri][ci] = &fa->states[i];
			}
			if (fa->transt[ri][ci] == NULL)
				return (RE("Tabela przej�� zawiera stan, kt�ry"
				    " nie nale�y do automatu!; b��d pliku danych"));
		}
	}

	/* 
	 * Wsparce dla symulatora -- stan 0 oraz zaznaczenie ostatniego
	 * przej�cia.
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
 * Wypisz stan.
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
	/* Operujemy indeksami w tablicy liter alfabetu */
	if (word < 0 && word >= fa->nalpha)
		return (RE("Z�a litera nie nale��ce do alfabetu"));

	/* 
	 * Szukaj stanu obecnego. sti to nic innego jak numery WIERSZA
	 * naszej tablicy przej�� transt. Ale my na razie szukamy w
	 * states..
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
		return (RE("Straszliwy b��d implementacji; napisz do autora"));

	/* 
	 * Sprawdzamy, czy aby na pewno wszystko jest w porz�dku z
	 * wewn�trznymi strukturami programu -- i-ty element w states to
	 * i-ty wiersz w tabeli przej��. Dodatkowo, pierwszy stan w
	 * tabeli przej�� ma indeks 0. Dodatkowo, on jest wska�nikiem na
	 * to, co states. Dlatego to definitywnie musi by� tym samym.
	 */
	ASSERT(cur == fa->transt[sti][0]);

	/*
	 * Teraz nasze s�owo zosta�o sprawdzone w przedziale
	 * 0..fa->nalpha. Jednak tabela przej�� jako pierwsz� kolumn� ma
	 * sam stan, wi�c +1 rozwi�zuje problem.
	 */
	curnew = fa->transt[sti][word + 1];

	/* 
	 * Hm.. Czy stan nie jest aby "osamotniony" je�eli chodzi o
	 * przej�cia?
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
 * Wypisz automat.
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
 * Wypisz w dot.
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

	/* Ustawianie atrybut�w dla grafu */
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
 * Wypisz do pliku z rozszerzeniem dot. kroku symulacji.
 */
void
FA_dump_dot(struct FA *fa, const char *ofname)
{

	FA_dump_dot_fn(fa, ofname, FA_INSERT_EXT);
}

/*
 * Wypisz do jednego pliku.
 */
void
FA_dump_dot_one(struct FA *fa, const char *ofname)
{

	FA_dump_dot_fn(fa, ofname, FA_INSERT_NOTHING);
}

/*
 * Restart automatu do pocz�tkowej postaci.
 */
void
FA_restart(struct FA *fa)
{
	struct FA_State *st;
	int i;

	FA_ASSERT(fa);

	/*
	 * Usu� znacznik "stan obecny" ze wszystkich stan�w.
	 */
	for (i = 0; i < fa->nstates; i++) {
		st = &fa->states[i];
		ASSERT(st != NULL);
		st->flag &= ~FA_STATE_CURR;
	}

	/* Zaznacz stan pierwszy jako obecny */
	st = &fa->states[0];
	st->flag = FA_STATE_CURR;

	/* W stanie pierwszym wygeneruj strza�k� w spos�b poprawny */
	fa->trans_from = fa->state0;
	fa->trans_to = st;
}

/*
 * Czy jeste�my w stanie ostatnim?
 */
int
FA_final(struct FA *fa)
{
	struct FA_State *st;
	int i;

	FA_ASSERT(fa);
	for (i = 0; i < fa->nstates; i++) {
		st = &fa->states[i];
		/* Czy stan obecny jest ostatecznym? */
		if (st->flag & FA_STATE_LAST && st->flag & FA_STATE_CURR)
			return (1);
	}
	return (0);
}
