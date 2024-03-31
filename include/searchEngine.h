//
// Created by LDm on 30.03.2024.
//

#pragma once

#include "converterJSON.h"
#include "invertedIndex.h"


struct RelativeIndex{
    size_t doc_id;
    float rank;
    bool operator == (const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

//---------------------
class SearchServer
{
public:
    explicit SearchServer(invertedIndex &ind): _index(ind){};

    std::vector<std::vector<RelativeIndex>> search (const std::vector<std::string>& queries_input);

private:
    invertedIndex _index;

    std::vector<std::string> U_WordList (std::vector<std::string>& wordList);

    std::vector<RelativeIndex> getRank(std::vector<std::string> &_list);

    void sortRelativeVector(std::vector<RelativeIndex> &vec);

    std::vector<std::string> findRarestWord(std::vector<std::string>& _list);
};

