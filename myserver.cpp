#include"myserver.h"
#include"mytask.h"
#include<QDebug>
#include<QTcpSocket>
#include<QThreadPool>
#include<QApplication>

//class begins


class SocketList
{
public:
QTcpSocket *soc;
SocketList *next;

SocketList()
{soc=NULL;next=NULL;}
};

SocketList *head=new SocketList();
SocketList *current=NULL;

//definition ends


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
soc->setSocketDescriptor(handle);
//adding to list

if(head->soc==NULL && head->next==NULL)
  {head->soc=soc;current=head;}
else
  {  current->next=new SocketList();
     current->next->soc=soc;
     current->next->next=NULL;
     current=current->next; 
  }

//

connect(soc,SIGNAL(readyRead()),this,SLOT(readyReading()));
}

void myserver::readyReading()
{
   QTcpSocket *socket=(QTcpSocket *)QObject::sender();
    qDebug()<<socket;
   mytask *task=new mytask(socket);
   QThreadPool::globalInstance()->start(task);
    task->setAutoDelete(true);
   connect(task,SIGNAL(broadcast(QString,QTcpSocket*,int)),this,SLOT(broadcasted(QString,QTcpSocket*,int)),Qt::DirectConnection);
}

void myserver::broadcasted(QString str,QTcpSocket *soc,int flag)
{
 
  if(flag==1 || flag==2)  
 	 {
	QTcpSocket *s=new QTcpSocket();
	s->setSocketDescriptor(soc->socketDescriptor());
	QByteArray b;
	b.append(str);
	s->write(b);
	s->flush();
	s->waitForBytesWritten();
	}

 SocketList *start=head;
      while(start!=NULL)
	{
            if(start->soc!=soc)
		{QByteArray ar;
		ar.append("<");
		 ar.append((start->soc->peerAddress()).toString());
		 ar.append("> ");
	         ar.append(str);
	         start->soc->write(ar);
		 start->soc->flush();
		 start->soc->waitForBytesWritten();
	 	}
 		start=start->next;
       }//while
}


