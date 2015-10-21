#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QApplication>
#include "mainwindow.h"

#include "FLViz.h"

#ifndef EX_USAGE
#define EX_USAGE	64
#endif

/* Debugging */
extern int flag_D;

/* Here we expect error messages to show up */
extern const char *errmsg[];

/* How to use this program */
static void 
usage(const char *prog)
{

	fprintf(stderr, "%s [-d] [-D] [-h] [-s] [-o <file>] [-v] [-g] <file>\n", prog);
	fprintf(stderr, "-d\toutput rendered format\n");
	fprintf(stderr, "-D\tenable debugging\n");
	fprintf(stderr, "-h\thelp\n");
	fprintf(stderr, "-s\tsymulation mode\n");
	fprintf(stderr, "-o <file>\tset output file\n");
	fprintf(stderr, "-v\tverbose output\n");
	fprintf(stderr, "-g\tdon't start GUI\n");
	exit(EX_USAGE);
}


/*
 * FLViz
 */
int
main(int argc, char **argv)
{
	QApplication *a;
	struct FA *fa;
	FILE *fp;
	int o;
	int flag_v = 0;
	int flag_d = 0;
	char *prog = argv[0];
	char *in_fn = NULL;
	int error = 0;
	int flag_s = 0;
	int flag_g = 0;
	int i;
	int word;
	char *ofname = NULL;

	while ((o = getopt(argc, argv, "dDghosv")) != -1)
		switch (o) {
		case 'd':
			flag_d++;
			break;
		case 'D':
			flag_D++;
			break;
		case 'h':
			usage(argv[0]);
			break;
		case 's':
			flag_s++;
			break;
		case 'o':
			ofname = optarg;
			break;
		case 'v':
			flag_v++;
			break;
		case 'g':
			flag_g++;
			break;
		default:
			fprintf(stderr, "Unknown option\n");
			exit(EX_USAGE);
		}

	argc -= optind;
	argv += optind;

	if (!flag_g) {
		a = new QApplication(argc, argv);
		MainWindow w;
		w.show();
		error = a->exec();
		return (error);
	}

	if (argc == 0)
		usage(prog);

	/* 
	 * Same as in GUI: make a graph and depending on the mode, we
	 * S generate it in the convenient form.
	 */
	in_fn = argv[0];
	fp = fopen(in_fn, "r");
	ASSERT(fp != NULL && "Couldn't open file");

	error = FA_create(&fa, fp);
	if (error != 0) {
		printf("Error = %d, MSG: %s\n", error, errmsg[-error]);
		exit(1);
	}
	FA_ASSERT(fa);

	if (ofname == NULL)
		ofname = "outfile.FLViz";

	/* Simulation mode */
	if (flag_s) {
		if (flag_d)
			FA_dump_dot(fa, ofname);
		for (;;) {
			FA_dump(fa);
			printf("-- Podaj indeks s³owa automatu --\n");
			i = scanf("%d", &word);
			if (i != 1)
				break;
			FA_trans(fa, word);
			if (flag_d)
				FA_dump_dot(fa, ofname);
		}
		exit(EXIT_SUCCESS);
	}
	if (flag_d) {
		FA_dump_dot(fa, ofname);
		exit(EXIT_SUCCESS);
	}
	FA_dump(fa);
	FA_free(fa);

	exit(EXIT_SUCCESS);
}
