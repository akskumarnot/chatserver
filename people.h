#ifndef PEOPLE_H
#define PEOPLE_H
#include<QObject>
class QTcpSocket;

class people:public QObject
{
Q_OBJECT
public:
QString person_name;
QTcpSocket *person;
people *nextperson;
people();
};


#endif
