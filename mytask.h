#ifndef MYTASK_H
#define MYTASK_H

#include<QThread>
#include<QRunnable>
#include<QTcpSocket>

class mytask :public QObject,public QRunnable
{
Q_OBJECT
public:
mytask(QTcpSocket *,QObject * parent=0);
void run();
void insertperson(QString str);
void removeperson(QString str);
QString recstr;
int number_group(QString);

signals:
void broadcast(QString,QTcpSocket*,int);


private:
QTcpSocket *soc;
int descriptor;
};

#endif
