//
// Created by LDm on 26.03.2024.
//

#include "searchEngine.h"
#include <gtest/gtest.h>

TEST(sample_test_case, sample_test)
{
    EXPECT_EQ(1, 1);
}
void TestInvertedIndexFunctionaliti(
        const std::vector<std::string> &docs,
        const std::vector<std::string> & requests,
        const std::vector<std::vector<Entry>> & expected) {
    std::vector<std::vector<Entry>> result;
    invertedIndex idx;
    idx.UpdateDocumentBase(docs);
    for(auto & request : requests){
        std::vector<Entry> wordCount = idx.GetWordCount(request);
        result.push_back(wordCount);
    }
    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic){
    const std::vector<std::string> docs = {
            "london is the capital of great britain",
            "big ben is the nickname for the Great bell of the striking clock" };
    const std::vector<std::string> requests = {"london", "the"};
    const std::vector<std::vector<Entry>> expected = {
            {{1,0},{0,1}},
            {{1,0},{3,1}}
    };
    TestInvertedIndexFunctionaliti(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestBasic2){
    const std::vector<std::string> docs = {
            "milk, milk, milk, milk, water, water, water",
            "milk, water, water",
            "milk, milk, milk, milk, milk, water, water, water, water, water",
            "americano, cappuccino"
    };
    const std::vector<std::string> requests = {"milk", "water", "cappuccino"};
    const std::vector<std::vector<Entry>> expected = {
            {{4,0},{1,1},{5,2},{0,3}},
            {{3,0},{2,1},{5,2},{0,3}},
            {{0,0},{0,1},{0,2},{1,3}}
    };
    TestInvertedIndexFunctionaliti(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord){
    const std::vector<std::string> docs = {
            "a b c d e f g h i j k l",
            "statement"
    };
    const std::vector<std::string> requests = {"m", "statement"};
    const std::vector<std::vector<Entry>> expected = {
            {},
            {{0,0},{1,1}}
    };
    TestInvertedIndexFunctionaliti(docs, requests, expected);
}

TEST(TestCaseSearchServer, TestSimple) {
    const std::vector<std::string> docs = {
            "milk milk milk milk water water water water",
            "milk water water",
            "milk milk milk milk milk water water water water water",
            "americano cappuccino"
    };
    const std::vector<std::string> request = {"milk water", "sugar"};
    const std::vector<std::vector<RelativeIndex>> expected = {
            { {2, 0.4545454545f},
                    {0, 0.3636363636f},
                    {1, 0.1818181818f} },
            {

            }

    };
    invertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result, expected);
}

TEST(TestCaseSearchServer, TestTop5) {
    const std::vector<std::string> docs = {
            "london is the capital of great britain",
            "paris is the capital of france",
            "berlin is the capital of germany",
            "rome is the capital of italy",
            "madrid is the capital of spain",
            "lisboa is the capital of portugal",
            "bern is the capital of switzerland",
            "moscow is the capital of russia", //
            "kiev is the capital of ukraine",
            "minsk is the capital of belarus",
            "astana is the capital of kazakhstan",
            "beijing is the capital of china",
            "tokyo is the capital of japan",
            "bangkok is the capital of thailand",
            "welcome to moscow the capital of russia the third rome", //
            "amsterdam is the capital of netherlands",
            "helsinki is the capital of finland",
            "oslo is the capital of norway",
            "stockholm is the capital of sweden",
            "riga is the capital of latvia",
            "tallinn is the capital of estonia",
            "warsaw is the capital of poland",
    };
    const std::vector<std::string> request = {"moscow is the capital of russia"};
    const std::vector<std::vector<RelativeIndex>> expected = {
        {
                {7, 0.5},
                {14, 0.5},

        }
    };
    invertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result, expected);
}

