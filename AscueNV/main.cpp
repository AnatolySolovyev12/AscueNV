
#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"
#include "TelegramJacket.h"

#include <qtimer.h>


int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	TelegramJacket tgBotya;
	//qDebug() << QString("7ea01341022152092ae6e700c40141000301ddd77e").length(); // 42
	return a.exec();
	
}

