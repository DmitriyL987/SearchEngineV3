#define MIN_WORD_SIZE 2 // ограничивает минимальную длину слова
#include "invertedIndex.h"

//-----------------------------------------------------

void invertedIndex::UpdateDocumentBase(const std::vector<std::string> &input_docs) {
    docs = input_docs;
    freq_dictionary.clear();
    std::vector<std::vector<Entry>> entryList(input_docs.size());
    std::vector <std::vector<std::string>> fragmentedText(input_docs.size());
    for(int i = 0; i < input_docs.size(); ++i) separationWord(input_docs[i],fragmentedText[i]);

    std::shared_future<void>future;

    for(int c = 0; c < input_docs.size(); ++c){
        future = std::async(std::launch::async, [this, &fragmentedText](int i)
        {
            for(auto &word : fragmentedText[i]) {
                auto it = freq_dictionary.find(word);
                if(it == freq_dictionary.end())
                    freq_dictionary.emplace(std::make_pair(word, GetWordCount(word)));
            }
        }, c);
    }
    future.wait();  //Update base - ok!
}
//-----------------------------------------------------

std::vector<Entry> invertedIndex::GetWordCount(const std::string& word) {
    if(word.size()< MIN_WORD_SIZE) return std::vector<Entry>();
    std::vector<Entry> entryList;
    if(!docs.empty()) {
        Entry entry;
        for (size_t i = 0; i < docs.size(); ++i) {
            size_t count = 0;
            int foundPos = -1;
            for (size_t j = 0; j < docs[i].size(); ++j) {
                foundPos = -1;
                foundPos = (int)docs[i].find(word, j);
                if (foundPos >= 0) {
                    j = foundPos + word.size();
                    ++count;
                }
            }
            entry.doc_id =  i;
            entry.count = count;
            entryList.emplace_back(entry);
        }
    }else {
        std::cerr << "docs is empty!!!\n";
    }
    return entryList;
}

//------------------------------------
// разбирает по словам текст в любой кодировке с учетом словаря символов
void invertedIndex::separationWord(const std::string &text, std::vector<std::string> &words) const {
    words.clear();
    if (text.size() < MIN_WORD_SIZE) return;
    std::string dict = R"("/.?,;:!*-+ @#$%^№&`~()[]{}<>|'=_\)";
    auto is_containDelim = [dict](char ch) {
        if(dict.find(ch) != std::string::npos) return true;
        else if (ch == '\n' || ch == '\t' || ch == '\000' ) return true;
        else return false;
    };
    auto censor = [](std::string &_word){// исключаем слова не несущие смысловую нагрузку
        std::string censorship = "the,The,are,not,Not,yes,Yes,And,and,for,For";
        if(censorship.find(_word) != censorship.npos) return true;
        else return false;
    };
    std::string word;
    for(int i = 0; i < text.size(); ++i){
        if (is_containDelim(text[i])) {
            if (word.size() > (MIN_WORD_SIZE-1)) words.emplace_back(word); // только слова более 2 символов
            word.clear();
        } else {
            word += text[i];
        }
    }
    if(censor(word)) word.clear();
    if(!word.empty()) words.emplace_back(word);
}

//-------------------------------------

std::map<std::string,std::vector<Entry>> invertedIndex::listToFreqDict(std::vector<std::string> &_list) {
    std::map<std::string,std::vector<Entry>> entryList;
    for(auto &it : _list){
        auto elem = freq_dictionary.find(it);
        std::vector<Entry> vecEntry;
        Entry entry{0,0};
        if(elem != freq_dictionary.end()){
            for(auto el : elem->second) {
                entry.doc_id = el.doc_id;
                entry.count = el.count;
                vecEntry.emplace_back(entry);
            }
            entryList.insert({it,vecEntry});
        } else {
            vecEntry.emplace_back(entry);
            entryList.insert({it,vecEntry});
        }
    }
    return entryList;
}
