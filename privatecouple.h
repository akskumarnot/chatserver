#ifndef PRIVATECOUPLE_H
#define PRIVATECOUPLE_H
#include<QTcpSocket>

class privatecouple: public QObject
{
Q_OBJECT
	public:
	QString name1;
	QString name2;
	bool there1;
	bool there2;
	QTcpSocket *one;
	QTcpSocket *two;
	QString id;
	privatecouple *nextcouple;
	privatecouple();
};
#endif
