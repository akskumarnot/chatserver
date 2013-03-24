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
qDebug()<<soc<<"was created";
soc->setSocketDescriptor(handle);
connect(soc,SIGNAL(readyRead()),this,SLOT(readyReading()));
}

void myserver::readyReading()
{
   QTcpSocket *socket=(QTcpSocket *)QObject::sender();
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


	if(mode==0 || mode==2)
	{	
		QStringList l=str.split("$");
		QString gpname=l.at(2);
		
			
		peoplelist *pl=firstgroup->nextgroup;
		while(pl!=NULL && (pl->group_name!=gpname))
		{
		pl=pl->nextgroup;
		}
		
		
		people *gp=pl->group;

			if(gp==NULL){return;qDebug()<<"kisko bhejoon??";}
		
			while(gp!=NULL)
				{
				if(gp->person_name!=l.at(3))
					{	
						QTcpSocket *s=new QTcpSocket();
						s->setSocketDescriptor(gp->person->socketDescriptor());
						QByteArray a;
						if(mode==2)
						{a.append("$room$"+l.at(4));
						}
						else{a.append(l.at(3)+" : "+l.at(4));}
						s->write(a);
						s->flush();
						s->waitForBytesWritten();	
					}	
					gp=gp->nextperson;
										
				}

	return;
		
	}
}


