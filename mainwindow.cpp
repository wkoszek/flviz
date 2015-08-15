#include <QPushButton>
#include <QFileDialog>
#include <QByteArray>
#include <QTextCodec>
#include <QDir>

#include <stdio.h>
#include <io.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FLViz.h"
#include "flvizgv.h"

/*
 * G��wne okienko.
 * Podpinamy przyciski otw�rz/zako�cz
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , fa(NULL)
    , stbuttons(NULL)
    , stbuttons_num(-1)
{
	ui->setupUi(this);
	connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(buttonOPEN()));
	connect(ui->buttonExit, SIGNAL(clicked()), this, SLOT(close()));
        connect(ui->buttonRestart, SIGNAL(clicked()), this, SLOT(buttonRESTART()));
        ui->buttonRestart->setVisible(false);
        setWindowTitle(tr("FLViz 2015.08.14 (c) Wojciech A. Koszek <wojciech@koszek.com>"));
}

/*
 * Destruktor.
 */
MainWindow::~MainWindow()
{
	delete ui;
}

/*
 * Klikni�cie na "Otw�rz"
 */
void
MainWindow::buttonOPEN(void)
{
	QString fileName;
	char buf[4096];
	QString *curpath = NULL;
	int error = 0;
	FILE *fp;
	QPushButton *b = NULL;
	int i;

	/* Zaczynamy bez wiadomo�ci w pasku statusu */
	statusBar()->showMessage(tr(""));

	/* Nastepnie obs�ugujemy menu dot. wyboru pliku */
	curpath = new QString(getcwd(buf, sizeof(buf)));
	fileName = QFileDialog::getOpenFileName(this, tr("Open SVG File"),
	*curpath
	, "FLV files (*.flv *.txt)"
	);

	/* Je�eli nie ma nazwy pliku... */
	if (fileName.isEmpty()) {
		statusBar()->showMessage(tr("Nie podano nazwy pliku."));
		return;
	}

	/*
	 *  Wcze�niej wykorzystywali�my jaki� automat -- prosimy o zwolnienie
	 */
        if (fa != NULL) {
		FA_free(fa);
                fa = NULL;
        }
	/*
	 * Teraz otwieramy plik, no chyba �e wyst�pi� b��d
	 */
	fp = fopen(fileName.toStdString().c_str(), "r");
	if (fp == NULL)
		return;
	/*
	 * Tworzymy struktur� automatu.
	 */
	error = FA_create(&fa, fp);
	if (error != 0) {
		/*
		 * W razie b��du tworzymy komunikat, kt�ry jest bardzo
		 * szczeg�owy i wrzucamy go do paska statusu.
		 */
		memset(buf, 0, sizeof(buf));
		(void)snprintf(buf, sizeof(buf) - 1, "ERROR%d: ", -error);
		QByteArray *encodedString = new QByteArray(errmsg[-error]);
		QTextCodec *codec = QTextCodec::codecForName("ISO 8859-2");
		QString errstr = codec->toUnicode(*encodedString);
		statusBar()->showMessage(buf + errstr);
		return;
	}

	/*
	 * Nast�pnie w zale�no�ci od tego, ile liter alfabetu mamy, tworzymy
	 * przyciski dynamicznie. Jednak najpierw upewniamy si�, czy aby jaki�
	 * automat przed nami nie istnia�. Je�eli tak, to zwalniamy stare
	 * przyciski.
	 */
        if (stbuttons != NULL && stbuttons_num != -1) {
		for (i = 0; i < stbuttons_num; i++) {
			b = stbuttons[i];
			ui->verticalLayout->removeWidget(b);
			b->setVisible(false);
			disconnect(b, SIGNAL(clicked()), this, SLOT(faChange()));
			delete b;
		}
		delete [] stbuttons;
                stbuttons_num = -1;
	}
	/* I teraz re-kreacja */
	stbuttons_num = fa->nalpha;
	stbuttons = new QPushButton *[stbuttons_num];
	for (i = 0; i < stbuttons_num; i++) {
		b = stbuttons[i] = new QPushButton(ui->centralWidget);
                b->setText(QString(fa->alpha[i].word));
		/* 
		 * Rejestrujemy filtr zdarze�, bo wsz�dzie korzystamy z
		 * faChange()
		 */
		b->installEventFilter(this);
		connect(b, SIGNAL(clicked()), this, SLOT(faChange()));
		b->setVisible(true);
                ui->verticalLayout->insertWidget(WIDGETS_AFTER_SIMLABEL + i,b);
	}
	faChange();
        statusBar()->showMessage(fileName);
}

void
MainWindow::buttonRESTART(void)
{

	FA_restart(fa);
	ui->buttonRestart->setVisible(false);
	faChange();
}

/*
 * Uaktualniamy okno wyrenderowanym obrazkiem grafu.
 */
void
MainWindow::faChange(void)
{

	QString tmppath = QDir::tempPath() + QString("/out.out");
#if VERBOSE_DEBUGGING
	statusBar()->showMessage(tmppath);
#endif
	FA_dump_dot_one(fa, tmppath.toStdString().c_str());
	ui->grv->openFile(tmppath);

	if (FA_final(fa)) {
		ui->buttonRestart->setVisible(true);
	}
}

/*
 * To wymaga komentarza..:
 * eventFilter umo�liwia u�ycie jednej funkcji jako procedury obs�ugi wielu
 * przycisk�w -- w czasie rejestracji dynamicznych struktur QPushButton nie
 * jestesmy po prostu w stanie przekaza� tam za kazdym razem innej funkcji
 * ob�sugi przycisku. St�d eventFilter filtruje klikni�cia w przycisk,
 * odtwarza, kt�r� liter� alfabetu dany przycisk reprezentuje oraz uaktualnia
 * obrazek grafu.
 */
bool
MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	int i;
	
	if (stbuttons == NULL || stbuttons_num <= 0)
		return (false);
	for (i = 0; i < stbuttons_num; i++) {
		if (obj == stbuttons[i]) {
			if (event->type() == QEvent::MouseButtonPress) {
				faChange();
				FA_trans(fa, i);
			}
		}
	}
	return (false);
}
