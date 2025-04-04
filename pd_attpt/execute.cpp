#include <windows.h>
#include <filesystem>
#include <string>
#include <iostream>

using namespace std;

int main(){
    char CurPath[MAX_PATH];
    GetModuleFileNameA(NULL, CurPath, MAX_PATH);

    string fullPath(CurPath);
    // std::cout << "完整路徑: " << fullPath << std::endl;

    size_t pos = fullPath.find_last_of("\\/");
    string folderPath = fullPath.substr(0, pos);

    // cout<<folderPath<<'\n';

    string cmd = "C:\\Windows\\System32\\conhost.exe cmd /k " + folderPath + "\\BfsDfsSimulator.exe";
    system(cmd.c_str());
    
}   