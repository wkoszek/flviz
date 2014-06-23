#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QtGui/QApplication>
#include "mainwindow.h"

#include "FLViz.h"

#ifndef EX_USAGE
#define EX_USAGE	64
#endif

/* Debugging */
extern int flag_D;

/* Tutaj spodziewamy siê komunikatów o b³êdach */
extern const char *errmsg[];

/* Jak u¿ywaæ tego programu */
static void 
usage(const char *prog)
{

	fprintf(stderr, "%s [-d] [-D] [-h] [-s] [-o <file>] [-v] [-g] <file>\n", prog);
	fprintf(stderr, "-d\twyrzuæ format zrenderowany\n");
	fprintf(stderr, "-D\tw³¹cz debugging\n");
	fprintf(stderr, "-h\thelp (pomoc)\n");
	fprintf(stderr, "-s\ttryb symulacji\n");
	fprintf(stderr, "-o <file>\tustaw plik wyjœciowy\n");
	fprintf(stderr, "-v\tszczegó³owe wypisywanie komunikatów\n");
	fprintf(stderr, "-g\tnie uruchamiaj interfejsu graficznego\n");
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

	/* Podanie g udpala interfejs tekstowy */
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
	 * To samo co w graficznym interfejsie.
	 * Tworzymy graf i w zale¿noœci od trybu, wyrzucamy
	 * go w dogodnej formie
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

	/* Tryb symulacji */
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
