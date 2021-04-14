#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>

void initTable(std::map<std::string, int>& m);
void compressMessage(std::string message, std::vector<int>& v);
void decompressMessage(std::vector<int> v);
bool isExistInTable(std::string s, std::map<std::string, int> m);
bool isCodeExistInTable(int n, std::map<std::string, int> m);
std::string transCode(int n, std::map<std::string, int>& m);
std::string codeToBits(std::vector<int> v, int maxLen);
int maxLenBit(std::vector<int> v);
std::string readStringFile(std::string filename);
int binaryToDecimal(std::string binary);
void hideInformation(std::string bits, std::string imgName, std::string& maintainMatrix);
void hideInColor(int& colorValue, std::string& bits, std::string& maintainMatrix);
int evalColorValue(int colorValue, int bit, std::string& maintainMatrix);
std::string decimalToBinary(int n);
void hideExtraInformation(std::string maintainMatrix, int maxLen);
void getExtraInformation(int& maxLen, std::string& maintainMatrix);
void getInformation(std::string& bits, std::string& maintainMatrix);
void reEvalColorValue(int colorValue, std::string& maintainMatrix, std::string& bits);

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
    std::cout << "Compressed code: ";
    for(int i=1; i<=message.size(); i++){
        if(i != message.size()) c = message.substr(i, 1);
        else c = string256th;
        bool inTable = isExistInTable(p+c, m);
        if(inTable){
            p += c;
        }else{
            for(itr=m.begin(); itr!=m.end(); itr++){
                if(itr->first.compare(p) == 0){
                    std::cout << itr->second << " ";
                    v.push_back(itr->second);
                    break;
                }
            }
            m.insert(std::pair<std::string, int>(p+c, m.size()));
            p = c;
        }
    }
    std::cout << "\n";
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
    message += transCode(oldCode, m);
    std::string c="", s;

    for(int i=1; i<v.size(); i++){
        int newCode = v[i];
        if(isCodeExistInTable(newCode, m)){
            s = transCode(newCode, m);
        }else{
            s = transCode(oldCode, m)+c;
        }
        message += s;
        c = s.substr(0, 1);
        m.insert(std::pair<std::string, int>(transCode(oldCode, m)+c, m.size()));
        oldCode = newCode;
    }

    std::cout << "Decompressed code: "<< message << "\n\n";
}

std::string transCode(int n, std::map<std::string, int>& m){
    std::map<std::string, int>::iterator itr;

    for(itr=m.begin(); itr!=m.end(); ++itr){
        if(itr->second == n) return itr->first;
    }
    return "-1";
}

std::string codeToBits(std::vector<int> v, int maxLen){
    std::string bits;
    for(int i=0; i<v.size(); i++){
        std::string bit;
        switch(maxLen){
            case 7:
                bit = std::bitset<7>(v[i]).to_string();
                break;
            case 8:
                bit = std::bitset<8>(v[i]).to_string();
                break;
            case 9:
                bit = std::bitset<9>(v[i]).to_string();
                break;
            case 10:
                bit = std::bitset<10>(v[i]).to_string();
                break;
            default:
                std::cout << "error!";
                break;
        }
        bits += bit;
    }
    std::cout << "Compressed value: \t" << bits << "\n";
    return bits;
}

int maxLenBit(std::vector<int> v){
    int max = -1;
    for(int i=0; i<v.size(); i++){
        if(max < v[i]) max = v[i];
    }
    int len = 1;
    while(max/2) {
        len++;
        max /= 2;
    }
    return len;
}

std::string readStringFile(std::string filename){
    std::string line, message;
    std::ifstream f;
    f.open(filename);
    if(f.is_open()){
        while(getline(f, line)) message += line;
        f.close();
    }
    else{
        std::cout << "Unable to open file " << filename << "\n";
    }
    return message;
}

int binaryToDecimal(std::string binary){
    int bin = stoi(binary);
    int dec=0, i=0, remainder;
    while(bin!=0){
        remainder = bin%10;
        bin /= 10;
        dec += remainder*pow(2, i);
        ++i;
    }
    return dec;
}

void hideInformation(std::string bits, std::string imgName, std::string& maintainMatrix){
    cv::Mat img = cv::imread(imgName);

    for(int i=0; i<img.rows; i++){
        if(bits.length() == 0) break;
        for(int j=0; j<img.cols; j++){
            cv::Vec3b intensity = img.at<cv::Vec3b>(i, j);
            int blue = (int)intensity.val[0];
            int green = (int)intensity.val[1];
            int red = (int)intensity.val[2];

            if(bits.length() != 0){
                hideInColor(blue, bits, maintainMatrix);
                intensity.val[0] = blue;
            } 
            if(bits.length() != 0){
                hideInColor(green, bits, maintainMatrix);
                intensity.val[1] = green;
            } 
            if(bits.length() != 0){
                hideInColor(red, bits, maintainMatrix);
                intensity.val[2] = red;
            }
            img.at<cv::Vec3b>(i, j) = intensity;
            if(bits.length() == 0) break;
        }
    }
    cv::imwrite("getSteged.jpg", img);
}

