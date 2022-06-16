#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>

std::string readFileIntoString(const std::string& path) {
    auto file = std::ostringstream{};
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '"
             << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    file << input_file.rdbuf();
    return file.str();
}

std::string path_ = ".\\config.json";
std::string configFile = readFileIntoString(path_); 
const char* configData = configFile.c_str(); // configArray is the array from the string that hosts the json document



class JSONIZER{
    private:
        std::string launcher = "{";
        bool doEnd = true; 
/*By conclution the first element will be started like it where a previous element
Because of that doEnd strarts at false */
    public:
        std::vector<std::string> toVECTOR(std::string charArray){
            std::string emptyString = "";
            std::vector<std::string> configVector;
            bool isSecond = false;
            for(int i = 0; i < charArray.length(); i++){
                if(charArray.at(i) != '{' || charArray.at(i) != '}' || charArray.at(i) != ':'){
                    if(isSecond == false){ 
                        emptyString.push_back(charArray.at(i));

                    }
                }

            }
            return configVector;
        }
        std::string toSJSON(std::vector<std::string> element){
            int capacity = element.size();
            for(int i = 1; i <= capacity; i++){
                int evenOdd = capacity % 2;
                if(evenOdd != 0 & doEnd == true){
                    launcher += "\"" + element.at(i-1) + "\"" + ":";
                    doEnd = false;
                }else if(evenOdd == 0 & doEnd == false & i != capacity){
                    launcher += "\"" + element.at(i-1) + "\", ";
                    doEnd = true;
                }else{
                    launcher += "\"" + element.at(i-1) + "\" }";
                }
            }
            return launcher;
        }
};

int main(){
    std::vector<std::string> myVector;
    myVector.push_back("temp1");
    myVector.push_back("5454");
    myVector.push_back("temp2");   
    myVector.push_back("123");

    JSONIZER JSONsession;
    std::cout<<JSONsession.toSJSON(myVector);
    return 0;
}