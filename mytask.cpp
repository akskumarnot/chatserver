#include"mytask.h"
#include<QApplication>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlRecord>
#include<QStringList>
#include<QSqlError>

mytask::mytask(QTcpSocket* soc,QObject * parent):QObject(parent)
{
Q_UNUSED(parent);
this->soc=soc;
}

void mytask::run()
{
QByteArray arr=soc->readAll();
QString str(arr);
int flag=0;
	if(str.left(7)=="$signin")
	{flag=1;int found=0;
	 QStringList list=str.split("$");
	 QString name=list[2];
	 QString pass=list[3];
	 QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
	 db.setDatabaseName("table.db");

		if( !db.open() )
			{
			qDebug() << db.lastError();
			qDebug( "Failed to connect." );
			}
      
	 		QSqlQuery qry;
	
			 if(!qry.exec("select * from login"))
				{qDebug()<<qry.lastError();}
		 else{

			while(qry.next())
			  {
			      if(qry.value(0).toString()==name && qry.value(1).toString()==pass)
				  {found=1;}
			  }		
	            }
	 	
				if(found==1)
                		{
				emit broadcast("$valid$",soc,1);
				return;
				}
				db.close();
		
	}

   	else if(str.left(6)=="$list$")
	{
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("table.db");
		if(!db.open())
			{qDebug() << db.lastError();
			qDebug( "Failed to connect." );}
		
		QSqlQuery qry;
		 
		if(!qry.exec("select * from category"))
			{qDebug()<<qry.lastError();}
			QString str="$";
		while(qry.next())
			{
			str+=qry.value(0).toString();
			str+="$";
			}
			
			emit broadcast(str,soc,2);
		
		db.close();
				    
	}
   	
if(!flag){emit broadcast(str,soc,0);}

}



