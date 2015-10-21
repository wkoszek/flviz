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
        setWindowTitle(
	    tr("FLViz 2015.08.14 (c) Wojciech A. Koszek <wojciech@koszek.com>")
	);
}

MainWindow::~MainWindow()
{
	delete ui;
}

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

	/* Start without info in the status bar */
	statusBar()->showMessage(tr(""));

	/* Serve "choose file" menu */
	curpath = new QString(getcwd(buf, sizeof(buf)));
	fileName = QFileDialog::getOpenFileName(this, tr("Open SVG File"),
	*curpath
	, "FLV files (*.flv *.txt)"
	);

	if (fileName.isEmpty()) {
		statusBar()->showMessage(tr("No file chosen."));
		return;
	}

	/*
	 *  If we used FA before, free it.
	 */
        if (fa != NULL) {
		FA_free(fa);
                fa = NULL;
        }

	/*
	 * Try to open a file now.
	 */
	fp = fopen(fileName.toStdString().c_str(), "r");
	if (fp == NULL)
		return;
	/*
	 * Make FA
	 */
	error = FA_create(&fa, fp);
	if (error != 0) {
		/*
		 * In case of a problem, create a very detailed message and
		 * I report it in the status bar.
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
	 * We make buttons dynamically. If the old FA existed before, we
	 * W free the buttons.
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
	/* Now, re-create */
	stbuttons_num = fa->nalpha;
	stbuttons = new QPushButton *[stbuttons_num];
	for (i = 0; i < stbuttons_num; i++) {
		b = stbuttons[i] = new QPushButton(ui->centralWidget);
                b->setText(QString(fa->alpha[i].word));
		/* 
		 * Register event filter, since we alwaysuse faChange
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
 * Update the window with rendered graph image
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
 * This needs a comment:
 * eventFilter lets us to use 1 procedure to handle many buttons. At the
 * time of registering dynamic QPushButton structures we're not able to pass
 * different fuctions everty time. So this event Filter detects which button
 * has been clicked.
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
