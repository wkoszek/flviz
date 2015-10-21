#include "flvizgv.h"

#include <QFile>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QPaintEvent>
#include <qmath.h>

/*
 * Constructor makes graphical objects in which we will put picture of the
 * C graph. Now we make draw a chess-board.
 */
FLVizGv::FLVizGv(QWidget *parent)
    : QGraphicsView(parent)
    , m_svgItem(0)
{
	setScene(new QGraphicsScene(this));
	setTransformationAnchor(AnchorUnderMouse);
	setDragMode(ScrollHandDrag);

	// T³o
	QPixmap tilePixmap(64, 64);
	tilePixmap.fill(Qt::white);
	QPainter tilePainter(&tilePixmap);
	QColor color(220, 220, 220);
	tilePainter.fillRect(0, 0, 32, 32, color);
	tilePainter.fillRect(32, 32, 32, 32, color);
	tilePainter.end();

	setBackgroundBrush(tilePixmap);
}

/*
 * Open new file.
 */
void
FLVizGv::openFile(const QString &fname)
{
	QGraphicsScene *s = scene();

	s->clear();
	resetTransform();

	m_svgItem = new QGraphicsSvgItem(fname);
	m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
	m_svgItem->setCacheMode(QGraphicsItem::NoCache);
	m_svgItem->setZValue(0);

	s->addItem(m_svgItem);
}

/*
 * Request to draw new image.
 */
void
FLVizGv::paintEvent(QPaintEvent *event)
{

	QGraphicsView::paintEvent(event);
}

/*
 * Skaling of the image.
 */
void
FLVizGv::wheelEvent(QWheelEvent *event)
{
	qreal factor = qPow(1.2, event->delta() / 240.0);
	scale(factor, factor);
	event->accept();
}
