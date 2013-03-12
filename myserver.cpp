#include"myserver.h"
#include"mytask.h"
#include<QDebug>
#include<QTcpSocket>
#include<QByteArray>
#include<QThreadPool>
#include<QApplication>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QStringList>
#include<QSqlError>
#include"peoplelist.h"
#include"people.h"

peoplelist * firstgroup=new peoplelist();


myserver::myserver(QObject *parent):QTcpServer(parent)
{
firstgroup->group_name="header";
firstgroup->group=NULL;firstgroup->nextgroup=NULL;
QThreadPool::globalInstance()->setMaxThreadCount(10);
//initializing groups
	 QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
	 db.setDatabaseName("table.db");

		if( !db.open() )
			{
			qDebug() << db.lastError();
			qDebug( "Failed to connect." );
			}

				QSqlQuery qry;
	
			 if(!qry.exec("select * from category"))
				{qDebug()<<qry.lastError();}
		 	 else{
				peoplelist *temp=firstgroup;
	
				while(qry.next())
					{
				           temp->nextgroup=new peoplelist();
						temp=temp->nextgroup;
						temp->group_name=qry.value(0).toString();
						temp->nextgroup=NULL;	
						temp->group=NULL;			
					}
					
		             }

		 db.close();

//end of bullshit
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

void myserver::broadcasted(QString str,QTcpSocket *soc,int mode)
{
 
  if(mode==1)  
 	 {
	QTcpSocket *s=new QTcpSocket();
	s->setSocketDescriptor(soc->socketDescriptor());
	QByteArray b;
	b.append(str);
	s->write(b);
	s->flush();
	s->waitForBytesWritten();
	return;
	}


	if(mode==0)
	{QStringList l=str.split("$");
	QString gpname=l.at(2);
	
	peoplelist *pl=firstgroup;
	while(pl->nextgroup!=NULL && (pl->group_name!=l.at(3)))
	{
		pl=pl->nextgroup;
	}
	
		people *gp=pl->group;
		
			while(gp->nextperson!=NULL)
				{
				if(soc!=gp->person)
					{
						QByteArray ar;
						ar.append("<");
		 				ar.append(gp->person_name);
						 ar.append(">");
	         				ar.append(l.at(3));
	         				gp->person->write(ar);
		 				gp->person->flush();
		 				gp->person->waitForBytesWritten();
					}	
					gp=gp->nextperson;			
				}

	return;	
	}
}


