#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <deque>
#include "string_processing.h"
#include "search_server.h"
#include "paginator.h"
#include "request_queue.h"
#include "log_duration.h"
#include "remove_duplicates.h"

using namespace std;

ostream& operator<<(ostream& out, vector<string> v) {
    bool first = false;
    for (const auto i : v) {
        if (first) {
            out << ' ';
        }
        first = true;
        out << i;
    }
    return out;
}

template <typename T>
ostream& operator<<(ostream& out, vector<T> v) {
    for (const auto i : v) {
        out << i;
    }
    return out;
}

void FindTopDocuments(const SearchServer& search_server, const string& query) {
    cout << "Результаты поиска по запросу: "s << query << endl;
    vector<Document> documents = search_server.FindTopDocuments(query);
    cout << documents;
    return;
}

int IntFromStatus(const DocumentStatus& status) {
    if (status == DocumentStatus::ACTUAL) {return 0;}
    if (status == DocumentStatus::IRRELEVANT) {return 1;}
    if (status == DocumentStatus::BANNED) {return 2;}
    return 3;
}

void AddDocument(SearchServer& search_server, int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    search_server.AddDocument(document_id, document, status, ratings);
}


int main() {
    SearchServer search_server("and in at"s);
       
    // отличие только в стоп-словах, считаем дубликатом
    AddDocument(search_server, 4, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});

    // множество слов такое же, считаем дубликатом документа 1
    AddDocument(search_server, 5, "funny pet and not very nasty rat"s, DocumentStatus::ACTUAL, {1, 2});

    // добавились новые слова, дубликатом не является
    AddDocument(search_server, 6, "funny pet and not very nasty rat"s, DocumentStatus::ACTUAL, {1, 2});

    // множество слов такое же, как в id 6, несмотря на другой порядок, считаем дубликатом
    AddDocument(search_server, 7, "very nasty rat and not very funny pet"s, DocumentStatus::ACTUAL, {1, 2});

    // есть не все слова, не является дубликатом
    AddDocument(search_server, 8, "pet with rat and rat and rat"s, DocumentStatus::ACTUAL, {1, 2});

    // слова из разных документов, не является дубликатом
    AddDocument(search_server, 9, "nasty rat with curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 10, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});

    const auto search_results = search_server.FindTopDocuments("curly dog"s);
    int page_size = 2;
    const auto pages = Paginate(search_results, page_size);
    // Выводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }

    RemoveDuplicates(search_server);
    
    return 0;
}
