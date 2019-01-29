//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#pragma once
#include <string>
#include <windows.h>
#include <QString>

typedef struct FileStruct {

        bool isRoot;//1
        long long fileTotalSize;//8
        long long fileFreeSize;//8
        int diskType;//4

        unsigned    attrib;//4
        __time64_t  time_create; //8   // -1 for FAT file systems
        __time64_t  time_access; //8   // -1 for FAT file systems
        __time64_t  time_write;//8
        _fsize_t    size;//4

        QString fileName;

}mfile;

//typedef struct ProcessStruct {
//        std::string pName;
//        DWORD pID;
//}mprocess;


typedef struct VoiceStruct {
        unsigned long  voiceSize;//音频的大小
        char *voiceMem;//音频数据

}mvoice;

typedef struct OrderStruct {
        int orderType;//指令类型
        char *orderMem;//指令内容
        int orderSize;//指令长度

}morder;

