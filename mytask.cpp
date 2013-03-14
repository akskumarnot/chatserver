#include"mytask.h"
#include<QApplication>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlRecord>
#include<QStringList>
#include<QSqlError>
#include"people.h"
#include"peoplelist.h"

extern peoplelist *firstgroup;

mytask::mytask(QTcpSocket* soc,QObject * parent):QObject(parent)
{
this->soc=soc;
}

void mytask::run()
{
QByteArray arr=soc->readAll();
QString str(arr);
//adding the person

	if(str.left(6)=="$room$")
	{insertperson(str);
	QStringList abc=str.split("$");
	QString nu="$room$"+abc.at(2)+"$"+abc.at(3)+"$"+abc.at(3)+" joined the room$";
	emit broadcast(nu,soc,2);
	return;
	}
//adding over	


//removing the person
	if(str.left(5)=="$bye$")
	{removeperson(str);
	QStringList abc=str.split("$");
	QString nu="$room$"+abc.at(2)+"$"+abc.at(3)+"$"+abc.at(3)+" said buh-bye$";
	qDebug()<<nu;
	emit broadcast(nu,soc,2);
	return;
	}
//removing over


//signing the person in

	if(str.left(7)=="$signin")
	{qDebug()<<soc<<"wants to signin";
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
					  {emit broadcast("$valid$",soc,1);}
				  }		
	            		}
	 		db.close();
		return;
	   }
//signing over


//send data for populating the client chat list
   	 if(str.left(6)=="$list$")
	{qDebug()<<soc<<"wants the list";
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
			
			emit broadcast(str,soc,1);
		
		db.close();
	    return;
	}
//send data to populate
   	
  if(str.left(9)=="$message$")
	{
	qDebug()<<soc<<"wants to send message";
	emit broadcast(str,soc,0);
	}
}


void mytask::insertperson(QString str)
{
qDebug()<<"insertion starts";
QStringList l=str.split("$");
	peoplelist *temp1=firstgroup->nextgroup;

		while(temp1!=NULL && temp1->group_name!=l.at(2))
			{
				temp1=temp1->nextgroup;
			}							
	
									
		if(temp1==NULL){return;}
	

		if(temp1->group==NULL)
			{											
			   temp1->group=new people();
			   temp1->group->person_name=l.at(3);
			   temp1->group->nextperson=NULL;
			   temp1->group->person=soc;
			   qDebug()<<soc<<"and"<<temp1->group->person<<"should be same";
			   	
			}
		else
		  {
			people *temp2=temp1->group;
			while(temp2->nextperson!=NULL)
			 {
				temp2=temp2->nextperson;
			 }
			
			
			temp2->nextperson=new people();
			temp2=temp2->nextperson;

			temp2->person_name=l.at(3);
			qDebug()<<l.at(3)<<"joined";
			temp2->person=soc;
			temp2->nextperson=NULL;
			
		  }

}	

void mytask::removeperson(QString str)
{
qDebug()<<"removal starts";
QStringList l=str.split("$");
	peoplelist *temp1=firstgroup->nextgroup;

	while(temp1->nextgroup!=NULL && temp1->group_name!=l.at(2))
	{temp1=temp1->nextgroup;}
	
	qDebug()<<"removing from group"<<temp1->group_name;
	people *temp2=temp1->group;
	people *prev=NULL;
	
	qDebug()<<temp2;
	if(temp2->person_name==l.at(3))
		{
		temp1->group=temp2->nextperson;
		delete temp2;
		temp2=NULL;
		}
	else{
		while(temp2->person_name!=l.at(3))
			{prev=temp2;temp2=temp2->nextperson;}

			prev->nextperson=temp2->nextperson;
			qDebug()<<temp2->person_name<<"is leaving";
			delete temp2;
			temp2=NULL;
	    }	
	

	
}

int mytask::number_group(QString grp)
{
peoplelist *p=firstgroup->nextgroup;

while(p->nextgroup!=NULL && p->group_name==grp)
	{
	p=p->nextgroup;
	}

int number=0;
people *ppl=p->group;
while(ppl->nextperson!=NULL)
  {ppl=ppl->nextperson;number++;}
return number;	
}




