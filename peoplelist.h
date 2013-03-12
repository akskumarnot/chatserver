#ifndef PEOPLELIST_H
#define PEOPLELIST_H

#include"people.h"
#include<QObject>

class peoplelist:public QObject
{Q_OBJECT
public:
QString group_name;
people *group;
peoplelist * nextgroup;
peoplelist();
};


#endif
