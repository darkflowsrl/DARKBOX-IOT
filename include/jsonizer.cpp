#include <iostream>
#include <string.h>
#include <vector>

class JSONIZER{
    private:
        std::string launcher = "{";
        bool doEnd = true; 
/*By conclution the first element will be started like it where a previous element
Because of that doEnd strarts at false */
    public:
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