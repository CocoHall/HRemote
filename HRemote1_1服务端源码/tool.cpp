#include "tool.h"
#include <QDir>
#include <QTextCodec>

tool::tool(){

}

bool tool::isQuit=false;
int tool::port=12322;
bool tool::islisten=false;
SOCKET tool::sockSrv=0;
MainWindow* tool::m=nullptr;
QMutex* tool::mut=new QMutex();
QSqlDatabase tool::database=QSqlDatabase();
myaccepyThread* tool::acc=new myaccepyThread();

QList<netManager*>* tool::storeList=new QList<netManager*>();
ThreadSafeQueue<netManager*>* tool::deleteQueue=new ThreadSafeQueue<netManager*>();

QString tool::GetAddressBySocket(SOCKET client)
{

    SOCKADDR_IN addrIp;
    int addrIpsize=sizeof(addrIp);
    getpeername(client,(SOCKADDR*)&addrIp,&addrIpsize);

    char ipadd[20];
    strcpy_s(ipadd,inet_ntoa(addrIp.sin_addr));
    QString result = QString(QLatin1String(ipadd));
    return result+":"+QString::number(addrIp.sin_port,10);
}

QString tool::GetIPBySocket(SOCKET client)
{

    SOCKADDR_IN addrIp;
    int addrIpsize=sizeof(addrIp);
    getpeername(client,(SOCKADDR*)&addrIp,&addrIpsize);

    char ipadd[20];
    strcpy_s(ipadd,inet_ntoa(addrIp.sin_addr));
    return QString(QLatin1String(ipadd));
}

QString tool::translateTime(int millisecond){
    if(millisecond<1000){
        return "正在使用";
    }
    else if(millisecond<1000*60){
        return QString::number(millisecond/1000)+"秒";
    }
    else if(millisecond<1000*60*60){
        return QString::number(millisecond/1000/60)+"分";
    }
    else if(millisecond<1000*60*60*24){
        return QString::number(millisecond/1000/60/60)+"小时";
    }
    else {
        return QString::number(millisecond/1000/60/60/24)+"天";
    }
}

long long tool::pow(int a,int b){
    if(b<=0){
        return 1;
    }
    long long s=1;
    for(int i=0;i<b;i++){
        s*=a;
    }
    return s;

}


//输入Byte，输出换算后的的存储大小
QString tool::ByteConvert(long long byteSize) {
    if (byteSize <= 0) return "0.00 B";
    int i = -1;

    QString unit[] = { " B", " KB", " MB", " GB", " TB", " PB", " EB", " ZB", " YB" };
    long long k = byteSize;
    //cout << "k "<<k << endl;
    while (k>0) {
        k /= 1024;
        i++;
    }
    //cout << "i" << i << endl;
    i = i % 9;
    byteSize=byteSize/pow(1024,i-1);
    float t = byteSize*1.0/1024;

    char buf[10] = {0};
    sprintf(buf, "%.2f", t);

    return buf + unit[i];
}

QString tool::getFileType(QString filename){
    if(filename.lastIndexOf(".")==-1){
        return filename;
    }
    return filename.mid(filename.lastIndexOf("."),-1);

}


QString tool::timestampConvert(__time64_t t){

    t+=8*60*60;//东八区
    struct tm *p;
    p=gmtime(&t);
    char s[80];
    strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);
    return s;

}

void tool::openDatabase(){


    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
       database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("info.db");
        database.setUserName("hremote");
        database.setPassword("hremote");

    }
    if(database.open()){
        QSqlDatabase db = QSqlDatabase::database();
        if(!db.tables().contains("info")){
            QSqlQuery sql_query;
            sql_query.exec("create table info(name text , nickname text)");
        }
    }
}

void tool::insertNickname(QString name,QString nickname){
    if(database.isOpen()){
        QSqlQuery sql_query;
        if(getNickname(name).length()>0){
            if(nickname.length()>0){
                sql_query.prepare("UPDATE info SET nickname=:nickname WHERE name=:name");
            }else{
                sql_query.prepare("DELETE FROM info WHERE name=:name");
            }
        }else{
            if(nickname.length()==0)return;
            sql_query.prepare("INSERT INTO info(name,nickname) VALUES(:name,:nickname)");
        }

        sql_query.bindValue(":name", name);
        sql_query.bindValue(":nickname", nickname);

        sql_query.exec();
    }

}

QString tool::getNickname(QString name){
    if(database.isOpen()){
        QSqlQuery sql_query;
        sql_query.prepare("select nickname from info where name=:name limit 1");
        sql_query.bindValue(":name", name);
        sql_query.exec();
        while(sql_query.next())
        {
            QString result = sql_query.value(0).toString();
            return result;
        }


    }
    return "";
}
