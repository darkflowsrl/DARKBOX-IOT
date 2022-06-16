#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

string readFileIntoString(const string& path) {
    auto file = ostringstream{};
    ifstream input_file(path, ios::in);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    file << input_file.rdbuf();
    return file.str();
}



int main(){ 

    string path_ = ".\\config.json";
    string configFile = readFileIntoString(path_); 
    const char* configArray = configFile.c_str(); // configArray is the array from the string that hosts the json document
    cout<<configFile;

    return 0;
}

