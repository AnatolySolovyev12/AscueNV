#include <QApplication>
#include "TelegramJacket.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	TelegramJacket tgBotya;
	//tgBotya.setWindowIcon(QIcon("icon.png"));
	//tgBotya.setWindowTitle("AscueNV by Solovev");

	return a.exec();
}