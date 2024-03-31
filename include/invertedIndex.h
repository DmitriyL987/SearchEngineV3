//
// Created by LDm on 30.03.2024.
//

#pragma once

#include <map>
#include <future>
#include <iostream>
#include "converterJSON.h"


struct  Entry{
    size_t count;
    size_t doc_id;

    bool operator ==(const Entry &other) const {
        return (doc_id == other.doc_id && count == other.count);
    }
};
//-----------------------------------------------------

class invertedIndex {
public:
    invertedIndex() = default;

    void UpdateDocumentBase(const std::vector<std::string> &input_docs);

    std::vector<Entry> GetWordCount(const std::string& word);

    void separationWord(const std::string &text, std::vector<std::string>& words) const;

    std::map<std::string,std::vector<Entry>> listToFreqDict(std::vector<std::string> &_list);

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
};
