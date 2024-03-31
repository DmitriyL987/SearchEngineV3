#include "searchEngine.h"

//-------------------------------------------------------------
std::vector<std::vector<RelativeIndex>> SearchServer::search (const std::vector<std::string>& queries_input)
{
    std::vector<std::string> tmp ;
    tmp.clear();
    // разбили запросы по отдельным словам
    std::vector<std::vector<std::string>> fragmentedRequest;

    for (size_t i = 0; i < queries_input.size(); ++i){
        std::string  str = queries_input[i];
        _index.separationWord(str, tmp);
        fragmentedRequest.emplace_back(tmp);
    }

    //формирует списки уникальных слов uniqueWordsList из fragmentedRequest
    std::vector<std::vector<std::string>> uniqueWordsList;

    for(size_t i = 0; i < fragmentedRequest.size(); ++i)
        uniqueWordsList.emplace_back(U_WordList(fragmentedRequest[i]));

    //упорядоченный список слов по частоте встречаемости в словаре
    std::vector<std::vector<std::string>> orderedWordList(uniqueWordsList.size());
    orderedWordList.clear();

    for(size_t i = 0; i < uniqueWordsList.size(); ++i)
        orderedWordList.emplace_back(findRarestWord(uniqueWordsList[i]));

    std::vector<std::vector<RelativeIndex>> relativeIndex;
    relativeIndex.clear();

    for(size_t i = 0; i < orderedWordList.size(); ++i)
        relativeIndex.emplace_back( getRank(orderedWordList[i]));


    return relativeIndex;
}
//------------------------------------------

//возвращает вектор уникальных слов отсортированный по частоте встречаемости в словаре по возрастанию
std::vector<std::string>  SearchServer::findRarestWord(std::vector<std::string>& _list){
    std::map <size_t, std::string> map;
    std::map<std::string, std::vector<Entry>> freq_dictionary_Req = _index.listToFreqDict(_list);
    std::map<std::string, std::vector<Entry>>::iterator word;
    std::vector<std::string> orderedWord;

// суммирует count по вектору Entry для поиска редкого слова
    auto sumCount = [&freq_dictionary_Req](const std::string &str){
        size_t summ = 0;
        auto it = freq_dictionary_Req.find(str);
        for(auto elem : it->second) summ += elem.count;
        return summ;
    };
    for(const auto& it : freq_dictionary_Req){
        size_t summ = sumCount(it.first);
        if(summ != 0 && (map.count(summ) == 0)) map.insert({summ, it.first});
    }
    if(!map.empty()) // вектор сортирован в порядке от самых редко встречаемых к часто встречаемым
        for (const auto& it : map) orderedWord.push_back(it.second);

    std::cout << std::endl;
    return orderedWord;
}

//------------------------------------------------------------------
//возвращает сортированный по убыванию вектор структур RelativeIndex
std::vector<RelativeIndex> SearchServer::getRank(std::vector<std::string> &_list){
    if(_list.empty()) return {};
    std::map<std::string, std::vector<Entry>> dict = _index.listToFreqDict(_list);//
    std::map<std::string, std::vector<Entry>> d_Request; //структура для хранения слов выборки
    std::vector<size_t> docList;
    std::vector<size_t> rankTableAbs;
    std::vector<std::string> wordList;
    std::vector <float> relativeRanks;
    std::vector <size_t> absoluteRanks;
    std::map<std::string, std::vector<Entry>>::iterator it;

    //  первое самое редкое слово находим в словаре
    it = dict.find(_list[0]);
    if(it != dict.end()) {
        for (size_t k = 0; k < it->second.size(); ++k) {
            if(it->second[k].count != 0) {
                docList.push_back(it->second[k].doc_id);
                rankTableAbs.push_back(it->second[k].count);
            }
        }
    }// сформирован список документов по первому самому редкому слову
    wordList.clear();

    // поиск соответствия след. слова списку документов
    for(auto word : _list){
        it = dict.find(word);
        for(auto numDoc : docList){
            bool is_cont = false;
            for(auto el : it->second)
                if(el.doc_id == numDoc) { is_cont = true; break;}
            if(is_cont && std::count(wordList.begin(), wordList.end(), word) == 0) wordList.push_back(word);
        }
    }
    //---------------------------
    for (int i = 1;i < wordList.size(); ++i){
        std::vector<Entry> entry_vec;
        it  = dict.find(wordList[i]);
        if (it != dict.end()){
            for(auto j : docList){
                for(auto el : it->second){
                    if((j == el.doc_id) && (el.count != 0))
                        entry_vec.push_back({el.count, el.doc_id});
                }
            }
            if(!entry_vec.empty()) {// здесь словарь слов в соответствии со списком по первому слову
                d_Request.emplace(std::make_pair(wordList[i], entry_vec));
                entry_vec.clear();
            }
        }
    }
    // проверили соответствие списка документов и слов запроса
    //реализация п.7
    auto sum_abs_rank = [&d_Request](const size_t doc){
        size_t sum = 0;
        auto word = d_Request.begin();
        while (word != d_Request.end()){
            for(auto item : word->second){
                if(item.doc_id == doc) {
                    sum += item.count;
                    break;
                }
            }
            ++word;
        }
        return sum;
    };
//------------------
    float maxRank = 0.0;
    absoluteRanks.resize(docList.size());
    for(int i = 0; i < docList.size(); ++i){
        absoluteRanks[i] += sum_abs_rank(docList[i]);
        maxRank += static_cast<float>(absoluteRanks[i]);
    }
//-----------------
    std::vector<RelativeIndex> relativeIndex;
    float rank = 0.0f;
    for (size_t i = 0; i < docList.size(); ++i){
        if(maxRank!=0.0f) rank = static_cast<float>(absoluteRanks[i])/maxRank;
        else rank= 0.0f;
        if(rank != 0.0f) relativeIndex.push_back({docList[i], rank});
    }

    if(relativeIndex.empty()) {
        //std::cout << "getRank: relativeIndex is empty\n";
        return {};
    }
    sortRelativeVector(relativeIndex);
    return relativeIndex;
} // getRank end.

//------------------------------------
// Сортировка по релевантности в порядке убывания
void SearchServer::sortRelativeVector(std::vector<RelativeIndex> &vec){//++
    RelativeIndex rel;
    for(int i = 0; i < vec.size()-1;++i) {
        int mInd = i;
        for (int j = i+1; j < vec.size();++j) if (vec[mInd].rank < vec[j].rank) mInd = j;
        if(mInd != i){
            rel = vec[i];
            vec[i] = vec[mInd];
            vec[mInd] = rel;
        }
    }
}

//---------- Создаём списки уникальных слов для одного входящего запроса --
std::vector<std::string> SearchServer::U_WordList(std::vector<std::string> &wordList) {
    std::vector<std::string> res(0);
    for (auto &it : wordList ) { if (std::count(res.begin(), res.end(), it) == 0) res.push_back(it); }
    res.shrink_to_fit();
    return res;
}
