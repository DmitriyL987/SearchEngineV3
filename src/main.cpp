//
// Created by LDm on 31.03.2024.
//
#include <iostream>
#include "searchEngine.h"


int main() {
    ConverterJSON converter;
    invertedIndex inverter;
    std::vector<std::string> texts;
    texts = converter.GetTextDocuments();
    inverter.UpdateDocumentBase(texts);
    std::vector<std::string> requests = converter.GetRequests();
    SearchServer server (inverter);
    std::vector<std::vector<RelativeIndex>> tmp(server.search(requests));
    std::vector<std::vector<std::pair<int, float>>> preAnswer(tmp.size());

    for(int i = 0;i < tmp.size(); ++i){
        size_t responsesLimit = converter.GetResponsesLimit() < tmp[i].size() ? converter.GetResponsesLimit() : tmp[i].size();
        for( int j = 0; j < responsesLimit; ++j)
            preAnswer[i].emplace_back(std::make_pair(tmp[i][j].doc_id,tmp[i][j].rank));
    }
    converter.putAnswers(preAnswer);
    return 0;
}
