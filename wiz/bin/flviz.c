#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//#include "wiz\include\graphviz\cgraph.h"
#include "..\..\wiz\include\graphviz\gvc.h"
#include "..\..\wiz\include\graphviz\graph.h"

int
main(int argc, char **argv)
{
	GVC_t *gvc = NULL;
	graph_t *g = NULL;
	FILE *fp = NULL;
	Agnode_t *n, *m;
	Agedge_t *e;
	Agsym_t *a;
	
	gvc = gvContext();
	gvParseArgs(gvc, argc, argv);
	assert(gvc != NULL);

	g = agopen("g", AGDIGRAPHSTRICT);
	n = agnode(g, "n");
	m = agnode(g, "m");
	e = agedge(g, n, m);

	/* Ustawianie atrybutów dla grafu */
	agraphattr(g, "size", "5,5");

	/* Set an attribute - in this case one that affects the visible
	 * renderi */
	agsafeset(n, "color", "red", "");
	assert(g != NULL);

	/* Compute a layout using layout engine from command line args */
	gvLayoutJobs(gvc, g);

	/* Write the graph according to -T and -o options */
	gvRenderJobs(gvc, g);

//	gvLayout(gvc, g, "dot");
//	gvRender(gvc, g, "dot", stdout);
	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);
	exit(1);
}
