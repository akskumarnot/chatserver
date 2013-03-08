#include"myserver.h"
#include"mytask.h"
#include<QDebug>
#include<QTcpSocket>
#include<QThreadPool>

QTcpSocket *arr[100];
int counter=0;

myserver::myserver(QObject *parent):QTcpServer(parent)
{
QThreadPool::globalInstance()->setMaxThreadCount(10);
}

void myserver::startServer()
{
  if(!this->listen(QHostAddress::Any,9982))
    {qDebug()<<"could not start";}
  else
   {qDebug()<<"listening";}
 
}

void myserver::incomingConnection(int handle)
{
qDebug()<<"got new connection";
QTcpSocket *soc=new QTcpSocket();
arr[counter]=soc;
counter++;
soc->setSocketDescriptor(handle);
connect(soc,SIGNAL(readyRead()),this,SLOT(readyReading()));
}

void myserver::readyReading()
{
   QTcpSocket *socket=(QTcpSocket *)QObject::sender();
    qDebug()<<socket;
   mytask *task=new mytask(socket);
   QThreadPool::globalInstance()->start(task);
    task->setAutoDelete(true);
   connect(task,SIGNAL(broadcast(QString,QTcpSocket*)),this,SLOT(broadcasted(QString,QTcpSocket*)),Qt::DirectConnection);
}

void myserver::broadcasted(QString str,QTcpSocket *soc)
{
   	int i=0;
      while(i!=counter)
	{
            if(arr[i]!=soc)
		{QByteArray ar;
	         ar.append(str);
	         arr[i]->write(ar);
		 arr[i]->flush();
		 arr[i]->waitForBytesWritten();
	 	}
 		i++;
       }//while
}


