
#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"
#include "TelegramJacket.h"

#include <qtimer.h>


int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	TelegramJacket tgBotya;
	/*
	QList <QString> numberList{ "101", "102", "103", "104", "/101", "/102", "/103" ,"/104",">101", ">102", ">103" ,">104","_101", "_102", "_103", "_104" };

	QString serialStringForProtocolinTelegram = "400";

	if (numberList.indexOf(serialStringForProtocolinTelegram) >=0)
		qDebug() << numberList.indexOf(serialStringForProtocolinTelegram);
	else
		qDebug() << "not in list";
*/
	return a.exec();
	
}

