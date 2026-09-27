#include "../include/server.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>



void LanguageServer::routesLspMessage(const std::string& raw_json){
    try{

        auto msg = json::parse(raw_json);

        if (!msg.contains("jsonrpc") || msg["jsonrpc"] != "2.0")
            return;

        if (msg.contains("method")) {
            std::string method = msg["method"];

            if (method == "initialize"){
               handleInitialize(msg);

            } else if (method == "textDocument/didOpen"){
                handleTextDocumentSync(method, msg["params"]);

            } else if (method == "textDocument/didChange"){
                handleTextDocumentSync(method, msg["params"]);

            } else if (method == "textDocument/completion"){
                handleCompletion(msg);

            } else if (method == "shutdown"){
                handleShutdown(msg);

            } else if (method == "exit"){
                handleExit();
            }
        }
    } catch (const json::exception& e){

        std::cerr << "LSP Router Error: " << e.what() << "\n";
    }
}

void LanguageServer::handleInitialize(const json& request){

    json requestId = request.value("id", json(nullptr));
    json response;
    response["jsonrpc"] = "2.0";
    response["id"] = requestId;

    json capabilities;

    capabilities["textDocumentSync"] = 1;

    // NOTE: LSP trigger characters must each be a single character.
    // "->" won't be honored by most clients as a multi-char trigger;
    // ">" alone will fire when the user types the second half of "->".
    capabilities["completionProvider"] = {
        {"resolveProvider", false},
        {"triggerCharacters", {".", ">", ":"}}
    };

    response["result"] = {
        {"capabilities", capabilities}
    };

    sendResponse(response);

}

void LanguageServer::handleCompletion(const json& request){
    json requestId = request.contains("id") ? request["id"] : nullptr;

    json completion_list = json::array();

    json item;

    item["label"] = "CodingChallenges";
    item["kind"] = 14;
    item["documentation"] = "Accelerate your programming skills by building real-world projects.";
    item["insertText"] = "CodingChallenges";
    completion_list.push_back(item);

    json response;
    response["jsonrpc"] = "2.0";
    response["id"] = requestId;
    response["result"] = completion_list;

    sendResponse(response);
}




void LanguageServer::handleTextDocumentSync(const std::string& method, const json& params){
    std::string uri = params["textDocument"]["uri"];
    std::string text;

    if (method == "textDocument/didOpen"){
        text = params["textDocument"]["text"];
    } else if (method == "textDocument/didChange"){
        // textDocumentSync = 1 (Full) means contentChanges[0].text is the
        // whole document. If you switch to incremental sync (2) later,
        // this needs to apply ranged edits instead of taking text wholesale.
        if (!params["contentChanges"].empty()){
            text = params["contentChanges"][0]["text"];
        }
    } else {
        std::cerr << "handleTextDocumentSync called with unexpected method: " << method << "\n";
        return;
    }

    std::string target = "Watching a Video";
    std::string feedback = "It's much better to learn by doing! Try Coding Challenges instead.";

    int severity = 3;

    runDiagnostics(uri, text, target, feedback, severity);
}

void LanguageServer::runDiagnostics(const std::string& uri, const std::string& text, const std::string& target_phrase,const std::string& message, int severity){

    if (target_phrase.empty()) return;

    json diagnostics_array = json::array();
    std::istringstream stream(text);
    std::string line;
    int current_line = 0;

    while(std::getline(stream, line)){
        size_t character_offset = line.find(target_phrase);

        while(character_offset != std::string::npos){
            size_t start_char = character_offset;
            size_t end_char = start_char + target_phrase.length();

            json diagnostics;

            diagnostics["range"] = {
                {"start", {{"line", current_line}, {"character", start_char}}},
                {"end", {{"line", current_line}, {"character", end_char}}}
            };

            diagnostics["severity"] = severity;
            diagnostics["source"] = "cclsp";
            diagnostics["message"] = message;

            diagnostics_array.push_back(diagnostics);

            character_offset = line.find(target_phrase, end_char);

        }
        current_line++;
    }

    json notification;


    notification["jsonrpc"] = "2.0";
    notification["method"] = "textDocument/publishDiagnostics";

    notification["params"] = {
        {"uri", uri},
        {"diagnostics", diagnostics_array}
    };

    sendResponse(notification);
}

void LanguageServer::handleShutdown(const json& request){
    json requestId = request.contains("id") ? request["id"] : nullptr;

    json response;
    response["jsonrpc"] = "2.0";
    response["id"] = requestId;
    response["result"] = nullptr;

    sendResponse(response);
}

void LanguageServer::handleExit(){
    should_exit_ = true;
}




void LanguageServer::sendResponse(const json& responseObj){

    std::string responseBody = responseObj.dump();

    std::string header = "Content-Length: " + std::to_string(responseBody.length()) + "\r\n\r\n";

    std:: cout << header << responseBody;

    std::cout<< std::flush;
}