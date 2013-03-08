#include"mytask.h"
#include<QApplication>

mytask::mytask(QTcpSocket* soc,QObject * parent):QObject(parent)
{
Q_UNUSED(parent);
this->soc=soc;
}

void mytask::run()
{
QByteArray arr=soc->readAll();
QString str(arr);
qDebug()<<descriptor;
emit broadcast(str,soc);
}



