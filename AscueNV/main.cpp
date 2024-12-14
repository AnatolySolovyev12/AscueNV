
#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"
#include "TelegramJacket.h"

#include <qtimer.h>


int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	TelegramJacket tgBotya;



	/*
signals:
	void status(const QString&);

emit status(tr("dbreconnet"));

connect(mySMTP, SIGNAL(status(QString)), this, SLOT(MessegeAboutMailSend(QString))); // прям в методе после создания объекта

private slots:
void MessegeAboutReconnectDb(QString);


connect(myParamForSmtp, SIGNAL(status(QString)), this, SLOT(MessegeAboutReconnectDb(QString)));

*/



	return a.exec();
	
}

