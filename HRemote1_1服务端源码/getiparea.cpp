/**
* file: ip_from.cpp
* @date: 2007.4.20
* 给定ip,通过查询纯真数据库来给出具体的地址
*/
//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ios>
#include <fstream>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include "mainwindow.h"
using namespace std;

typedef unsigned int IP_TYPE;

const int  IP_SIZE = 4;
const int  OFFSET_SIZE = 3;
const int  INDEX_RECORD_SIZE = IP_SIZE + OFFSET_SIZE;

class IpLocater {
private:
    FILE* dbfile;
    int   first_index;
    int   last_index;
    enum { REDIRECT_MODE_1 = 0x01, REDIRECT_MODE_2 = 0x02 };
protected:

public:
    IpLocater(const string dbfilename = "qqwry.dat")
    {
        dbfile = fopen(dbfilename.c_str(), "rb");
        if (!dbfile) {
            throw "qqwry.dat文件不存在！";
        }

        fread(&first_index, sizeof(int), 1, dbfile);
        fread(&last_index, sizeof(int), 1, dbfile);
    }
    ~IpLocater() {
        fclose(dbfile);
    }

    int getRecordCount() const {
        return (last_index - first_index) / INDEX_RECORD_SIZE + 1;
    }

    string readString(const int offset = 0) {
        if (offset) {
            fseek(dbfile, offset, SEEK_SET);
        }

        char ch = fgetc(dbfile);
        ostringstream sstr;
        while (ch != 0 && ch != EOF) {
            sstr << ch;
            ch = fgetc(dbfile);
        }
        return sstr.str();
    }

    inline int readInt3(const int offset = 0) {
        if (offset) {
            fseek(dbfile, offset, SEEK_SET);
        }

        int result = 0;
        fread(&result, sizeof(char), 3, dbfile);
        return result;
    }

    string readAreaAddr(const int offset = 0) {
        if (offset) {
            fseek(dbfile, offset, SEEK_SET);
        }
        char b = fgetc(dbfile);
        if (b == REDIRECT_MODE_1 || b == REDIRECT_MODE_2) {
            int areaOffset = 0;
            fread(&areaOffset, 1, 3, dbfile);
            if (areaOffset) {
                return readString(areaOffset);
            }
            else {
                return "Unkown";
            }
        }
        else {
            fseek(dbfile, -1, SEEK_CUR);
            return readString();
        }
    }

    unsigned int readLastIp(const int offset) {
        fseek(dbfile, offset, SEEK_SET);
        unsigned int ip = 0;
        fread(&ip, sizeof(unsigned int), 1, dbfile);
        return ip;
    }

    string readFullAddr(const int offset, int ip = 0) {
        string address = "";

        fseek(dbfile, offset + 4, SEEK_SET);

        char ch = fgetc(dbfile);
        if (ch == REDIRECT_MODE_1) {
            int countryOffset = 0;
            fread(&countryOffset, sizeof(char), 3, dbfile);

            fseek(dbfile, countryOffset, SEEK_SET);
            char byte = fgetc(dbfile);
            if (byte == REDIRECT_MODE_2) {
                int p = 0;
                fread(&p, 1, 3, dbfile);
                address = readString(p);
                fseek(dbfile, countryOffset + 4, SEEK_SET);
            }
            else {
                address = readString(countryOffset);
            }
            address += readAreaAddr(); // current position
        }
        else if (ch == REDIRECT_MODE_2) {
            int p = 0;
            fread(&p, 1, 3, dbfile);
            address = readString(p);
            address += readAreaAddr(offset + 8);
        }
        else {
            fseek(dbfile, -1, SEEK_CUR);
            address = readString();
            address += readAreaAddr();
        }

        return address;
    }

    void printAddr(int first = 0, int count = 100) {
        int record_count = getRecordCount();
        for (int i = first; i< first + count && i<record_count; i++)
        {
            fseek(dbfile, i*INDEX_RECORD_SIZE + first_index, SEEK_SET);
            unsigned int ip = 0;
            fread(&ip, sizeof(unsigned int), 1, dbfile);
            int offset = 0;
            fread(&offset, 1, OFFSET_SIZE, dbfile);
            cout << ip2string(ip) << "-" << ip2string(readLastIp(offset))
                << readFullAddr(offset) << endl;
        }
    }

    int find(unsigned int ip, int left, int right) {
        if (right - left == 1) {
            return left;
        }
        else {
            int middle = (left + right) / 2;

            int offset = first_index + middle * INDEX_RECORD_SIZE;
            fseek(dbfile, offset, SEEK_SET);
            unsigned int new_ip = 0;
            fread(&new_ip, sizeof(unsigned int), 1, dbfile);

            if (ip >= new_ip) {
                return find(ip, middle, right);
            }
            else {
                return find(ip, left, middle);
            }
        }
    }

    string getIpAddr(unsigned int ip) {
        int index = find(ip, 0, getRecordCount() - 1);
        int index_offset = first_index + index * INDEX_RECORD_SIZE + 4;
        int addr_offset = 0;
        fseek(dbfile, index_offset, SEEK_SET);
        fread(&addr_offset, 1, 3, dbfile);
        string address = readFullAddr(addr_offset, ip);

        string utf8_address = GBKToUTF8(address);
        return utf8_address;
    }

    string ip2string(unsigned int ip) const {
        ostringstream sstr;
        sstr << ((ip & 0xff000000) >> 24);
        sstr << "." << ((ip & 0xff0000) >> 16);
        sstr << "." << ((ip & 0xff00) >> 8);
        sstr << "." << (ip & 0xff);
        return sstr.str();
    }

    unsigned int string2ip(const string ipstr)const {
        string str = ipstr;
        unsigned int ip = 0;
        int p = 0;
        p = str.find(".");
        ip += atoi(str.substr(0, p).c_str());
        ip <<= 8;
        str = str.substr(p + 1, str.length());
        p = str.find(".");
        ip += atoi(str.substr(0, p).c_str());
        ip <<= 8;
        str = str.substr(p + 1, str.length());
        p = str.find(".");
        ip += atoi(str.substr(0, p).c_str());
        ip <<= 8;
        ip += atoi(str.substr(p + 1, str.length()).c_str());
        return ip;
    }

    string GBKToUTF8(const string& strGBK)
    {
        string strOutUTF8 = "";
        WCHAR * str1;
        int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
        str1 = new WCHAR[n];
        MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
        n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
        char * str2 = new char[n];
        WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
        strOutUTF8 = str2;
        delete[]str1;
        str1 = nullptr;
        delete[]str2;
        str2 = nullptr;
        return strOutUTF8;
    }

    string getIpAddr(string ip) {

        return getIpAddr(string2ip(ip));
    }

};

