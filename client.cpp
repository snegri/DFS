#include <iostream>
#include <map>
#include <string>
#include <string.h>
#include <fstream>
#include <thread>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <mutex>
#include <fstream>
#include <list>
#include <iterator>
#include <vector>
#include <set>
#include "md5.h"
#define BUFSIZE 1028

void printMenu() {
    std::cout << "-----------------" << std::endl;
    std::cout << "| List           |" << std::endl;
    std::cout << "| Get <Filename> |" << std::endl;
    std::cout << "| Put <Filename  |" << std::endl;
    std::cout << "-----------------" << std::endl;
}

void showlist(std::vector<std::string> g)
{
    std::vector<char> path;
    for (auto i = path.begin(); i != path.end(); ++i)
        std::cout << *i << ' ';
}

std::string bufferToString(char* buffer, int bufflen)
{
    std::string ret(buffer, bufflen);
    return ret;
}

int main (int argc, char * argv[]) {
    
    std::ifstream myReadFile;
    myReadFile.open(argv[1]);
    std::string line, placeHolder, temp, port, userName, password, userNameEnter, passwordEnter;
    int port1, port2, port3, port4;
    int count = 1;
    while (getline(myReadFile, line)) {
        std::istringstream ss(line);
        ss >> placeHolder;
        ss >> placeHolder;
        ss >> temp;
        port = temp.substr(temp.find(":")+1);
        if (count == 1) {
            port1 = stoi(port);
        }
        else if (count == 2) {
            port2 = stoi(port);
        }
        else if (count == 3) {
            port3 = stoi(port);
        }
        else if (count == 4) {
            port4 = stoi(port);
        }
        else if (count == 5){
            userName = placeHolder;
        }
        else if (count == 6){
            password = placeHolder;
        }
        count++;
    }

    std::vector<std::string> fileList;
    for(;;) {
        std::string option, temp;
        printMenu();
        getline(std::cin, option);
        std::istringstream ss(option);
        std::string filename;
        ss >> filename;
        ss >> filename;
        
        showlist(fileList);
        
        if ((option.find("Put") == 0) or (option.find("put") == 0)) {
            fileList.push_back(filename);
            std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
            in.seekg (0, in.end);
            int length = in.tellg();
            in.seekg (0, in.beg);
            int place;
            place = length;
            //std::cout << length/4 << std::endl;
            char c;
            int i=0,j=0;
            std::string part1;
            std::string part2;
            std::string part3;
            std::string part4;
            while (in.get(c)) {
                if (j < length/4) {
                    part1.push_back(c);
                }
                else if (j < length/2 and j >= length/4) {
                    part2.push_back(c);
                }
                else if (j < 3*(length/4) and j >= length/2) {
                    part3.push_back(c);
                }
                else if (j < length and j >= 3*(length/4)) {
                    part4.push_back(c);
                }
                if (i == (length/4)-1) {
                    i = 0;
                }
                else {
                    i++;
                }
                j++;
            }
            
            std::string full = part1 + part2 + part3 + part4;
            std::string hash = md5(full);
            hash = hash.substr(23,31);
            
            //std::cout << "Hash is: " << hash << std::endl;
            
            int y = std::stoul(hash, nullptr, 16) % 4;
            std::cout << "x is : " << y << std::endl;
            if (y == 0) {
                
                
                struct timeval tv;
                tv.tv_sec = 3;
                
                // DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                int sock = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
                
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port1);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //std::cout << "Uploading part1..." << std::endl;
                
                //std::cout << "Sending authentication credentials..." << std::endl;
                std::string credentials = userName + " " + password;
                //std::cout << credentials << std::endl;
                
                std::string request = credentials + '\n' + option;
                
                char req[BUFSIZE];
                strcpy(req, request.c_str());
                
                //Send Reqest <credentials>\n<request>
                int datalen = strlen(req);
                int tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                char *partNum = "1";
                datalen = strlen(partNum);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partNum, datalen);
                
                //Send first file
                datalen = part1.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                int n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                char *partNum2 = "2";
                datalen = strlen(partNum2);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partNum2, datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part2.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                std::cout << n << std::endl;
                
                close(sock);
                
                sock = 0;
                
                
                //END DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                // DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port2);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                std::string partS;
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "2";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part2.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "3";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                
                // DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                //std::cout <<"POrt 3 is " << port3 << std::endl;
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port3);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "3";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "4";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
               //std::cout << n << std::endl;
                
                //Send first file
                datalen = part4.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port4);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "4";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part4.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "1";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part1.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                //END DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
            
            if (y == 1) {
                
                struct timeval tv;
                tv.tv_sec = 3;
                
                // DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                int sock = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
                
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port1);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //std::cout << "Uploading part1..." << std::endl;
           
                //std::cout << "Sending authentication credentials..." << std::endl;
                std::string credentials = userName + " " + password;
                //std::cout << credentials << std::endl;
                
                std::string request = credentials + '\n' + option;
                
                char req[BUFSIZE];
                strcpy(req, request.c_str());
                
                //Send Reqest <credentials>\n<request>
                int datalen = strlen(req);
                int tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                char *partNum = "4";
                datalen = strlen(partNum);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partNum, datalen);
                
                
                //Send first file
                datalen = strlen(part4.c_str());
                //std::cout << "----------Sent----------" << part4.c_str() << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                //std::cout << "Sent " << datalen << " datalen" << std::endl;
                int n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                char *partNum2 = "1";
                datalen = strlen(partNum2);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partNum2, datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part1.length();
                //std::cout << "----------Sent----------" << part1 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                //std::cout << "Sent " << datalen << " datalen" << std::endl;
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                sock = 0;
                
                
                //END DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                // DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port2);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                std::string partS;
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "1";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part1.length();
                //std::cout << "----------Sent----------" << part1 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "2";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part2.length();
                //std::cout << "----------Sent----------" << part2 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                //std::cout <<"POrt 3 is " << port3 << std::endl;
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port3);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "2";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part2.length();
                //std::cout << "----------Sent----------" << part2 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "3";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part3.length();
                //std::cout << "----------Sent----------" << part3 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port4);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "3";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part3.length();
                //std::cout << "----------Sent----------" << part3 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "4";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part4.length();
                //std::cout << "----------Sent----------" << part4 << std::endl;
                //std::cout << "------------------------" << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                //END DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
            
            if (y == 2) {
                
                struct timeval tv;
                tv.tv_sec = 3;
                
                // DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                int sock = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
                
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port1);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //std::cout << "Uploading part1..." << std::endl;
                
                //std::cout << "Sending authentication credentials..." << std::endl;
                std::string credentials = userName + " " + password;
                //std::cout << credentials << std::endl;
                
                std::string request = credentials + '\n' + option;
                
                char req[BUFSIZE];
                strcpy(req, request.c_str());
                
                //Send Reqest <credentials>\n<request>
                int datalen = strlen(req);
                int tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                //std::cout << "Sending " << req <<std::endl;
                write(sock, req, datalen);
                
                //Send first part number
                char *partNum = "3";
                datalen = strlen(partNum);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partNum, datalen);
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                int n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                char *partNum2 = "4";
                datalen = strlen(partNum2);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partNum2, datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part4.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                sock = 0;
                
                
                //END DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                // DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port2);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                std::string partS;
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "4";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "1";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part1.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                std::cout <<"POrt 3 is " << port3 << std::endl;
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port3);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "1";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part1.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "2";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part2.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port4);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "2";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part2.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "3";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                //END DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
            
            if (y == 3) {
                
                struct timeval tv;
                tv.tv_sec = 3;
                
                // DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                int sock = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
                
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port1);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //std::cout << "Uploading part1..." << std::endl;
                
                std::cout << "Sending authentication credentials..." << std::endl;
                std::string credentials = userName + " " + password;
                //std::cout << credentials << std::endl;
                
                std::string request = credentials + '\n' + option;
                
                char req[BUFSIZE];
                strcpy(req, request.c_str());
                
                //Send Reqest <credentials>\n<request>
                int datalen = strlen(req);
                int tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                char *partNum = "2";
                datalen = strlen(partNum);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partNum, datalen);
                
                //Send first file
                datalen = part2.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                int n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                char *partNum2 = "3";
                datalen = strlen(partNum2);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partNum2, datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                sock = 0;
                
                
                //END DFS 1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                // DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port2);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                std::string partS;
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "3";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part3.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part3.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "4";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part4.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                std::cout <<"POrt 3 is " << port3 << std::endl;
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port3);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "4";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part4.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part4.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "1";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part1.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                
                //END DFS 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                // DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                sock = 0;
                //struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port4);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    return -1;
                }
                
                //Send Reqest <credentials>\n<request>
                datalen = strlen(req);
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                //Send first part number
                partS = "1";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, partS.c_str(), datalen);
                
                //Send first file
                datalen = part1.length();
                //std::cout << "File is: " << part1 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part1.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send second part number
                partS = "2";
                datalen = partS.length();
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, partS.c_str(), datalen);
                //std::cout << n << std::endl;
                
                //Send first file
                datalen = part2.length();
                //std::cout << "File is: " << part2 << std::endl;
                tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                n = write(sock, part2.c_str(), datalen);
                //std::cout << n << std::endl;
                
                close(sock);
                //END DFS 4!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }
        
        }
        else if ((option.find("Get")) == 0 or (option.find("get") == 0)) {
            
            
            //std::cout << "Filename is: " << filename << std::endl;
            
            
            int currentPort = 8000;
            std::list <int> partList;
            int count3 = 0;
            std::string fullFile, file1, file2, file3, file4;
            while (currentPort <= 8003) {
                int sock;
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(currentPort);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    std::cout << "Couldn't connect to server " << currentPort - 8000 << "." << std::endl;
                    currentPort++;
                    continue;
                }
                std::string credentials = userName + " " + password;
                std::string request = credentials + "\n" + option;
                char req[BUFSIZE];
                strcpy(req, request.c_str());
                
                //Send Reqest <credentials>\n<request>
                int datalen = strlen(req);
                int tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
                
                int buflen2;
                read(sock, (char*)&buflen2, sizeof(buflen2)); ///read in part num
                buflen2 = ntohl(buflen2);
                char erro1[buflen2];
                read(sock, erro1, buflen2);
                
                if (strcmp(erro1, "-1") == 0) {
                    std::cout << "Incorrect username/password." <<std::endl;
                    break;
                }
                
                std::cout << erro1 << std::endl;
                
                int buflen;
                char serverPart[BUFSIZE];
                int partNumber;
                
                int count2 = 1;
                int partNumberPrev;
                std::string bufferString = "";
                std::string bufferStringPrevious = "";
                while (count2 <= 4) {
                    //std::cout << "-----------------------------------------------------" << std::endl;
                    //std::cout << "In the while loop" << std::endl;
                    //std::cout << "Find is saying: " << find(partList, partNumber) << std::endl;
                    std::cout << "Count is: " << count2++ << std::endl;
                    
                    read(sock, (char*)&buflen, sizeof(buflen)); ///read in part num
                    buflen = ntohl(buflen);
                    int n = read(sock, serverPart, buflen);
                    std::string y(serverPart, sizeof(serverPart));
                    
                    //std::cout << "Server is has part: " << serverPart << std::endl;

                    if (strcmp(serverPart, "-1") == 0) {
                        //std::cout << buffer << std::endl;
                        continue;
                    }
                    

                    partNumberPrev = partNumber;
                    partNumber = stoi(y);
                    
                    //std::cout << "Part number previous is: " << partNumberPrev << std::endl;
                    //std::cout << "Part number current: " << partNumber << std::endl;

                    
                    std::list<int>::iterator it;
                    it = std::find(partList.begin(), partList.end(), partNumber);
                    if (it != partList.end()) {
                        //std::cout << "Client already has " << partNumber << std::endl;
                        char *error = "-1";
                        datalen = sizeof(error);
                        tmp = htonl(datalen);
                        write(sock, (char *)&tmp, sizeof(tmp));
                        write(sock, error, datalen);
                        continue;
                    }
                    
                    char *error = "1";
                    datalen = sizeof(error);
                    tmp = htonl(datalen);
                    write(sock, (char *)&tmp, sizeof(tmp));
                    write(sock, error, datalen);
                    
                    
                    partList.push_back(partNumber);
                    
                    read(sock, (char*)&buflen, sizeof(buflen)); ///Read in file
                    buflen = ntohl(buflen);
                    char buffer[buflen];
                    n = read(sock, buffer, buflen);
                    
                    bufferStringPrevious = bufferString;
                    bufferString = bufferToString(buffer, sizeof(buffer));
                    
                    if (partNumber == 1) {
                        file1 = bufferString;
                    }
                    else if (partNumber == 2) {
                        file2 = bufferString;
                    }
                    else if (partNumber == 3) {
                        file3 = bufferString;
                    }
                    else if (partNumber == 4) {
                        file4 = bufferString;
                        //std::cout << "Last part of file is: " << bufferString << std::endl;
                    }
                    
                    
                    //std::cout << "Buffer is: " << buffer << std::endl;
                    //std::cout << "String is: " << bufferString << std::endl;

                    bzero(buffer, BUFSIZE);
                    //std::cout << "-----------------------------------------------------" << std::endl;
                }
                //close(sock);
                currentPort++;
            }
            
            fullFile.append(file1);
            fullFile.append(file2);
            fullFile.append(file3);
            fullFile.append(file4);
            //std::cout << "Here is your file!" << std::endl;
            /*std::cout << "File 1 is: " << file1 <<std::endl;
            std::cout << "File 2 is: " << file2 <<std::endl;
            std::cout << "File 3 is: " << file3 <<std::endl;
            std::cout << "File 4 is: " << file4 <<std::endl;*/
            
            
            std::list<int>::iterator it1;
            std::list<int>::iterator it2;
            std::list<int>::iterator it3;
            std::list<int>::iterator it4;
            
            it1 = std::find(partList.begin(), partList.end(), 1);
            it1 = std::find(partList.begin(), partList.end(), 2);
            it1 = std::find(partList.begin(), partList.end(), 3);
            it1 = std::find(partList.begin(), partList.end(), 4);
            
            int count5 = 0;
            for (int i: partList) {
                if (i == 1) {
                    count5++;
                }
                if (i == 2) {
                    count5++;
                }
                if (i == 3) {
                    count5++;
                }
                if (i == 4) {
                    count5++;
                }
                //std::cout << i << ", ";
            }
            
            if (count5 == 4) {
                std::cout << fullFile << std::endl;
            }
            else {
                std::cout << "File is incomplete." << std::endl;
            }

            //std::cout << file4 << std::endl;
            std::string test = "Hello, world!";
            std::ofstream finalFile("testFile");
            finalFile << test;


            finalFile.close();
            
        }
        else if ((option.find("list")) == 0) {
            int currentPort = 8000;
            std::string fullList;
            while (currentPort <= 8003) {
                int sock;
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(currentPort);
                serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return 0;
                }
                
                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    //printf("\nConnection Failed \n");
                    currentPort++;
                    continue;
                }
                
                std::string credentials = userName + " " + password;
                std::string request = credentials + "\n" + option;
                char req[BUFSIZE];
                strcpy(req, request.c_str());
                
                //Send Reqest <credentials>\n<request>
                int datalen = strlen(req);
                int tmp = htonl(datalen);
                write(sock, (char *)&tmp, sizeof(tmp));
                write(sock, req, datalen);
    
                int buflen;
                read(sock, (char*)&buflen, sizeof(buflen)); ///Read in list1
                buflen = ntohl(buflen);
                char buffer[buflen];
                read(sock, buffer, buflen);
                //std::cout << "Received: " << buffer << std::endl;
                
                std::string fileList1(buffer, strlen(buffer));
                
                //std::cout << "File list to append is: " << fileList1 << std::endl;
                fullList.append(fileList1);
                //std::cout << "File list after append is: " << fullList << std::endl;

            

                
                currentPort++;
            }
            
            //std::cout << fullList << std::endl;
            
            std::map<std::string,std::set <int>> keepingTrack;
            int counter = 0;
            std::istringstream ss(fullList);
            std::string token, tokenPrev;
            while (std::getline(ss, token, ',')) {
                if (token != "," and (counter % 2) == 0) {
                    //std::cout << token << std::endl;
                }
                else if (token != "," and (counter % 2) == 1) {
                    //std::cout << "Part number " << token << std::endl;
                    //std::cout << "Part number " << stoi(token) << std::endl;
                    
                    //std::cout << "Prev is " << tokenPrev << std::endl;
                    keepingTrack[tokenPrev].insert(stoi(token));
                    //std::cout << "Inserting " << tokenPrev << std::endl;
                    //std::cout << std::to_string(keepingTrack[token]);
                }
                tokenPrev = token;
                counter++;
            }
            std::set <int> checkList;
            checkList.insert(1);
            checkList.insert(2);
            checkList.insert(3);
            checkList.insert(4);
            
            for (auto const x : keepingTrack)
            {
                //std::cout << x.first << ' : ' ;
                if (x.second == checkList) {
                    std::cout << x.first;
                }
                else {
                    std::cout << x.first + "[incomplete]";
                }
                for (auto const y : x.second) {
                    //std::cout << y << ' ';
                }
                std::cout << std::endl ;
            }
            
            
            
            
        }
    }
}
