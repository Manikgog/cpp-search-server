#pragma once
#include <iostream>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};


struct Document {
    Document() = default;

    Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;

    //перегрузка вывода в поток
    friend ostream& operator<<(ostream& output, const Document& d) {
        cout << "{ "s << "document_id = " << d.id << ", relevance = " << d.relevance << ", rating = " << d.rating << " }"s;
        return output;
    }
};
