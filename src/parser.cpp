#include "../include/parser.hpp"
#include<stdexcept>

int extractContentLength(const std::string& header){

    std::string target = "Content-Length: ";

    size_t pos = header.find(target);

    if(pos == std::string::npos){
        return 0;
    }

    size_t start_of_numbers = pos + target.length();

    size_t end_of_line = header.find("\r", start_of_numbers);

    std::string number_text = header.substr(start_of_numbers, end_of_line - start_of_numbers);

    try{
        return std::stoi(number_text);
    } catch ( const std::invalid_argument& e){
        return 0;
    }

}