#include "VectorImage.h"


VectorImage::VectorImage(QObject* parent)
	: QObject(parent)
{

}

VectorImage::~VectorImage()
{}

void VectorImage::generalFunc(QString any)
{

	//QString general = "104000052160\n\nSerial 104000052160\nSoftVer 1.6.19\nfUab 119 \nfUac 117 \nfUbc 124 \nfUIa 1 \nfUIb 0 \nfUIc 1 ";

	QString general = any;
	QString temporary = general.sliced(49);
	QList <QString> valuesList;

	QString test;
	general = "";

	bool space = false;

	for (auto& val : temporary)
	{
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
				valuesList.push_back(test);
				test = "";
				continue;
			}
		}
		if (space)
		{
			test += val;
			continue;
		}
	}

	//QImage image("C:/Users/PavlovAA/source/repos/TestProject/TestProject/x64/Release/vectorP.png");

	QImage image("vectorP.png");

	QPainter painter(&image); // класс общей рисовальни для создания графических объектов

	//Ua

	QPointF startPoint(133, 150);  // x y центр векторки

	QPointF endPoint(133, 26);

	QPointF IendPoint(133, 26);

	painter.setPen(QPen(QColor(247, 227, 60))); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, endPoint);

	//UIa

	printf("UIa\n"); ///////////////////////

	double angleUIa = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	double UIlength = 100;

	
	double radAngleUIa = qDegreesToRadians(valuesList[3].toInt());

	double totalAngleUIa = angleUIa + radAngleUIa; // Совокупный угол


	QPointF UIaLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIa), startPoint.y() + UIlength * sin(totalAngleUIa));

	painter.setPen(QPen(QPen(QColor(244, 189, 0)))); 

	painter.drawLine(startPoint, UIaLineEndPoint);

	//Uab

	double angleUa = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	double length = 124; 

	
	double radAngleUb = qDegreesToRadians(valuesList[0].toInt());

	double totalAngleAB = angleUa + radAngleUb; 

	
	QPointF UabLineEndPoint(startPoint.x() + length * cos(totalAngleAB), startPoint.y() + length * sin(totalAngleAB));

	painter.setPen(QPen(Qt::green));

	painter.drawLine(startPoint, UabLineEndPoint);

	//UIb

	endPoint = UabLineEndPoint;

	double angleUIb = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	UIlength = 100; 

	
	double radAngleUIb = qDegreesToRadians(valuesList[4].toInt());

	double totalAngleUIb = angleUIb + radAngleUIb; 

	
	QPointF UIbLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIb), startPoint.y() + UIlength * sin(totalAngleUIb));

	painter.setPen(QPen(QPen(QColor(55, 189, 55)))); 

	painter.drawLine(startPoint, UIbLineEndPoint);

	//Ubc

	endPoint = UabLineEndPoint;

	double angleUb = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	length = 124; 


	double radAngleUc = qDegreesToRadians(valuesList[2].toInt());

	double totalAngleBC = angleUb + radAngleUc; 

	
	QPointF UbcLineEndPoint(startPoint.x() + length * cos(totalAngleBC), startPoint.y() + length * sin(totalAngleBC));

	painter.setPen(QPen(Qt::red)); 

	painter.drawLine(startPoint, UbcLineEndPoint);

	//UIc
	printf("UIc\n");
	endPoint = UbcLineEndPoint;

	double angleUIc = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	UIlength = 100; 


	double radAngleUIc = qDegreesToRadians(valuesList[5].toInt());

	double totalAngleUIc = angleUIc + radAngleUIc;


	QPointF UIcLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIc), startPoint.y() + UIlength * sin(totalAngleUIc));

	painter.setPen(QPen(QPen(QColor(193, 23, 23))));

	painter.drawLine(startPoint, UIcLineEndPoint);

	// Закрываем QPainter
	painter.end();

	// Сохраняем изменённое изображение в файл
	image.save("mod_vectorP.png");

	emit messageReceived();

	return;
}

