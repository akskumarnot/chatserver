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
QString recstr;

signals:
void broadcast(QString,QTcpSocket*);

private:
QTcpSocket *soc;
int descriptor;
};

#endif
