#ifndef JSONIZER_H
#define JSONIZER_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>

/* The class JSONIZER will be used for serialize and deserialize JSON files, this will be the
class used by the ESP32 firmware to deserialize the config file.
To seriliaze you will have to store the data into a vector an then, pass the vector through
the toJSON function of the class
This class has three methods.
@giulicrenna*/

class JSONIZER{
    private:
        bool isSecond = false;
        std::string tempString = "";
        std::vector<std::string> configVector;
        /*By conclution the first element will be started like it where a previous element
        Because of that doEnd strarts at false */
    public:
        /* readFileIntoString: const string -> string 
        This function is used to turn the content from a .json file to a string */
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
        /* toVECTOR: string -> vector<string>
        This functions is used to turn a char Array (wich contains the json) into a vector
        where the odd indexes are the json variable and the next index (obviuosly even) will that variable value*/
        std::vector<std::string> toVECTOR(std::string charArray){
            for(int i = 0; i < static_cast<int>(charArray.length()); i++){
                if(charArray.at(i) != '{'){
                    if(isSecond == false){ 
                        if(charArray.at(i) == ':'){
                            configVector.push_back(tempString);
                            isSecond = true;
                            tempString.clear();
                        }else if(charArray.at(i) != ':'){
                            tempString.push_back(charArray.at(i));
                        }
                    }else if(isSecond == true){ //even odd
                        if(charArray.at(i) == ','){
                            configVector.push_back(tempString);
                            isSecond = false;
                            tempString.clear();
                        }else if(charArray.at(i) != ',' && charArray.at(i) != '}'){
                            tempString.push_back(charArray.at(i));
                        }else if(charArray.at(i) == '}'){
                            configVector.push_back(tempString);
                        tempString.clear();
                        }
                    }else{
                        std::cout<<"Error";
                    }
                }
            }
            return configVector;
        }
        /* toJSON: vector<string> -> string
        inversely to the "toVECTOR" metehod, this take a vector as parameter and 
        return you a string formatted as a json file
        PD: toSJSON means ToStringJson*/
        std::string toSJSON(std::vector<std::string> myVector){  
            std::string launcher = "{";
            bool doEnd = true;
             
            int capacity = myVector.size();
            for(int i = 1; i <= capacity; i++){
                float evenOdd = i % 2;
                if(evenOdd != 0 && doEnd == true){ //par
                    launcher += "\"" + myVector.at(i-1) + "\"" + ":";
                    doEnd = false;
                }else if(evenOdd == 0 && doEnd == false && i != capacity){ //impar
                    launcher += "\"" + myVector.at(i-1) + "\", ";
                    doEnd = true;
                }else{
                    launcher += "\"" + myVector.at(i-1) + "\"}";
                }
            }
            return launcher;
        }
};

#endif