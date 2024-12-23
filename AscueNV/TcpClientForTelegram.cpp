#include "TcpClientForTelegram.h"

#include <QTextStream>


TcpClientForTelegram::TcpClientForTelegram(QString any, QObject* parent) : serialStringForProtocol(any),QObject(parent), socket(new QTcpSocket(this))
{
	//QTextStream in(stdin);
	//QTextStream out(stdout);
	qDebug() << serialStringForProtocol;
	myTimer = new QTimer();
	connect(socket, &QTcpSocket::connected, this, &TcpClientForTelegram::onConnected);
	connect(socket, &QTcpSocket::disconnected, this, &TcpClientForTelegram::onDisconnected);
	connect(socket, &QTcpSocket::readyRead, this, &TcpClientForTelegram::onReadyRead);
	connect(socket, &QTcpSocket::errorOccurred, this, &TcpClientForTelegram::onErrorOccurred);
	connect(myTimer, &QTimer::timeout, this, &TcpClientForTelegram::exchangeFromTimer);

	//out << "Enter IP:" << Qt::endl;
	//in >> ip;

}

TcpClientForTelegram::~TcpClientForTelegram()
{
	if (socket->isOpen()) {
		socket->close();
	}
}

void TcpClientForTelegram::connectToServer(const QString& host, quint16 port)
{
	socket->connectToHost(QHostAddress(host), port);
	qDebug() << "Connect to " + QHostAddress(host).toString();


	/* while (!connectedState)
	 {
		 QTimer::singleShot(3000, [this]() {
			 qDebug() << "Trying connect to host: 10.0.7.116:8888";
			 socket->connectToHost(QHostAddress("10.0.7.116"), 8888);
			 });
	 }*/

}

void TcpClientForTelegram::sendMessage(const QByteArray& message)
{


	if (socket->state() == QTcpSocket::ConnectedState) {


		//	QString hexValue = QString::number(0, 16);
			//QByteArray byteArray = QByteArray::fromHex(hexValue.toUtf8());
			//socket->write(byteArray);

		socket->write(message);
		qDebug() << "TX >> " << message;
	}
	else {
		qDebug() << "Not connected to server.";
	}
}

void TcpClientForTelegram::onConnected()
{
	// connectedState = true;
	qDebug() << "Connected to server.";
	exchange();
}

void TcpClientForTelegram::onDisconnected()
{
	connectedState = false;
	qDebug() << "Disconnected from server.";
}

void TcpClientForTelegram::onReadyRead()
{
	QByteArray data = socket->readAll();
	//QString message = QString::fromUtf8(data);
	// emit messageReceived(message);
	qDebug() << "RX << " << data.toHex();

	if (serialStringForProtocol == "101" || serialStringForProtocol == "103")
	{
		if (counterForResend >= 2 && counterForResend != 16)
		{
			QString temporaryAnswer = data.toHex();
			summAnswer(temporaryAnswer);
		}
	}
	if (serialStringForProtocol == "102" || serialStringForProtocol == "104" || serialStringForProtocol == "106")
	{
		if (counterForResend >= 2 && counterForResend != 30)
		{
			QString temporaryAnswer = data.toHex();
			summAnswer(temporaryAnswer);
		}

	}

	myTimer->stop();
	counterForResend++;
	reTransmitQuery = 0;
	exchange();
}

void TcpClientForTelegram::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Socket error:" << socketError << socket->errorString();
	answerString += socket->errorString() + '.' + " No connection or bad signal";
}

