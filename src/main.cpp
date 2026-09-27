#include<iostream>
#include<string>

#include "../include/parser.hpp"
#include "../include/server.hpp"


int main(){

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr std::size_t max_header_bytes = 8 * 1024;
    constexpr std::size_t max_payload_bytes = 64 * 1024 * 1024;


    LanguageServer Server;


    while(true){
        std::string header;
        char ch ;
        bool header_complete = false;

        while(std::cin.get(ch)){
            header.push_back(ch);

            if (header.size() >  max_header_bytes ){
                std::cerr << "LSP header exceed size limit\n";
                return 1;
            }
            if (header.size() >= 4 &&
                header.compare(header.size() - 4, 4, "\r\n\r\n") == 0 ){
                header_complete = true;
                break;
                }

        }

        if (!header_complete) {
            if (!header.empty()) {
                std::cerr << "Incomplete LSP header\n";
                return 1;
            }
            break;
        }



        const int content_length = extractContentLength(header);
        if (content_length <= 0 ||
            static_cast<std::size_t>(content_length) > max_payload_bytes){
            std::cerr << "Invalid or oversized LSP Content-length\n";
            return 1;
        }

        std::string json_payload(static_cast<std::size_t>(content_length), '\0');
        std::cin.read(
            json_payload.data(),
            static_cast<std::streamsize>(json_payload.size())
        );

        if (std::cin.gcount() != static_cast<std::streamsize>(json_payload.size())){
            std::cerr << "Incomplete LSP message body\n";
            return 1;
        }


        Server.routesLspMessage(json_payload);

        if (Server.shouldExit()){
            break;
        }
    }

    return 0;
}