#ifndef FLVIZGV_H
#define FLVIZGV_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QWheelEvent;
class QPaintEvent;
class QFile;
class QString;
QT_END_NAMESPACE

class FLVizGv : public QGraphicsView
{
	Q_OBJECT

public:
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
