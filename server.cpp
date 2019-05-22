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
#include "dirent.h"
#include <sys/types.h>
#include <vector>
#include <list>


#define BUFSIZE 1028

std::map<std::string,std::string> userList;

std::vector<std::string> list_dir(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);
    std::vector<std::string> listOfDirectories;

    if (dir == NULL) {
        return listOfDirectories;
    }
    while ((entry = readdir(dir)) != NULL) {
        listOfDirectories.push_back(entry->d_name);
    }
    return listOfDirectories;
    closedir(dir);
}


void connection_handler(int conn_fd, std::string root){
    
    char readMessage[BUFSIZE];
    int buflen; // SIZE!!!!!!!
    
    read(conn_fd, (char*)&buflen, sizeof(buflen));
    buflen = ntohl(buflen);
    read(conn_fd, readMessage, buflen);
        //std::cout << "------------" << std::endl;
    
    std::string request(readMessage, sizeof(readMessage));
    
    //std::cout << readMessage << std::endl;
    
    std::string credentials = request.substr(0, request.find('\n'));
    
    char * temp = strtok(readMessage, "\n");
    temp = strtok(NULL, "\n");
 
    std::string userName, password;
    std::istringstream ss2(credentials);
    ss2 >> userName;
    ss2 >> password;
    
    if (password == userList[userName]) {
        //std::cout << "User " << userName << " authenticated." << std::endl;
    }
    else {
        std::cout << "Unable to authenticate user." << std::endl;
        char *error = "-1";
        int datalen = sizeof(error);
        int tmp = htonl(datalen);
        write(conn_fd, (char *)&tmp, sizeof(tmp));
        write(conn_fd, error, datalen);
        return;
    }
    
    struct stat info;
    std::string pathname = root + userName + "/";
    if(stat(pathname.c_str(), &info) != 0) {
        std::cout << "Filepath does not exist: ", pathname.c_str();
        if (mkdir(pathname.c_str(), 0777) == -1)
            std::cerr << "Error :  " << strerror(errno) << std::endl;
        
        else
            std::cout << "Created home directory for " << userName << std::endl;
    }
    
    
    std::string option(temp, strlen(temp));
    
    if ((option.find("Put") == 0) or (option.find("put") == 0)) {
        std::string filename;
        std::istringstream ss(option);
        ss >> filename;
        ss >> filename;
    
        
        //std::cout << "Option is: " << option << std::endl;
        
        //std::cout << "Request is: " << temp << std::endl;
        //std::cout << "Filename is: " << filename << std::endl;
        
        std::string fileNamePrefix = filename.substr(0, filename.find("."));
        std::string fileNameType = filename.substr(filename.find(".")+1);
        
        //std::cout << "FileNameType----" << fileNameType << "-----" << std::endl;
        //std::cout << "FileNamePrefix: " << fileNamePrefix << std::endl;

        //Read in first part number----------------------------------------------
        read(conn_fd, (char*)&buflen, sizeof(buflen)); ///SIZE MARKER
        buflen = ntohl(buflen);
        //std::cout << buflen << std::endl;
        bzero(readMessage, BUFSIZE);
        int n = read(conn_fd, readMessage, buflen);
        
        std::string partNum(readMessage, sizeof(readMessage));
        std::string finalName;
        //std::cout << fileNamePrefix << std::endl;
        finalName = root + userName + "/" + filename + "." + partNum; //insert 1 into filename

        std::cout << "Final name for writing is: " << finalName << std::endl;
        
        bzero(readMessage, sizeof(readMessage));
        //-------------------------------------------------------------
        
        
        //read in first file
        read(conn_fd, (char*)&buflen, sizeof(buflen)); ///receive part 1 of file
        buflen = ntohl(buflen);
        char part1[buflen];
        //std::cout << "Received " << buflen << " buflen" << std::endl;
        n = read(conn_fd, part1, buflen);
        
        //std::cout << "------Received----" << std::endl;
        //std::cout << part1 << std::endl;
        //std::cout << "-----------------------" << std::endl;
        std::ofstream writeFile;
        writeFile.open(finalName);
        //std::cout << "Just opened " << finalName << std::endl;
        writeFile.write(part1, strlen(part1));
        writeFile.close();

        
        bzero(readMessage, BUFSIZE);
        //Read in second file part number
        read(conn_fd, (char*)&buflen, sizeof(buflen)); ///SIZE MARKER
        buflen = ntohl(buflen);
        char part2[buflen];
        n = read(conn_fd, part2, buflen);
        
        std::string partNum2(part2, sizeof(part2));
        finalName = root + userName + "/" + filename + "." + partNum2; //insert 1 into filename
        //std::cout << "Final name for writing is: " << finalName <<std::endl;
        
        
        bzero(part2, buflen);
        read(conn_fd, (char*)&buflen, sizeof(buflen)); ///SIZE MARKER
        buflen = ntohl(buflen);
        char filePart2[buflen];
        //std::cout << "Received " << buflen << " datalen" << std::endl;
        n = read(conn_fd, filePart2, buflen);
        
        //std::cout << "------Received----" << std::endl;
        //std::cout << filePart2 << std::endl;
        //std::cout << "-----------------------" << std::endl;
        std::ofstream writeFile2;
        writeFile2.open(finalName);
        //std::cout << "Just opened " << finalName << std::endl;
        writeFile2.write(filePart2, strlen(filePart2));
        writeFile2.close();
    }
    else if ((option.find("Get") == 0) or (option.find("get") == 0)) {
        std::cout << "Made it into get!" << std::endl;
        std::string filename;
        std::istringstream ss(option);
        ss >> filename;
        ss >> filename;
        std::string fileNamePrefix = filename.substr(0, filename.find("."));
        std::cout << "File name is " << filename << std::endl;

        std::ifstream writeFile;
        std::string line, wholePart;
        for (int i = 1; i < 5; i++) {
            std::cout << "Part is: " << i << std::endl;
            writeFile.open(root + userName + "/" + filename + "." + std::to_string(i));
            std::cout << "-----------" << root + userName + "/" + filename + "." + std::to_string(i) << "-----------" << std::endl;
            char output[4096];
            writeFile.read(output, 4096);
            //std::cout << output << std::endl;
            //std::cout << "-----------------------------------" << std::endl;
            if (strcmp(output, "") == 0) {
                std::cout << "Wasnt found on server" << std::endl;
                char *error = "-1";
                int datalen = sizeof(error);
                int tmp = htonl(datalen);
                write(conn_fd, (char *)&tmp, sizeof(tmp));
                write(conn_fd, error, datalen);
                continue;
            }
            writeFile.close();
        
            int datalen = sizeof(i);
            int tmp = htonl(datalen);
            write(conn_fd, (char *)&tmp, sizeof(tmp));
            write(conn_fd, (std::to_string(i)).c_str(), datalen);
            std::cout << "Just sent part number: " << i << std::endl;
            
            read(conn_fd, (char*)&buflen, sizeof(buflen));
            buflen = ntohl(buflen);
            read(conn_fd, readMessage, buflen);
            
            if (strcmp(readMessage, "-1") == 0) {
                std::cout << "Client doesnt need this file." << std::endl;
                continue;
            }
            else {
                datalen = sizeof(output);
                tmp = htonl(datalen);
                write(conn_fd, (char *)&tmp, sizeof(tmp));
                write(conn_fd, output, datalen);
                //std::cout << "Just sent poem: " << output << std::endl;
                
                bzero(output, BUFSIZE);
            }
        }
    }
    else if ((option.find("list") == 0)) {
        std::cout << "Hi im in LIST!" << std::endl;
        std::cout << "Request was " << option << std::endl;
        //char command[50] = "ls -l";
        //system(command);
        std::string currentDir = "./" + root + userName + "/";
        //std::cout << currentDir << std::endl;
        char currentDirBuffer[BUFSIZE];
        strcpy(currentDirBuffer, currentDir.c_str());
        std::vector<std::string> fileList = list_dir(currentDirBuffer);
        /*int datalen = sendString.length();
        int tmp = htonl(datalen);
        write(conn_fd, (char *)&tmp, sizeof(tmp));
        write(conn_fd, sendString.c_str(), datalen);*/
        std::list <std::string> partList;
        std::string title, partNumber, sendString;
        int count = 0;
        for (auto i = fileList.begin(); i != fileList.end(); ++i) {
            if (count != 0 and count != 1) {
                std::cout << *i << std::endl;
                title = (*i).substr(0, (*i).find("."));
                partNumber = (*i).back();
                //std::cout << "Title is " << title << std::endl;
                //std::cout << "Part number is " << partNumber << std::endl;
                sendString = sendString + title + "," + partNumber + ",";
            }
            count++;
        }
        std::cout << sendString << std::endl;
        
        char listOfFiles[BUFSIZE];
        strcpy(listOfFiles, sendString.c_str());
        int datalen = strlen(listOfFiles);
        int tmp = htonl(datalen);
        write(conn_fd, (char *)&tmp, sizeof(tmp));
        write(conn_fd, listOfFiles, datalen);
        
        
    }
    int err = close(conn_fd);
    //std::cout << err << std::endl;
}


int main (int argc, char * argv[]) {
    
    std::ifstream myReadFile;
    myReadFile.open("dfs.conf");
    std::string line, username, password;
    while (getline(myReadFile, line)) {
        std::istringstream ss(line);
        ss >> username;
        ss >> password;
        userList[username] = password;
    }
    
    int port;
    std::string root = argv[1];
    struct sockaddr_in servaddr;
    int sock_fd, conn_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    std::string portS = argv[2];
    port = std::stoi(portS);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(port);
    
    bind(sock_fd,(struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(sock_fd, 128);
    
    //std::cout << "Started proxy with:" << std::endl << "host - localhost" << std::endl << "port - "<< port << std::endl;
    while(1) {
        std::cout << "Waiting for another connection..." << std::endl;
        conn_fd = accept(sock_fd,(struct sockaddr*) NULL, NULL);
        std::thread threader(connection_handler, conn_fd, root);
        threader.join();
    }
    close(sock_fd);

}