void hideInColor(int& colorValue, std::string& bits, std::string& maintainMatrix){
    // std::cout << "Bits: " << bits << "\n";
    int firstBit = (int)bits.at(0) - (int)'0';
    int bitsToHide = evalColorValue(colorValue, firstBit, maintainMatrix);
    std::string hidedBits = bits.substr(0, bitsToHide);

    // int decTemp = colorValue >> bitsToHide;
    // std::string binTemp = decimalToBinary(decTemp);
    // binTemp += hidedBits;
    // colorValue = binaryToDecimal(binTemp);

    int leftBits = 8 - hidedBits.length();
    // std::cout << "leftBits: " << leftBits << "\n";
    std::string tempBin = std::bitset<8>(colorValue).to_string().substr(0, leftBits);
    // std::cout << "tempBin before: " << tempBin << "\n";
    tempBin += hidedBits;
    // std::cout << "colorValue: " << std::bitset<8>(colorValue).to_string() << "\n";
    // std::cout << "tempBin: " << tempBin << "\n";
    colorValue = binaryToDecimal(tempBin);
    // std::cout << "colorValue after: " << std::bitset<8>(colorValue).to_string() << "\n";

    // size_t pos = bits.find(hidedBits);
    // if(pos != std::string::npos){
    //     bits.erase(pos, hidedBits.length());
    // }
    bits.erase(0, bitsToHide);
}

int evalColorValue(int colorValue, int bit, std::string& maintainMatrix){
    switch(bit){
        case 1:
            if(colorValue >= 240 && colorValue <= 255){
                maintainMatrix += "1";
                return 5;
            }else if(colorValue >= 224 && colorValue <= 239){
                maintainMatrix += "0";
                return 3;
            }else if(colorValue >= 192 && colorValue <= 223){
                maintainMatrix += "0";
                return 2;
            }else if(colorValue >= 32 && colorValue <= 191){
                maintainMatrix += "0";
                return 1;
            }else if(colorValue >= 16 && colorValue <= 31){
                maintainMatrix += "0";
                return 2;
            }else{
                maintainMatrix += "0";
                return 4;
            }
            break;
        case 0:
            if(colorValue >= 240 && colorValue <= 255){
                maintainMatrix += "0";
                return 4;
            }else if(colorValue >= 224 && colorValue <= 239){
                maintainMatrix += "1";
                return 5;
            }else if(colorValue >= 192 && colorValue <= 223){
                maintainMatrix += "1";
                return 3;
            }else if(colorValue >= 32 && colorValue <= 191){
                maintainMatrix += "1";
                return 2;
            }else if(colorValue >= 16 && colorValue <= 31){
                maintainMatrix += "1";
                return 3;
            }else{
                maintainMatrix += "1";
                return 5;
            }
        default:
            std::cout << "Something wrong!!!\n";
            break;
    }
    return -1;
}

std::string decimalToBinary(int n)
{
    std::string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    return r;
}

void hideExtraInformation(std::string maintainMatrix, int maxLen){
    std::string maxLenStr = std::bitset<16>(maxLen).to_string();
    std::string extraInfo = maxLenStr + maintainMatrix;
    char* extraInfoChar = &extraInfo[0];

    std::fstream stegFile;
    stegFile.open("getSteged.jpg", std::ios::in|std::ios::out|std::ios::ate);

    stegFile.write(extraInfoChar, extraInfo.length());
    delete[] extraInfoChar;
    stegFile.close();
}

void getExtraInformation(int& maxLen, std::string& maintainMatrix){
    std::fstream stegFile;
    std::string revExtraInformation;
    stegFile.open("getSteged.jpg");
    char ch;
    int pos;
    stegFile.seekg(-1,std::ios::end);
    pos=stegFile.tellg();
    for(int i=0;i<pos;i++)
    {
        ch=stegFile.get();

        if((int)ch == -39){
            stegFile.seekg(-2,std::ios::cur);
            ch = stegFile.get();
            if((int)ch == -1) break;
        }else{
            revExtraInformation += ch;
        }

        stegFile.seekg(-2,std::ios::cur);
    }
    stegFile.close();

    // remove the eof
    // revExtraInformation.erase(0, 1);
    
    std::string extraInformation = std::string(revExtraInformation.rbegin(), revExtraInformation.rend());
    maxLen = binaryToDecimal(extraInformation.substr(0, 16));
    size_t position = extraInformation.find(extraInformation.substr(0, 16));
    if(position != std::string::npos){
        extraInformation.erase(position, 16);
    }
    maintainMatrix = extraInformation;

    // std::cout << "Maintain Matrix: " << maintainMatrix << "\n";
    // std::cout << "maxLen: " << maxLen << "\n";
}

