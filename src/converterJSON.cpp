#include "converterJSON.h"

void ConverterJSON::init() {
    std::ifstream configFile ("..\\..\\config\\config.json");
    if(!configFile.is_open()) {
        std::cerr << "config file is missing" << std::endl;

    } else {
        configFile >> config;
        if (!config.contains("config")) {
            std::cerr << "config file is empty" << std::endl;
        } else{
            std::cout<< "config is founded!" << std::endl;
            responsesLimit = config["config"]["max_responses"];
            indexUpdate = config["config"]["update"];
            name = config["config"]["name"];
            version = config["config"]["version"];
        }
        configFile.close();
    }
    std::cout << "responsesLimit: " << responsesLimit << "\n";
    std::cout << "indexUpdate: " << indexUpdate << "\n";
    std::cout << "name: " << name << "\n";
    std::cout << "version: " << version << "\n";
    std::cout<< "init - ok!" << std::endl;
}
//-------------------------------

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> textList;
    listDocuments.clear();
    json::iterator it = config.find("files");
    for (int i = 0; i < (*it).size(); ++i) {
        listDocuments.emplace_back((*it)[i]);
    }
    for(int i = 0; i < listDocuments.size(); ++i){
        std::ifstream file(listDocuments[i], std::ios::ate);
        if(file.is_open()){
            auto szFile = file.tellg();
            std::string str(szFile, '\0');
            file.seekg(0);
            file.read(&str[0], szFile);
            textList.emplace_back(str);
            file.close();
        } else {
            std::cerr << "Ошибка открытия файла" << std::endl;
        }
    }
    return textList;
}

//-------------------------------

std::vector<std::string> ConverterJSON::GetRequests() {
    std::vector<std::string> Requests;
    std::ifstream req("..\\..\\config\\requests.json");
    if(req.is_open()) {
        json j;
        req >> j;
        req.close();
        Requests = j["requests"].get<std::vector<std::string>>();
    }
    return Requests;
}
//-------------------------------

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> _answers) {
    char mk = '\t';
    char rk = '\n';
    std::ofstream fileAnswer("..\\..\\config\\answers.json");
    if(fileAnswer.is_open()){
        json answerJSON;
        if(!_answers.empty()) {
            int countResponse =  _answers.size();
            for (int el = 0; el < countResponse; ++el) {
                json jsonRequests = json::object();
                std::string postfix;
                postfix.resize(3 - std::to_string(el + 1).size());
                std::fill(postfix.begin(), postfix.end(), '0');
                const std::string keyRequests = "requests" + postfix + std::to_string(el + 1);
                json result;
//------------------------------------------
                if (!_answers[el].empty()) {
                    if (_answers[el].size() > 0) {
                        result["result"] = "true";
                        jsonRequests[keyRequests] += result;
                        if (_answers[el].size() > 1) {
                            json jsonRelevance = json::object();
                            for (int i = 0; i < _answers[el].size() ; ++i) {
                                json j_pair;
                                j_pair["docId"] = std::to_string(_answers[el][i].first);
                                j_pair["rank"] = std::to_string(_answers[el][i].second);
                                jsonRelevance["relevance"].emplace_back(j_pair);
                            }
                            if(!jsonRelevance["relevance"].empty()) {
                                jsonRequests[keyRequests].emplace_back(jsonRelevance);
                            }
                        } else{ //если _answers[el].size() = 1
                            json j_pair;
                            j_pair["docId"] = std::to_string(_answers[el][0].first);
                            j_pair["rank"] = std::to_string(_answers[el][0].second);
                            jsonRequests[keyRequests] += j_pair;
                        }
                    } //else {//если _answers[el].size() = 0
                    //result["result"] = "false";
                    //jsonRequests[keyRequests] += result;
                    // }
                }else {//если _answers[el].empty() == true
                    result["result"] = "false";
                    jsonRequests[keyRequests] += result;
                }
                answerJSON["answers"] += jsonRequests;
            }
        }else {//если _answers.empty() == true
            //std::cout << "Answer is empty\n";
            return;
        }
        fileAnswer << answerJSON;
        fileAnswer.close();
    } else std::cerr << "failed to open/create file \"..\\config\\answers.json\"" << std::endl;
}
//-----------------------------------------