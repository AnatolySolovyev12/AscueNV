#include "VectorImage.h"
#include <QTextStream.h>

VectorImage::VectorImage(QObject* parent)
	: QObject(parent)
{
}

VectorImage::~VectorImage()
{
}

void VectorImage::generalFunc(QString any)
{
	QTextStream outerr(stderr);

	QString general = any;
	QString temporary = general.sliced(88);

	QList <QString> valuesList;

	QString bufferSign;
	general = "";

	bool space = false;

	for (auto& val : temporary)
	{
		if (val == '\n') continue;

		if (val.isSpace())
		{
			if (!space)
			{
				space = true;
				continue;
			}

			if (space)
			{
				space = false;
				valuesList.push_back(bufferSign);
				bufferSign = "";
				continue;
			}
		}
		if (space)
		{
			bufferSign += val;
			continue;
		}
	}

	if(valuesList.length() < 6)
	{
		outerr << "Error. Length of valueList is less then need" << Qt::endl;
		return;
	}


	QImage image;

	if (!image.load("vectorP.png"))
	{
		outerr << "Error. Can't load vectorP.png" << Qt::endl;
		return;
	}

	QPainter painter(&image); // класс общей рисовальни для создания графических объектов

	//Ua

	QPointF startPoint(472, 455);  // x y центр векторки

	QPointF endPoint(471, 30);

	//QPointF IendPoint(133, 26);

	QPen vectorPen;

	vectorPen.setColor(QColor(254, 251, 107));

	vectorPen.setWidth(8);
	
	painter.setPen(vectorPen); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	//painter.setPen(QPen(QColor(247, 227, 60))); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, endPoint);

	//UIa

	double angleUIa = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	double UIlength = 350;

	
	double radAngleUIa = qDegreesToRadians(valuesList[3].toInt());

	double totalAngleUIa = angleUIa + radAngleUIa; // Совокупный угол


	QPointF UIaLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIa), startPoint.y() + UIlength * sin(totalAngleUIa));

	//painter.setPen(QPen(QPen(QColor(244, 189, 0)))); 
	vectorPen.setColor(QColor(244, 189, 0));

	painter.setPen(vectorPen);

	painter.drawLine(startPoint, UIaLineEndPoint);

	//Uab

	double angleUa = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	double length = 425; 

	
	double radAngleUb = qDegreesToRadians(valuesList[0].toInt());

	double totalAngleAB = angleUa + radAngleUb; 

	
	QPointF UabLineEndPoint(startPoint.x() + length * cos(totalAngleAB), startPoint.y() + length * sin(totalAngleAB));

	//painter.setPen(QPen(Qt::green));

	vectorPen.setColor(QColor(155, 252, 37));

	painter.setPen(vectorPen);

	painter.drawLine(startPoint, UabLineEndPoint);

	//UIb

	endPoint = UabLineEndPoint;

	double angleUIb = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	UIlength = 350; 

	
	double radAngleUIb = qDegreesToRadians(valuesList[4].toInt());

	double totalAngleUIb = angleUIb + radAngleUIb; 

	QPointF UIbLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIb), startPoint.y() + UIlength * sin(totalAngleUIb));

	//painter.setPen(QPen(QPen(QColor(55, 189, 55)))); 
	vectorPen.setColor(QColor(55, 189, 55));

	painter.setPen(vectorPen);

	painter.drawLine(startPoint, UIbLineEndPoint);

	//Ubc

	endPoint = UabLineEndPoint;

	double angleUb = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	length = 425; 

	double radAngleUc = qDegreesToRadians(valuesList[2].toInt());

	double totalAngleBC = angleUb + radAngleUc; 

	QPointF UbcLineEndPoint(startPoint.x() + length * cos(totalAngleBC), startPoint.y() + length * sin(totalAngleBC));

	//painter.setPen(QPen(Qt::red)); 

	vectorPen.setColor(QColor(245, 145, 139));

	painter.setPen(vectorPen);

	painter.drawLine(startPoint, UbcLineEndPoint);

	//UIc

	endPoint = UbcLineEndPoint;

	double angleUIc = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	UIlength = 350; 

	double radAngleUIc = qDegreesToRadians(valuesList[5].toInt());

	double totalAngleUIc = angleUIc + radAngleUIc;

	QPointF UIcLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIc), startPoint.y() + UIlength * sin(totalAngleUIc));

	//painter.setPen(QPen(QPen(QColor(193, 23, 23))));

	vectorPen.setColor(QColor(240, 40, 30));

	painter.setPen(vectorPen);

	painter.drawLine(startPoint, UIcLineEndPoint);

	// Закрываем QPainter
	painter.end();

	// Сохраняем изменённое изображение в файл
	if (!image.save(QString::number(key) + "_vectorP.png"))
	{
		outerr << "Error. Can't save " << QString::number(key) + "_vectorP.png" << Qt::endl;
		return;
	}

	emit messageReceived();

	return;
}


void VectorImage::setKey(int64_t any)
{
	key = any;
}