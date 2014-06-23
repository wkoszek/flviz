#include "flvizgv.h"

#include <QFile>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QPaintEvent>
#include <qmath.h>

/*
 * Konstruktor tworz¹cy obiekt graficzny, w którym w przysz³oœci umieœcimy
 * obrazek grafu; teraz rysujemy w nim szachownicê.
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
 * Otwieranie nowego pliku.
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
 * Obs³uga ¿¹danie o narysowanie obrazka.
 */
void
FLVizGv::paintEvent(QPaintEvent *event)
{

	QGraphicsView::paintEvent(event);
}

/*
 * Skalowanie obrazka
 */
void
FLVizGv::wheelEvent(QWheelEvent *event)
{
#if 0
	qreal factor = event->delta() / 240.0;
	disp_scale += factor;
	factor += 1.0;
	scale(factor, factor);
#endif
	qreal factor = qPow(1.2, event->delta() / 240.0);
	scale(factor, factor);
	event->accept();
}
