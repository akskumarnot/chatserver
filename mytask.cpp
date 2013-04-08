#include"mytask.h"
#include<QApplication>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlRecord>
#include<QStringList>
#include<QSqlError>
#include"people.h"
#include"peoplelist.h"
#include"privatecouple.h"

extern peoplelist *firstgroup;int uid;
extern privatecouple *privatelist;

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
	emit broadcast(list_people(abc.at(2)),soc,1);
	qDebug()<<list_people(abc.at(2));
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
					  {emit broadcast("$valid$",soc,1);db.close();return;}
		
				  }

				emit broadcast("$invalid$",soc,1);
				db.close();		
	            		}
	 		
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



if(str.left(7)=="$check$")
	{	
		QStringList abc=str.split("$");
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("table.db");
		
		if(!db.open())
			{qDebug() << db.lastError();
			qDebug( "Failed to connect.");}
			
		QSqlQuery qry;

		if(!qry.exec("select username from login"))
			{qDebug()<<qry.lastError();return;}
		
		int fl=0;
		while(qry.next())
		{	
			if(qry.value(0).toString()==abc.at(2))
			{
			emit broadcast("$nonvalidated$",soc,1);
			qDebug()<<"already there";fl=1;break;}
		
		}

		if(fl==0)
		{emit broadcast("$validated$",soc,1);}
	db.close();
	return;
	}

	if(str.left(8)=="$signup$")
	{
	qDebug()<<"signup here";
	QStringList lol=str.split("$");
	
		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("table.db");
		
		if(!db.open())
			{qDebug() << db.lastError();
			qDebug( "Failed to connect.");}
			
		QSqlQuery qry;
		QString val="insert into people values('";
		val.append(lol.at(2)+"','"+lol.at(3)+"','"+lol.at(4)+"')");
		qDebug()<<val;
		if(!qry.exec(val))
		{qDebug()<<qry.lastError();return;}

		QString val1="insert into login values('";
		val1.append(lol.at(2)+"','"+lol.at(4)+"')");
		qDebug()<<val1;
		if(!qry.exec(val1))
		{qDebug()<<qry.lastError();return;}

	return;
	}

//private chat section

	if(str.left(4)=="$pm$") 

		{
			QStringList abc=str.split("$");
			if(abc.at(2)=="request")
				{
					QString gpname=abc.at(3);
	
					peoplelist *pl=firstgroup->nextgroup;
					while(pl!=NULL && (pl->group_name!=gpname))
						{
						pl=pl->nextgroup;
						}

					if(pl==NULL)return;
			
					people *man2=pl->group;
						
						
					while(man2!=NULL && man2->person_name!=abc.at(5))
						{
							man2=man2->nextperson;
						}	

				//man2 is the person jisko private chat se message bhejega soc
					if(man2==NULL)return;												
					
					privatecouple * couple=privatelist;
					
					while(couple->nextcouple!=NULL && ((couple->name1!=abc.at(4) && couple->name2!=abc.at(5))||(couple->name1!=abc.at(5) && couple->name2!=abc.at(4))))
						{
							couple=couple->nextcouple;
						}
				

				if(couple->nextcouple==NULL)	
					{
					couple->nextcouple = new privatecouple();
					couple=couple->nextcouple;
					couple->id=QString::number(++uid);
					couple->name1=abc.at(4);
					couple->name2=abc.at(5);
					couple->one=soc;
					couple->two=NULL;
					couple->there1=true;
					couple->there2=false;
					//need to send the ids to both the men
				
						
					emit chatcast("$pm$id$"+QString::number(uid),couple->one);
					emit chatcast("$pm$openbox$"+QString::number(uid)+"$"+abc.at(3)+"$"+abc.at(4),man2->person);
						
					}
				}

		//adding non-initiator of chat

		if(abc.at(2)=="addthisbox")
			{
				
				qDebug()<<"trying ";
				privatecouple * couple=privatelist->nextcouple;

					if(couple==NULL)
						{return;}
				
				while(couple!=NULL && couple->id!=abc.at(3))
					{
						couple=couple->nextcouple;
					}	
					
				if(couple==NULL)return;										
				
				couple->two=soc;				
				couple->there2=true;
				couple->nextcouple=NULL;
			}
		//ending the adding


			if(abc.at(2)=="message")
					{
						privatecouple *couple=privatelist->nextcouple;
						
	
						while(couple->nextcouple!=NULL && ((couple->name1!=abc.at(4) && couple->name2!=abc.at(5))||(couple->name1!=abc.at(5) && couple->name2!=abc.at(4))))		
							{qDebug()<<couple->id;
								couple=couple->nextcouple;
								
							}
						



						if(couple==NULL){return;}
							QTcpSocket *r;
						if(couple->one==soc)
							{r=couple->two;}							
						else{r=couple->one;}

						
						emit chatcast("$pm$message$"+abc.at(5),r);			
					
					}	
		return;
		}


//  	
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


QString mytask::list_people(QString grp)
	{
	peoplelist *p=firstgroup->nextgroup;
	
	while(p->nextgroup!=NULL && p->group_name!=grp)	
		{
		p=p->nextgroup;		
		}
	
	people *ppl=p->group;
	QString names="$names$";
	while(ppl!=NULL)
 		{names+=ppl->person_name+"$";ppl=ppl->nextperson;}
	qDebug()<<"here2";
	return names; 	
	}




