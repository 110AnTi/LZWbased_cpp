#include <iostream>
#include <map>
#include <string>
#include <vector>

void initTable(std::map<std::string, int>& m);
void compressMessage(std::string message, std::vector<int>& v);
void decompressMessage(std::vector<int> v);
bool isExistInTable(std::string s, std::map<std::string, int> m);
bool isCodeExistInTable(int n, std::map<std::string, int> m);
std::string transCode(int n, std::map<std::string, int>& m);

void initTable(std::map<std::string, int>& m){
    for(int i=0; i<256; i++){
        std::string s;
        s += char(i);
        m.insert(std::pair<std::string, int>(s, i));
    }
}

void compressMessage(std::string message, std::vector<int>& v){
    std::map<std::string, int> m;
    initTable(m);

    std::string p = message.substr(0, 1);
    std::map<std::string, int>::iterator itr;
    std::string string256th = message.substr(0, 2);
    std::string c;

    // for the last string in message, if p = last string then c = '', so that we cant extract
    //the code of last string because p+c is in the table
    for(int i=1; i<=message.size(); i++){
        if(i != message.size()) c = message.substr(i, 1);
        else c = string256th;
        bool inTable = isExistInTable(p+c, m);
        if(inTable){
            p += c;
        }else{
            for(itr=m.begin(); itr!=m.end(); itr++){
                if(itr->first.compare(p) == 0){
                    std::cout << itr->second << " " << itr->first<<"\n";
                    v.push_back(itr->second);
                    break;
                }
            }
            m.insert(std::pair<std::string, int>(p+c, m.size()));
            p = c;
        }
    }
}

bool isExistInTable(std::string s, std::map<std::string, int> m){
    bool inTable = false;
    std::map<std::string, int>::iterator itr;

    for(itr=m.begin(); itr!=m.end(); ++itr){
            if(itr->first.compare(s) == 0){
                inTable = true;
                break;
        }
    }
    return inTable;
}

bool isCodeExistInTable(int n, std::map<std::string, int> m){
    bool inTable = false;
    std::map<std::string, int>::iterator itr;

    for(itr=m.begin(); itr!=m.end(); ++itr){
            if(itr->second == n){
                inTable = true;
                break;
        }
    }
    return inTable;
}

void decompressMessage(std::vector<int> v){
    std::map<std::string, int> m;
    initTable(m);
    
    std::string message;

    int oldCode = v[0];
    std::cout << transCode(oldCode, m);
    std::string c="", s;

    for(int i=1; i<v.size(); i++){
        int newCode = v[i];
        if(isCodeExistInTable(newCode, m)){
            s = transCode(newCode, m);
        }else{
            s = transCode(oldCode, m)+c;
        }
        std::cout << s;
        c = s.substr(0, 1);
        m.insert(std::pair<std::string, int>(transCode(oldCode, m)+c, m.size()));
        oldCode = newCode;
    }
}

std::string transCode(int n, std::map<std::string, int>& m){
    std::map<std::string, int>::iterator itr;

    for(itr=m.begin(); itr!=m.end(); ++itr){
        if(itr->second == n) return itr->first;
    }
    return "-1";
}

int main(){
    std::vector<int> v;

    compressMessage("ABCABDABABDB", v);
    decompressMessage(v);
    
    return 0;
}
