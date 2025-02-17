#pragma once

#include <QObject>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QPointF>
#include <cmath>

class VectorImage : public QObject
{
	Q_OBJECT

public:
	VectorImage(QObject* parent);
	~VectorImage();

	void generalFunc(QString any);
	void setKey(int64_t);

signals:
	void messageReceived();

private:

	int64_t key;
};
