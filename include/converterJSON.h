#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ConverterJSON {
    json config;
    std::string name;
    std::string version;
    int responsesLimit;
    int indexUpdate;

    std::vector<std::string> listDocuments;
    std::vector<std::string> requests;

public:
    ConverterJSON(){
        init();
    }

    void init();

    std::vector<std::string> GetTextDocuments();

    std::vector<std::string> GetRequests();

    int GetResponsesLimit() {return responsesLimit;};

    void putAnswers(std::vector<std::vector<std::pair<int, float>>> _answers);

    void postProcessingAnswerJSON();
};