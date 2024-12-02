
#include "DbTelegramExport.h"


DbTelegramExport::DbTelegramExport(QObject* parent) : QObject(parent)
{
	connectDataBase();
}

DbTelegramExport::~DbTelegramExport()
{}

void DbTelegramExport::connectDataBase()
{
	QSqlDatabase mw_db = QSqlDatabase::addDatabase("QODBC"); // Для раблоты ODBC в Windows необходимо задвать пользовательский DNS в администрировании системы. Иначен не будет работать.

	// mw_db.setHostName(myParamForSmtp->hostName); // хост где лежит БД
	mw_db.setDatabaseName("DBEG"); // указываем имя пользовательского DNS который был создан в системе ранее.
	mw_db.setUserName("solexp");
	mw_db.setPassword("RootToor#");

	if (!mw_db.open()) // открываем БД. Если не открывает то вернёт false
	{


		mw_db.lastError().databaseText(); // если что-то пойдёт не так то пишем это в переменные
		mw_db.lastError().driverText();

		qDebug() << "Cannot open database: " << mw_db.lastError();

	}
}

void DbTelegramExport::queryDbResult(QString any)
{
	QSqlQuery query;
	QString queryString;

	int iD = 0;

	int guidId;

	QDate curDate = QDate::currentDate();

	curDate = curDate.addDays(-1);

	QString timeInQuery = curDate.toString("yyyy-MM-dd"); // Разворачиваем формат даты так как в БД.

	queryString = "select ID_MeterInfo from MeterInfo where SN = '" + any + "'"; // запрашиваем первичный ID по номеру прибора
	query.exec(queryString);

	query.next();

	iD = query.value(0).toInt();

	queryString = "select ID_Point from MeterMountHist where ID_MeterInfo = '" + any.setNum(iD) + "'"; // получаем ID из счётчика

	query.exec(queryString);
	query.next();
	iD = query.value(0).toInt();

	queryString = "select * from dbo.PointParams where ID_Point = '" + any.setNum(iD) + "' and ID_Param = '4'"; // получаем ID параметра активной энергии счётчика
	query.exec(queryString);
	query.next();

	iD = query.value(0).toInt();


	queryString = "select Val from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and DT = '" + timeInQuery + " 22:00:00:000' and N_Rate = '1'";

	query.exec(queryString);
	query.next();
	day = query.value(0).toString();


	queryString = "select Val from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and DT = '" + timeInQuery + " 22:00:00:000' and N_Rate = '2'";

	query.exec(queryString);
	query.next();
	night = query.value(0).toString();

	queryString = "select ID_Parent from NDIETable where ID_PP = '" + any.setNum(iD) + "'"; // получаем ID для последующего получаения GUID
	query.exec(queryString);
	query.next();
	iD = query.value(0).toInt();

	queryString = "select Code from NDIETable where ID_DIE = '" + any.setNum(iD) + "'"; // получаем GUID
	query.exec(queryString);
	query.next();
	guid = query.value(0).toString();

	mw_db.close();
}


void DbTelegramExport::setAny(QString anyString)
{
	anyTelegramString = anyString;
}


QString DbTelegramExport::getAny()
{
	anyTelegramString = day + "   " + night + "   " + guid;
	return anyTelegramString;
}