void TcpClientForTelegram::summAnswer(QString& any)
{
	if (serialStringForProtocol == "101" || serialStringForProtocol == "103")
	{
		bool ok;
		bool minus = false;
		QString temporaryAnswer = any.sliced(34);
		temporaryAnswer.chop(6);
		QString frankenshteinString;

		qDebug() << "after sliced and chop = " + temporaryAnswer << '\n';

		if ((temporaryAnswer.toUInt(&ok, 16) > 4200000000) && counterForResend > 5)
		{
			temporaryAnswer = QString("%1")
				.arg(4294967295 - temporaryAnswer.toUInt(&ok, 16));
			minus = true;
		}
		else
		{
			temporaryAnswer = QString("%1")
				.arg(temporaryAnswer.toULongLong(&ok, 16));
		}

		if (counterForResend <= 5)
		{
			if (temporaryAnswer.length() == 6)
			{
				temporaryAnswer.push_front("0");
			}

			if (temporaryAnswer.length() == 5)
			{
				temporaryAnswer.push_front("0");
				temporaryAnswer.push_front("0");
			}

			frankenshteinString = temporaryAnswer.last(6);
			//qDebug() << "frankenshteinString after add with last = " + frankenshteinString;
			temporaryAnswer.chop(6);
			if (temporaryAnswer == "")
			{
				frankenshteinString.push_front(",");
				//qDebug() << "frankenshteinString after add with push = " + frankenshteinString;
				frankenshteinString.push_front("0");
				//	qDebug() << "frankenshteinString after add with second push = " + frankenshteinString;
			}
			else {
				frankenshteinString.push_front(",");
				frankenshteinString.push_front(temporaryAnswer);
			}

			switch (counterForResend)
			{
			case 2:
			{
				answerString += "A+ ";
				break;
			}
			case 3:
			{
				answerString += "\nA- ";
				break;
			}
			case 4:
			{
				answerString += "\nR+ ";
				break;
			}
			case 5:
			{
				answerString += "\nR- ";
				break;
			}
			break;
			};

			answerString += frankenshteinString + ' ';
			qDebug() << "after convert " + frankenshteinString << '\n';
		}

		if (counterForResend == 6 || counterForResend == 11 || counterForResend == 12 || counterForResend == 13)
		{
			switch (counterForResend)
			{
			case 6:
			{
				answerString += "\nRelay ";
				break;
			}
			case 11:
			{
				answerString += "\nS (full) ";
				break;
			}
			case 12:
			{
				answerString += "\nP (active) ";
				break;
			}
			case 13:
			{
				answerString += "\nQ (reactive) ";
				break;
			}
			break;
			};

			if (minus)
			{
				answerString += '-' + temporaryAnswer + ' ';
			}
			else
			{
				answerString += temporaryAnswer + ' ';
			}
			//answerString += temporaryAnswer + ' ';
			qDebug() << "after convert " + temporaryAnswer << '\n';
		}

		if (counterForResend == 7 || counterForResend == 8 || counterForResend == 9 || counterForResend == 10 || counterForResend == 14 || counterForResend == 15)
		{

			if (temporaryAnswer.length() == 2)
			{
				temporaryAnswer.push_front("0");
			}

			if (temporaryAnswer.length() == 1)
			{
				temporaryAnswer.push_front("0");
				temporaryAnswer.push_front("0");
			}

			frankenshteinString = temporaryAnswer.last(3);
			//qDebug() << "frankenshteinString after add with last = " + frankenshteinString;
			temporaryAnswer.chop(3);

			if (temporaryAnswer == "")
			{
				frankenshteinString.push_front(",");
				//	qDebug() << "frankenshteinString after add with push = " + frankenshteinString;
				frankenshteinString.push_front("0");
				//qDebug() << "frankenshteinString after add with second push = " + frankenshteinString;
			}
			else {
				frankenshteinString.push_front(",");
				frankenshteinString.push_front(temporaryAnswer);
			}

			switch (counterForResend)
			{
			case 7:
			{
				answerString += "\nI ";
				break;
			}
			case 8:
			{
				answerString += "\nI neutral ";
				break;
			}
			case 9:
			{
				answerString += "\nU ";
				break;
			}
			case 10:
			{
				answerString += "\nFr ";
				break;
			}
			case 14:
			{
				answerString += "\ncos (f) ";
				break;
			}
			case 15:
			{
				answerString += "\nI diff ";
				break;
			}
			break;
			}

			if (minus)
			{
				answerString += '-' + frankenshteinString + ' ';
			}
			else
			{
				answerString += frankenshteinString + ' ';
			}
			qDebug() << "after convert " + frankenshteinString << '\n';
		}
	}

	if (serialStringForProtocol == "102" || serialStringForProtocol == "104" || serialStringForProtocol == "106")
	{
		bool ok;
		bool minus = false;
		QString temporaryAnswer = any.sliced(34);
		temporaryAnswer.chop(6);
		QString frankenshteinString;

		qDebug() << "after sliced and chop = " + temporaryAnswer << '\n';

		if ((temporaryAnswer.toUInt(&ok, 16) > 4200000000) && counterForResend > 17)
		{
			temporaryAnswer = QString("%1")
				.arg(4294967295 - temporaryAnswer.toUInt(&ok, 16));
			minus = true;
		}
		else
		{
			temporaryAnswer = QString("%1")
				.arg(temporaryAnswer.toULongLong(&ok, 16));
		}

		if (counterForResend <= 17)
		{
			if (temporaryAnswer.length() == 6)
			{
				temporaryAnswer.push_front("0");
			}

			if (temporaryAnswer.length() == 5)
			{
				temporaryAnswer.push_front("0");
				temporaryAnswer.push_front("0");
			}

			if (temporaryAnswer.length() == 4)
			{
				temporaryAnswer.push_front("0");
				temporaryAnswer.push_front("0");
				temporaryAnswer.push_front("0");
			}

			frankenshteinString = temporaryAnswer.last(7);
			//qDebug() << "frankenshteinString after add with last = " + frankenshteinString;
			temporaryAnswer.chop(7);
			if (temporaryAnswer == "")
			{
				frankenshteinString.push_front(",");
				//qDebug() << "frankenshteinString after add with push = " + frankenshteinString;
				frankenshteinString.push_front("0");
				//	qDebug() << "frankenshteinString after add with second push = " + frankenshteinString;
			}
			else {
				frankenshteinString.push_front(",");
				frankenshteinString.push_front(temporaryAnswer);
			}

			switch (counterForResend)
			{
			case 2:
			{
				answerString += "A+ ";
				break;
			}
			case 3:
			{
				answerString += "\nA- ";
				break;
			}
			case 4:
			{
				answerString += "\nR+ ";
				break;
			}
			case 5:
			{
				answerString += "\nR- ";
				break;
			}
			case 6:
			{
				answerString += "\nA+ L1 ";
				break;
			}
			case 7:
			{
				answerString += "\nA+ L2 ";
				break;
			}
			case 8:
			{
				answerString += "\nA+ L3 ";
				break;
			}
			case 9:
			{
				answerString += "\nA- L1 ";
				break;
			}
			case 10:
			{
				answerString += "\nA- L2 ";
				break;
			}
			case 11:
			{
				answerString += "\nA- L3 ";
				break;
			}
			case 12:
			{
				answerString += "\nR+ L1 ";
				break;
			}
			case 13:
			{
				answerString += "\nR+ L2 ";
				break;
			}
			case 14:
			{
				answerString += "\nR+ L3 ";
				break;
			}
			case 15:
			{
				answerString += "\nR- L1 ";
				break;
			}
			case 16:
			{
				answerString += "\nR- L2 ";
				break;
			}
			case 17:
			{
				answerString += "\nR- L3 ";
				break;
			}

			break;
			};

			answerString += frankenshteinString + ' ';
			qDebug() << "after convert " + frankenshteinString << '\n';
		}

		if (counterForResend == 18 || counterForResend == 26 || counterForResend == 27 || counterForResend == 28)
		{
			switch (counterForResend)
			{
			case 18:
			{
				answerString += "\nRelay ";
				break;
			}
			case 26:
			{
				answerString += "\nS (full) ";
				break;
			}
			case 27:
			{
				answerString += "\nP (active) ";
				break;
			}
			case 28:
			{
				answerString += "\nQ (reactive) ";
				break;
			}
			break;
			};

			if (minus)
			{
				answerString += '-' + temporaryAnswer + ' ';
			}
			else
			{
				answerString += temporaryAnswer + ' ';
			}
			//answerString += temporaryAnswer + ' ';
			qDebug() << "after convert " + temporaryAnswer << '\n';
		}

		if (counterForResend == 19 || counterForResend == 20 || counterForResend == 21 || counterForResend == 22 || counterForResend == 23 || counterForResend == 24 || counterForResend == 25 || counterForResend == 29)
		{

			if (temporaryAnswer.length() == 2)
			{
				temporaryAnswer.push_front("0");
			}

			if (temporaryAnswer.length() == 1)
			{
				temporaryAnswer.push_front("0");
				temporaryAnswer.push_front("0");
			}

			frankenshteinString = temporaryAnswer.last(3);
			//qDebug() << "frankenshteinString after add with last = " + frankenshteinString;
			temporaryAnswer.chop(3);

			if (temporaryAnswer == "")
			{
				frankenshteinString.push_front(",");
				//	qDebug() << "frankenshteinString after add with push = " + frankenshteinString;
				frankenshteinString.push_front("0");
				//qDebug() << "frankenshteinString after add with second push = " + frankenshteinString;
			}
			else {
				frankenshteinString.push_front(",");
				frankenshteinString.push_front(temporaryAnswer);
			}

			switch (counterForResend)
			{
			case 19:
			{
				answerString += "\nI L1 ";
				break;
			}
			case 20:
			{
				answerString += "\nI L2 ";
				break;
			}
			case 21:
			{
				answerString += "\nI L3 ";
				break;
			}
			case 22:
			{
				answerString += "\nU L1 ";
				break;
			}
			case 23:
			{
				answerString += "\nU L2 ";
				break;
			}
			case 24:
			{
				answerString += "\nU L3 ";
				break;
			}
			case 25:
			{
				answerString += "\nFr ";
				break;
			}
			case 29:
			{
				answerString += "\ncos (f) ";
				break;
			}
			break;
			}

			if (minus)
			{
				answerString += '-' + frankenshteinString + ' ';
			}
			else
			{
				answerString += frankenshteinString + ' ';
			}
			qDebug() << "after convert " + frankenshteinString << '\n';
		}
	}
}

