
#include "DbTelegramExport.h"


DbTelegramExport::DbTelegramExport(QObject* parent) : QObject(parent)
{
	connectDataBase();
}

DbTelegramExport::~DbTelegramExport()
{}

void DbTelegramExport::connectDataBase()
{
	QSqlDatabase mw_db = QSqlDatabase::addDatabase("QODBC"); // ��� ������� ODBC � Windows ���������� ������� ���������������� DNS � ����������������� �������. ������ �� ����� ��������.

	// mw_db.setHostName(myParamForSmtp->hostName); // ���� ��� ����� ��
	mw_db.setDatabaseName("DBEG"); // ��������� ��� ����������������� DNS ������� ��� ������ � ������� �����.
	mw_db.setUserName("solexp");
	mw_db.setPassword("RootToor#");

	if (!mw_db.open()) // ��������� ��. ���� �� ��������� �� ����� false
	{


		mw_db.lastError().databaseText(); // ���� ���-�� ����� �� ��� �� ����� ��� � ����������
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

	QString timeInQuery = curDate.toString("yyyy-MM-dd"); // ������������� ������ ���� ��� ��� � ��.

	queryString = "select ID_MeterInfo from MeterInfo where SN = '" + any + "'"; // ����������� ��������� ID �� ������ �������
	query.exec(queryString);

	query.next();

	iD = query.value(0).toInt();

	queryString = "select ID_Point from MeterMountHist where ID_MeterInfo = '" + any.setNum(iD) + "'"; // �������� ID �� ��������

	query.exec(queryString);
	query.next();
	iD = query.value(0).toInt();

	queryString = "select * from dbo.PointParams where ID_Point = '" + any.setNum(iD) + "' and ID_Param = '4'"; // �������� ID ��������� �������� ������� ��������
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

	queryString = "select ID_Parent from NDIETable where ID_PP = '" + any.setNum(iD) + "'"; // �������� ID ��� ������������ ���������� GUID
	query.exec(queryString);
	query.next();
	iD = query.value(0).toInt();

	queryString = "select Code from NDIETable where ID_DIE = '" + any.setNum(iD) + "'"; // �������� GUID
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
