#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;



class LanguageServer {
    public:
      LanguageServer() = default;
      ~LanguageServer() = default;

      void routesLspMessage(const std::string& raw_json);

      bool shouldExit() const { return should_exit_; }

    private:
        void handleInitialize(const json& request);
        void sendResponse(const json& responseObj);

        void handleTextDocumentSync(const std::string& method, const json& params);
        void runDiagnostics(const std::string& uri, const std::string& text, const std::string& target_phrase, const std::string& message, int severity);

        void handleCompletion(const json& request);

        void handleShutdown(const json& request);
        void handleExit();

        bool should_exit_ = false;

};