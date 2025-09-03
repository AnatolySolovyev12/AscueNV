#include <QApplication>
#include "TelegramJacket.h"
#include <qdatetime.h>

QTimer* keepAliveTimer = new QTimer();

TelegramJacket * tgBotya = nullptr;

void writeRessurectionDateTime()
{
	QString filename = QCoreApplication::applicationDirPath() + "\\warnings.txt";
	QFile file(filename);

	// ��������� ���� � ������ "������ ��� ������ � ���������� ��� ����������"
	if (file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file); // ����� ������������ ������ ���������� � ����
		out << QDate::currentDate().toString() + " - " + QTime::currentTime().toString() << Qt::endl;
	}
	else
	{
		qWarning("Could not open file");
	}

	file.close();
}

int main(int argc, char* argv[])
{
	QObject::connect(keepAliveTimer, &QTimer::timeout, []() { // watchdogs timer

		delete tgBotya;
		tgBotya = nullptr;
		tgBotya = new TelegramJacket();
		qDebug() << "Resurrection";
		writeRessurectionDateTime();
		});

	keepAliveTimer->setInterval(300000);

	QApplication a(argc, argv);

	keepAliveTimer->start();

	tgBotya = new TelegramJacket();

	QObject::connect(tgBotya, &TelegramJacket::signalForBreakResurrection, []() {
		keepAliveTimer->start();
		});

	return a.exec();
}