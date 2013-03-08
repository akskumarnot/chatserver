#include<QApplication>
#include"myserver.h"

int main(int argc,char * argv[])
{
QApplication app(argc,argv);
myserver *ser=new myserver();
ser->startServer();
return app.exec();
}
