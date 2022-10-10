//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».
#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include "document.h"
#include "search_server.h"

using namespace std;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
   
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        auto search_results = search_server_.FindTopDocuments(raw_query, document_predicate);
        QueryResult query_result;
        query_result.query = raw_query;
        query_result.result = search_results;
        ++min_;
        if (min_ > min_in_day_) {
            --min_;
            requests_.pop_front();
            requests_.push_back(query_result);
        } else {
            requests_.push_back(query_result);
        }
        return search_results;
    }

    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);

    vector<Document> AddFindRequest(const string& raw_query);

    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        string query;               // поисковый запрос
        vector<Document> result;    // вектор документов найденных по запросу
    };
    deque<QueryResult> requests_;   // двухстороняя очередь структур QueryResult
    const static int min_in_day_ = 1440; // количество минут в дне
    int min_ = 0;                           // начальное количество минут
    const SearchServer& search_server_;     // ссылка на экземпляр класса SearchServer 
}; 
