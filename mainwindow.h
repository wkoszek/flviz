#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "FLViz.h"

#define	WIDGETS_AFTER_SIMLABEL	4

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void buttonOPEN(void);
        void buttonRESTART(void);
	void faChange(void);

protected:
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	Ui::MainWindow *ui;
	struct FA *fa;
        QPushButton **stbuttons;
        int stbuttons_num;
};

#endif // MAINWINDOW_H