void getInformation(std::string& bits, std::string& maintainMatrix){
    cv::Mat img = cv::imread("getSteged.jpg");

    for(int i=0; i<img.rows; i++){
        if(maintainMatrix.length() == 0) break;
        for(int j=0; j<img.cols; j++){
            cv::Vec3b intensity = img.at<cv::Vec3b>(i, j);
            // std::cout << intensity << "\n";
            int blue = (int)intensity.val[0];
            int green = (int)intensity.val[1];
            int red = (int)intensity.val[2];

            reEvalColorValue(blue, maintainMatrix, bits);
            if(maintainMatrix.length() == 0) break;
            reEvalColorValue(green, maintainMatrix, bits);
            if(maintainMatrix.length() == 0) break;
            reEvalColorValue(red, maintainMatrix, bits);
            if(maintainMatrix.length() == 0) break;
        }
    }
}

void reEvalColorValue(int colorValue, std::string& maintainMatrix, std::string& bits){
    std::string bit = maintainMatrix.substr(0, 1);
    std::cout << "bit: " << bit <<"\n";
    maintainMatrix.erase(0, 1);
    switch (stoi(bit))
    // switch(1)
    {
    case 1:
        if(colorValue >= 240 && colorValue <= 255){
            bits += std::bitset<8>(colorValue).to_string().substr(3, 5);
        }
        else if(colorValue >= 224 && colorValue <= 239){
            bits += std::bitset<8>(colorValue).to_string().substr(3, 5);
        }
        else if(colorValue >= 192 && colorValue <= 223){
            bits += std::bitset<8>(colorValue).to_string().substr(5, 3);
        }
        else if(colorValue >= 32 && colorValue <= 191){
            bits += std::bitset<8>(colorValue).to_string().substr(6, 2);
        }
        else if(colorValue >= 16 && colorValue <= 31){
            bits += std::bitset<8>(colorValue).to_string().substr(5, 3);
        }
        else if(colorValue >= 0 && colorValue <= 15){
            bits += std::bitset<8>(colorValue).to_string().substr(3, 5);
        }
        break;
    case 0:
        if(colorValue >= 240 && colorValue <= 255){
            bits += std::bitset<8>(colorValue).to_string().substr(4, 4);
        }
        else if(colorValue >= 224 && colorValue <= 239){
            bits += std::bitset<8>(colorValue).to_string().substr(5, 3);
        }
        else if(colorValue >= 192 && colorValue <= 223){
            bits += std::bitset<8>(colorValue).to_string().substr(6, 2);
        }
        else if(colorValue >= 32 && colorValue <= 191){
            bits += std::bitset<8>(colorValue).to_string().substr(7, 1);
        }
        else if(colorValue >= 16 && colorValue <= 31){
            bits += std::bitset<8>(colorValue).to_string().substr(6, 2);
        }
        else if(colorValue >= 0 && colorValue <= 15){
            bits += std::bitset<8>(colorValue).to_string().substr(4, 4);
        }
        break;
    default:
        std::cout << "Something wrong!!!\n";
        break;
    }
}

void splitBits(int maxLen, std::string& bits){
    std::string message;
    while(bits.length() > 0){
        std::string arrBit = bits.substr(0, maxLen);
        message += binaryToDecimal(arrBit);

        bits.erase(0, maxLen);
    }
    std::cout << "message : " << message << "\n";
}

void uncompressedBinary(std::string message){
    std::string bits;

    for(char& c:message){
        bits += std::bitset<8>(c).to_string();
    }

    std::cout << "Uncompressed value: \t" << bits << "\n";
}

int main(){
    std::vector<int> v;
    std::string message = readStringFile("message.txt");
    std::cout << "\nOriginal message: " << message << "\n";

    compressMessage(message, v);

    int maxLen = maxLenBit(v);
    std::string bits = codeToBits(v, maxLen);
    uncompressedBinary(message);

    decompressMessage(v);

    // std::string maintainMatrix;
    // hideInformation(bits, "animal.jpg", maintainMatrix);
    // hideExtraInformation(maintainMatrix, maxLen);

    // std::cout << "Maintain Matrix: " << maintainMatrix << "\n";

    // int revMaxLen; 
    // std::string revMaintainMatrix, revBits;
    // getExtraInformation(revMaxLen, revMaintainMatrix);
    // getInformation(revBits, revMaintainMatrix);

    // std::cout << "revBits: " << revBits << "\n";

    // splitBits(revMaxLen, revBits);
    
    return 0;
}
