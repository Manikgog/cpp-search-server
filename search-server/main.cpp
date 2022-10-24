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
    /*search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, {1, 2, 3});
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, {1, 2, 8});
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, {1, 1, 1});
    {
        LOG_DURATION_STREAM("MatchDocuments", cout);
        MatchDocuments(search_server, "big dog"s);
    }

    {
        LOG_DURATION_STREAM("FindTopDocuments", cout);
        FindTopDocuments(search_server, "big dog");
    }
    */

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

    /*AddDocument(search_server, 4, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 5, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 6, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 7, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 8, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});
    AddDocument(search_server, 9, "funny pet and curly hair"s, DocumentStatus::ACTUAL, {1, 2});*/
    // int id = 0;
    // cout << "enter id: ";
    // cin >> id;
    /*
    const auto str_tf = search_server.GetWordFrequencies(id);

    for (auto word_tf : str_tf) {
        cout << word_tf.first << " - "s << word_tf.second << endl;
    }
    cout << endl;
    */
    /*
    for (const int document_id : search_server) {
        cout << document_id << ' ';
    }
    cout << endl;

    search_server.RemoveDocument(id);
*/
    auto search_results = search_server.FindTopDocuments("curly dog"s);
    int page_size = 2;
    auto pages = Paginate(search_results, page_size);
    // Выводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }

    RemoveDuplicates(search_server);

    search_results = search_server.FindTopDocuments("curly dog"s);
    page_size = 2;
    pages = Paginate(search_results, page_size);
    // Выводим найденные документы по страницам
    for (auto page = pages.begin(); page != pages.end(); ++page) {
        cout << *page << endl;
        cout << "Page break"s << endl;
    }

    return 0;
}
