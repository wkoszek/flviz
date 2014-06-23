#ifndef FLVIZGV_H
#define FLVIZGV_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QWheelEvent;
class QPaintEvent;
class QFile;
class QString;
QT_END_NAMESPACE

/*
 * Dziedziczymy po QGraphicsView, ¿eby mo¿liwe by³o wykorzystanie 
 * rzeczy typu SVG
 */
class FLVizGv : public QGraphicsView
{
	Q_OBJECT

public:
	/*
	 * Metody wyjaœnione w pliku .cpp
	 */
	FLVizGv(QWidget *parent = 0);
	void rescale(void);

	void openFile(const QString &fname);
protected:
	void wheelEvent(QWheelEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	QGraphicsItem *m_svgItem;
};

#endif // FLVIZGV_H
