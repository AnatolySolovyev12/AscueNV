#include "VectorImage.h"

VectorImage::VectorImage(QObject* parent)
	: QObject(parent)
{

}

VectorImage::~VectorImage()
{}



void VectorImage::generalFunc(QString any)
{

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

	double angleUIa = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	double UIlength = 100; // Длина второй линии

	// Угол для второй линии в градусах (например, 45 градусов относительно первой)
	double radAngleUIa = qDegreesToRadians(4);

	double totalAngleUIa = angleUIa + radAngleUIa; // Совокупный угол

	// Вычисляем конечные координаты второй линии
	QPointF UIaLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIa), startPoint.y() + UIlength * sin(totalAngleUIa));

	painter.setPen(QPen(QPen(QColor(244, 189, 0)))); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, UIaLineEndPoint);

	//Uab

	double angleUa = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	double length = 124; // Длина второй линии

	// Угол для второй линии в градусах (например, 45 градусов относительно первой)
	double radAngleUb = qDegreesToRadians(119);

	double totalAngleAB = angleUa + radAngleUb; // Совокупный угол

	// Вычисляем конечные координаты второй линии
	QPointF UabLineEndPoint(startPoint.x() + length * cos(totalAngleAB), startPoint.y() + length * sin(totalAngleAB));

	painter.setPen(QPen(Qt::green)); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, UabLineEndPoint);

	//UIb

	endPoint = UabLineEndPoint;

	double angleUIb = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	UIlength = 100; // Длина второй линии

	// Угол для второй линии в градусах (например, 45 градусов относительно первой)
	double radAngleUIb = qDegreesToRadians(351);

	double totalAngleUIb = angleUIb + radAngleUIb; // Совокупный угол

	// Вычисляем конечные координаты второй линии
	QPointF UIbLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIb), startPoint.y() + UIlength * sin(totalAngleUIb));

	painter.setPen(QPen(QPen(QColor(55, 189, 55)))); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, UIbLineEndPoint);

	//Ubc

	endPoint = UabLineEndPoint;

	double angleUb = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	length = 124; // Длина второй линии

	// Угол для второй линии в градусах (например, 45 градусов относительно первой)
	double radAngleUc = qDegreesToRadians(124);

	double totalAngleBC = angleUb + radAngleUc; // Совокупный угол

	// Вычисляем конечные координаты второй линии
	QPointF UbcLineEndPoint(startPoint.x() + length * cos(totalAngleBC), startPoint.y() + length * sin(totalAngleBC));

	painter.setPen(QPen(Qt::red)); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, UbcLineEndPoint);

	//UIc

	endPoint = UbcLineEndPoint;

	double angleUIc = atan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x());

	UIlength = 100; // Длина второй линии

	// Угол для второй линии в градусах (например, 45 градусов относительно первой)
	double radAngleUIc = qDegreesToRadians(35);

	double totalAngleUIc = angleUIc + radAngleUIc; // Совокупный угол

	// Вычисляем конечные координаты второй линии
	QPointF UIcLineEndPoint(startPoint.x() + UIlength * cos(totalAngleUIc), startPoint.y() + UIlength * sin(totalAngleUIc));

	painter.setPen(QPen(QPen(QColor(193, 23, 23)))); // присваиваем объект пера с типом стиля кисти по умолчанию Qt::SolidPattern

	painter.drawLine(startPoint, UIcLineEndPoint);

	// Закрываем QPainter
	painter.end();

	// Сохраняем изменённое изображение в файл
	image.save("mod_vectorP.png");

	return;
}

