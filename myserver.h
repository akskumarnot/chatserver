#ifndef MYSERVER_H
#define MYSERVER_H

#include<QTcpServer>
#include<QObject>

class myserver: public QTcpServer
{
Q_OBJECT
public:


myserver(QObject *parent=0);

public slots:

void startServer();
void broadcasted(QString,QTcpSocket*);
void readyReading();

protected:
void incomingConnection(int handle);

};

#endif