QString TcpClientForTelegram::returnResultString()
{
	return answerString;
}

void TcpClientForTelegram::exchange()
{
	if (serialStringForProtocol == "101" || serialStringForProtocol == "103")
	{

		if (counterForResend != 17)
		{
			QTimer::singleShot(2000, [this]() {

				QString hexValueZero = QString::number(0, 16);
				QByteArray nullVal = QByteArray::fromHex(hexValueZero.toUtf8());

				if (counterForResend == 0)
				{
					//7E A0 1F 02 21 41 93 CC 30 81 80 12 05 01 F0 06 01 F0 07 04 00 00 00 01 08 04 00 00 00 01 F9 CB 7E

					QByteArray hexValue1 = "\x7e\xa0\x1f\x02\x21\x41\x93\xcc\x30\x81\x80\x12\x05\x01\xf0\x06\x01\xf0\x07\x04";
					QByteArray hexValue2 = "\x01\x08\x04";
					QByteArray hexValue3 = "\x01\xf9\xcb\x7e";

					QByteArray testArray = hexValue1 + nullVal + nullVal + nullVal + hexValue2 + nullVal + nullVal + nullVal + hexValue3;

					sendMessage(testArray);
				}

				if (counterForResend == 1)
				{
					//7E A0 45 02 21 41 10 95 BF E6 E6 00 60 36 A1 09 06 07 60 85 74 05 08 01 01 8A 02 07 80 8B 07 60 85 74 05 08 02 01 AC 0A 80 08 30 30 30 30 30 30 30 30 BE 10 04 0E 01 00 00 00 06 5F 1F 04 00 00 7E 1F FF FF 00 D1 7E

					QString hexValueZero = QString::number(0, 16);
					QByteArray nullVal = QByteArray::fromHex(hexValueZero.toUtf8());

					QByteArray hexValue1 = "\x7E\xA0\x45\x02\x21\x41\x10\x95\xBF\xE6\xE6";

					QByteArray hexValue2 = "\x60\x36\xA1\x09\x06\x07\x60\x85\x74\x05\x08\x01\x01\x8A\x02\x07\x80\x8B\x07\x60\x85\x74\x05\x08\x02\x01\xAC\x0A\x80\x08\x30\x30\x30\x30\x30\x30\x30\x30\xBE\x10\x04\x0E\x01";
					QByteArray hexValue3 = "\x06\x5F\x1F\x04";
					QByteArray hexValue4 = "\x7E\x1F\xFF\xFF";
					QByteArray hexValue5 = "\xD1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + nullVal + nullVal + hexValue3 + nullVal + nullVal + hexValue4 + nullVal + hexValue5;

					sendMessage(testArray);
				}

				if (counterForResend == 2) // A+ cur
				{
					//7E A0 1A 02 21 41 32 1B A2 E6 E6 00 C0 01 41 00 03 01 00 01 08 00 FF 02 00 29 FA 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x32\x1B\xA2\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x01\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x29\xFA\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 3) // A- cur
				{
					//7E A0 1A 02 21 41 54 2B A4 E6 E6 00 C0 01 41 00 03 01 00 02 08 00 FF 02 00 54 F6 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x54\x2B\xA4\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x02\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x54\xF6\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);

				}

				if (counterForResend == 4) // R+ cur
				{
					//7E A0 1A 02 21 41 76 3B A6 E6 E6 00 C0 01 41 00 03 01 00 03 08 00 FF 02 00 7F F2 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x76\x3B\xA6\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x03\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x7F\xF2\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);

				}

				if (counterForResend == 5) // R- cur
				{
					//7E A0 1A 02 21 41 98 4B A8 E6 E6 00 C0 01 41 00 03 01 00 04 08 00 FF 02 00 AE EE 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x98\x4B\xA8\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x04\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xAE\xEE\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 6) // Status relay
				{
					//7E A0 1A 02 21 41 BA 5B AA E6 E6 00 C0 01 41 00 46 00 00 60 03 0A FF 02 00 35 F7 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xBA\x5B\xAA\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x46";
					QByteArray hexValue4 = "\x60\x03\x0A\xFF\x02";
					QByteArray hexValue5 = "\x35\xF7\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + nullVal + hexValue4 + nullVal + hexValue5;

					sendMessage(testArray);
				}

				if (counterForResend == 7) // I sum
				{
					//7E A0 1A 02 21 41 DC 6B AC E6 E6 00 C0 01 41 00 03 01 00 0B 07 00 FF 02 00 DB B9 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xDC\x6B\xAC\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x0B\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xDB\xB9\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 8) // I neutral
				{
					//7E A0 1A 02 21 41 FE 7B AE E6 E6 00 C0 01 41 00 03 01 00 5B 07 00 FF 02 00 BA F9 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xFE\x7B\xAE\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x5B\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xBA\xF9\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 9) // U
				{
					//7E A0 1A 02 21 41 10 0B A0 E6 E6 00 C0 01 41 00 03 01 00 0C 07 00 FF 02 00 0A A5 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x10\x0B\xA0\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x0C\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x0A\xA5\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 10) // Frequency
				{
					//7E A0 1A 02 21 41 32 1B A2 E6 E6 00 C0 01 41 00 03 01 00 0E 07 00 FF 02 00 5C AD 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x32\x1B\xA2\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x0E\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x5C\xAD\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 11) // S
				{
					//7E A0 1A 02 21 41 54 2B A4 E6 E6 00 C0 01 41 00 03 01 00 09 07 00 FF 02 00 8D B1 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x54\x2B\xA4\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x09\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x8D\xB1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 12) // P
				{
					//7E A0 1A 02 21 41 76 3B A6 E6 E6 00 C0 01 41 00 03 01 00 01 07 00 FF 02 00 D5 90 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x76\x3B\xA6\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x01\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xD5\x90\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 13) // Q
				{
					//7E A0 1A 02 21 41 98 4B A8 E6 E6 00 C0 01 41 00 03 01 00 03 07 00 FF 02 00 83 98 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x98\x4B\xA8\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x03\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x83\x98\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 14) // cos (f)
				{
					//7E A0 1A 02 21 41 BA 5B AA E6 E6 00 C0 01 41 00 03 01 00 0D 07 00 FF 02 00 21 A1 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xBA\x5B\xAA\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x0D\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x21\xA1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 15) // I diff
				{
					//7E A0 1A 02 21 41 DC 6B AC E6 E6 00 C0 01 41 00 03 01 00 5B 07 83 FF 02 00 19 F1 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xDC\x6B\xAC\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x5B\x07\x83\xFF\x02";
					QByteArray hexValue5 = "\x19\xF1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5;

					sendMessage(testArray);
				}

				if (counterForResend == 16) // завершающий
				{
					//7E A0 08 02 21 41 53 5C 72 7E

					QByteArray hexValue1 = "\x7E\xA0\x08\x02\x21\x41\x53\x5C\x72\x7E";
					QByteArray testArray = hexValue1;

					sendMessage(testArray);
				}


				/*
				if (!socket->waitForReadyRead(30000)) ///////////////////////////////
				{
					qDebug() << "Nothing to read....";
				}
				*/

				/*
				if (reTransmitQuery >= 5) //////////////////////////
				{
					myTimer->stop();
					socket->close();
					ip = "";
					answerString += "\nNo or stopped responses from remote socket";
				}
				*/
				if (reTransmitQuery >= 4)
				{
					counterForResend = 17;
					answerString += "\nNo or stopped responses from remote socket";
				}

				myTimer->start(15000);
				});
		}
		else
		{
			myTimer->stop();
			socket->close();
			qDebug() << '\n' << answerString;
			ip = "";
			reTransmitQuery = 0;

			//emit messageReceived(answerString);
		}
	}

	if (serialStringForProtocol == "102" || serialStringForProtocol == "104" || serialStringForProtocol == "106")
	{

		if (counterForResend != 31)
		{
			QTimer::singleShot(2000, [this]() {

				QString hexValueZero = QString::number(0, 16);
				QByteArray nullVal = QByteArray::fromHex(hexValueZero.toUtf8());

				if (counterForResend == 0)
				{
					//7E A0 1F 02 21 41 93 CC 30 81 80 12 05 01 F0 06 01 F0 07 04 00 00 00 01 08 04 00 00 00 01 F9 CB 7E

					QByteArray hexValue1 = "\x7E\xA0\x1F\x02\x21\x41\x93\xCC\x30\x81\x80\x12\x05\x01\xF0\x06\x01\xF0\x07\x04";
					QByteArray hexValue2 = "\x01\x08\x04";
					QByteArray hexValue3 = "\x01\xF9\xCB\x7E";

					QByteArray testArray = hexValue1 + nullVal + nullVal + nullVal + hexValue2 + nullVal + nullVal + nullVal + hexValue3;

					sendMessage(testArray);
				}

				if (counterForResend == 1)
				{
					//7E A0 45 02 21 41 10 95 BF E6 E6 00 60 36 A1 09 06 07 60 85 74 05 08 01 01 8A 02 07 80 8B 07 60 85 74 05 08 02 01 AC 0A 80 08 30 30 30 30 30 30 30 30 BE 10 04 0E 01 00 00 00 06 5F 1F 04 00 00 7E 1F FF FF 00 D1 7E

					QString hexValueZero = QString::number(0, 16);
					QByteArray nullVal = QByteArray::fromHex(hexValueZero.toUtf8());

					QByteArray hexValue1 = "\x7E\xA0\x45\x02\x21\x41\x10\x95\xBF\xE6\xE6";

					QByteArray hexValue2 = "\x60\x36\xA1\x09\x06\x07\x60\x85\x74\x05\x08\x01\x01\x8A\x02\x07\x80\x8B\x07\x60\x85\x74\x05\x08\x02\x01\xAC\x0A\x80\x08\x30\x30\x30\x30\x30\x30\x30\x30\xBE\x10\x04\x0E\x01";
					QByteArray hexValue3 = "\x06\x5F\x1F\x04";
					QByteArray hexValue4 = "\x7E\x1F\xFF\xFF";
					QByteArray hexValue5 = "\xD1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + nullVal + nullVal + hexValue3 + nullVal + nullVal + hexValue4 + nullVal + hexValue5;

					sendMessage(testArray);
				}

				if (counterForResend == 2) // A+ cur
				{
					//7E A0 1A 02 21 41 32 1B A2 E6 E6 00 C0 01 41 00 03 01 00 01 08 00 FF 02 00 29 FA 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x32\x1B\xA2\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x01\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x29\xFA\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 3) // A- cur
				{
					//7E A0 1A 02 21 41 54 2B A4 E6 E6 00 C0 01 41 00 03 01 00 02 08 00 FF 02 00 54 F6 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x54\x2B\xA4\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x02\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x54\xF6\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);

				}

				if (counterForResend == 4) // R+ cur
				{
					//7E A0 1A 02 21 41 76 3B A6 E6 E6 00 C0 01 41 00 03 01 00 03 08 00 FF 02 00 7F F2 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x76\x3B\xA6\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x03\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x7F\xF2\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);

				}

				if (counterForResend == 5) // R- cur
				{
					//7E A0 1A 02 21 41 98 4B A8 E6 E6 00 C0 01 41 00 03 01 00 04 08 00 FF 02 00 AE EE 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x98\x4B\xA8\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x04\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xAE\xEE\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 6) //A + cur (phase 1)
				{
					//7E A0 1A 02 21 41 BA 5B AA E6 E6 00 C0 01 41 00 03 01 00 15 08 00 FF 02 00 35 A8 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xBA\x5B\xAA\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x15\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x35\xA8\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal +  hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 7) //A + cur (phase 2)
				{
					//7E A0 1A 02 21 41 DC 6B AC E6 E6 00 C0 01 41 00 03 01 00 29 08 00 FF 02 00 11 5E 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xDC\x6B\xAC\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x29\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x11\x5E\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 8) //A + cur (phase 3)
				{
					//7E A0 1A 02 21 41 FE 7B AE E6 E6 00 C0 01 41 00 03 01 00 3D 08 00 FF 02 00 0D 0C 7E 

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xFE\x7B\xAE\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x3D\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x0D\x0C\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 9) //A - cur (phase 1)
				{
					//7E A0 1A 02 21 41 10 0B A0 E6 E6 00 C0 01 41 00 03 01 00 16 08 00 FF 02 00 48 A4 7E 

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x10\x0B\xA0\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x16\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x48\xA4\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 10) //A - cur (phase 2)
				{
					//7E A0 1A 02 21 41 32 1B A2 E6 E6 00 C0 01 41 00 03 01 00 2A 08 00 FF 02 00 6C 52 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x32\x1B\xA2\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x2A\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x6C\x52\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 11)  //A - cur (phase 3)
				{
					//7E A0 1A 02 21 41 54 2B A4 E6 E6 00 C0 01 41 00 03 01 00 3E 08 00 FF 02 00 70 00 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x54\x2B\xA4\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x3E\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x70";
					QByteArray hexValue7 = "\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6 + nullVal + hexValue7;

					sendMessage(testArray);
				}

				if (counterForResend == 12)  //R + cur (phase 1)
				{
					//7E A0 1A 02 21 41 76 3B A6 E6 E6 00 C0 01 41 00 03 01 00 17 08 00 FF 02 00 63 A0 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x76\x3B\xA6\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x17\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x63\xA0\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 13)  //R + cur (phase 2)
				{
					//7E A0 1A 02 21 41 98 4B A8 E6 E6 00 C0 01 41 00 03 01 00 2B 08 00 FF 02 00 47 56 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x98\x4B\xA8\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x2B\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x47\x56\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 14)  //R + cur (phase 3)
				{
					//7E A0 1A 02 21 41 BA 5B AA E6 E6 00 C0 01 41 00 03 01 00 3F 08 00 FF 02 00 5B 04 7E 

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xBA\x5B\xAA\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x3F\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x5B\x04\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 15)  //R - cur (phase 1)
				{
					//7E A0 1A 02 21 41 DC 6B AC E6 E6 00 C0 01 41 00 03 01 00 18 08 00 FF 02 00 EA 9D 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xDC\x6B\xAC\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x18\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xEA\x9D\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}


				if (counterForResend == 16) //R - cur (phase 2)
				{
					//7E A0 1A 02 21 41 FE 7B AE E6 E6 00 C0 01 41 00 03 01 00 2C 08 00 FF 02 00 96 4A 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xFE\x7B\xAE\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x2C\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x96\x4A\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 17) //R - cur (phase 3)
				{
					//7E A0 1A 02 21 41 10 0B A0 E6 E6 00 C0 01 41 00 03 01 00 40 08 00 FF 02 00 D3 FC 7E 

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x10\x0B\xA0\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x40\x08";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xD3\xFC\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 18) //relay status
				{
					//7E A0 1A 02 21 41 32 1B A2 E6 E6 00 C0 01 41 00 46 00 00 60 03 0A FF 02 00 35 F7 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x32\x1B\xA2\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x46";
					QByteArray hexValue4 = "\x60\x03\x0A\xFF\x02";
					QByteArray hexValue5 = "\x35\xF7\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + nullVal + hexValue4 + nullVal + hexValue5;

					sendMessage(testArray);
				}

				if (counterForResend == 19) // I (phase 1)
				{
					//7E A0 1A 02 21 41 54 2B A4 E6 E6 00 C0 01 41 00 03 01 00 1F 07 00 FF 02 00 C7 EB 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x54\x2B\xA4\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x1F\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xC7\xEB\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 20) // I (phase 2)
				{
					//7E A0 1A 02 21 41 76 3B A6 E6 E6 00 C0 01 41 00 03 01 00 33 07 00 FF 02 00 53 5F 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x76\x3B\xA6\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x33\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x53\x5F\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 21) // I (phase 3)
				{
					//7E A0 1A 02 21 41 98 4B A8 E6 E6 00 C0 01 41 00 03 01 00 47 07 00 FF 02 00 FE 8A 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x98\x4B\xA8\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x47\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xFE\x8A\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 22) // U (phase 1)
				{
					//7E A0 1A 02 21 41 BA 5B AA E6 E6 00 C0 01 41 00 03 01 00 20 07 00 FF 02 00 9E 11 7E 

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xBA\x5B\xAA\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x20\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x9E\x11\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 23) // U (phase 2)
				{
					//7E A0 1A 02 21 41 DC 6B AC E6 E6 00 C0 01 41 00 03 01 00 34 07 00 FF 02 00 82 43 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xDC\x6B\xAC\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x34\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x82\x43\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 24) // U (phase 3)
				{
					//7E A0 1A 02 21 41 FE 7B AE E6 E6 00 C0 01 41 00 03 01 00 48 07 00 FF 02 00 77 B7 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\xFE\x7B\xAE\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x48\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x77\xB7\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 25) //frequensy
				{
					//7E A0 1A 02 21 41 10 0B A0 E6 E6 00 C0 01 41 00 03 01 00 0E 07 00 FF 02 00 5C AD 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x10\x0B\xA0\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x0E\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x5C\xAD\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 26) // S
				{
					//7E A0 1A 02 21 41 32 1B A2 E6 E6 00 C0 01 41 00 03 01 00 09 07 00 FF 02 00 8D B1 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x32\x1B\xA2\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x09\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x8D\xB1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 27) //P
				{
					//7E A0 1A 02 21 41 54 2B A4 E6 E6 00 C0 01 41 00 03 01 00 01 07 00 FF 02 00 D5 90 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x54\x2B\xA4\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x01\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\xD5\x90\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 28) //Q
				{
					//7E A0 1A 02 21 41 76 3B A6 E6 E6 00 C0 01 41 00 03 01 00 03 07 00 FF 02 00 83 98 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x76\x3B\xA6\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x03\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x83\x98\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 29) // cos(f)
				{
					//7E A0 1A 02 21 41 98 4B A8 E6 E6 00 C0 01 41 00 03 01 00 0D 07 00 FF 02 00 21 A1 7E

					QByteArray hexValue1 = "\x7E\xA0\x1A\x02\x21\x41\x98\x4B\xA8\xE6\xE6";
					QByteArray hexValue2 = "\xC0\x01\x41";
					QByteArray hexValue3 = "\x03\x01";
					QByteArray hexValue4 = "\x0D\x07";
					QByteArray hexValue5 = "\xFF\x02";
					QByteArray hexValue6 = "\x21\xA1\x7E";

					QByteArray testArray = hexValue1 + nullVal + hexValue2 + nullVal + hexValue3 + nullVal + hexValue4 + nullVal + hexValue5 + nullVal + hexValue6;

					sendMessage(testArray);
				}

				if (counterForResend == 30) // завершающий
				{
					//7E A0 08 02 21 41 53 5C 72 7E

					QByteArray hexValue1 = "\x7E\xA0\x08\x02\x21\x41\x53\x5C\x72\x7E";
					QByteArray testArray = hexValue1;

					sendMessage(testArray);
				}


				/*
				if (!socket->waitForReadyRead(30000)) ///////////////////////////////
				{
					qDebug() << "Nothing to read....";
				}
				*/

				/*
				if (reTransmitQuery >= 5) //////////////////////////
				{
					myTimer->stop();
					socket->close();
					ip = "";
					answerString += "\nNo or stopped responses from remote socket";
				}
				*/
				if (reTransmitQuery >= 4)
				{
					counterForResend = 31;
					answerString += "\nNo or stopped responses from remote socket";
				}

				myTimer->start(15000);
				});
		}
		else
		{
			myTimer->stop();
			socket->close();
			qDebug() << '\n' << answerString;
			ip = "";
			reTransmitQuery = 0;

			//emit messageReceived(answerString);
		}
	}
	
}


void TcpClientForTelegram::startToConnect(QString any)
{
	ip = any;
	connectToServer(ip, port);
}

void TcpClientForTelegram::resetAnswerString()
{
	answerString = "";
}

void TcpClientForTelegram::exchangeFromTimer()
{
	++reTransmitQuery;
	exchange();
}