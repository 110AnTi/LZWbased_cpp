#include <iostream>
#include <map>
#include <string>
#include <vector>

void initTable(std::map<std::string, int>& m);
void compressMessage(std::string message, std::map<std::string, int>& m);

void initTable(std::map<std::string, int>& m){
    for(int i=0; i<256; i++){
        std::string s;
        s += char(i);
        m.insert(std::pair<std::string, int>(s, i));
    }
}

void compressMessage(std::string message, std::map<std::string, int>& m){
    std::string p = message.substr(0, 1);
    std::map<std::string, int>::iterator itr;
    std::string string256th = message.substr(0, 2);
    std::string c;
    // for the last string in message, if p = last string then c = '', so that we cant extract
    //the code of last string because p+c is in the table

    for(int i=1; i<=message.size(); i++){
        bool inTable = false;
        if(i != message.size()) c = message.substr(i, 1);
        else c = string256th;
        for(itr=m.begin(); itr!=m.end(); ++itr){
            if(itr->first.compare(p+c) == 0){
                inTable = true;
                break;
            }
        }
        if(inTable){
            p += c;
        }else{
            for(itr=m.begin(); itr!=m.end(); itr++){
                if(itr->first.compare(p) == 0){
                    std::cout << itr->second << " " << itr->first<<"\n";
                    break;
                }
            }
            m.insert(std::pair<std::string, int>(p+c, m.size()));
            p = c;
        }
    }
}

int main(){
    std::map<std::string, int> m;
    initTable(m);

    compressMessage("ABCBCABCABCD", m);
    
    return 0;
}