#include <QApplication>
#include "DbTelegramExport.h"
#include "TelegramJacket.h"
#include <qtimer.h>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	TelegramJacket tgBotya;

	return a.exec();
}