
#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"
#include "TelegramJacket.h"

#include <qtimer.h>


int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	TelegramJacket tgBotya;

	return a.exec();
	
}

