#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server) {}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    // напишите реализацию
    auto search_results = search_server_.FindTopDocuments(raw_query, status);
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

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    // метод для формирования вектора результатов поиска по запросу
    auto search_results = search_server_.FindTopDocuments(raw_query);
    QueryResult query_result;               // экземпляр структуры QueryResult
    query_result.query = raw_query;         // поле - строка запроса
    query_result.result = search_results;   // поле - вектор найденных документов vector<Document>
    ++min_;                                 // добавление минуты (происходит при добавлении запроса)
                                            // исходя из условия получения запроса каждую минуту
    if (min_ > min_in_day_) {               // если количество минут больше количесва минут в дне
        --min_;                             // уменьшаем количество минут
        requests_.pop_front();              // удаляем первый в очереди элемент QueryResult
        requests_.push_back(query_result);  // добавляем новый элемент в конец очереди
    } else {                                // если количество минут меньше количества минут в дне
        requests_.push_back(query_result);  // просто добавляем новый элемент QueryResult в конец очереди
    }
    return search_results;
}

int RequestQueue::GetNoResultRequests() const {
    // метод который возвращает количество запросов с нулевым числом
    // найденных документов
    int numbers_of_no_result = 0;
    for (auto vec_docs : requests_) {
        if (vec_docs.result.size() == 0) {
            numbers_of_no_result++;
        }
    }
    return numbers_of_no_result;
}
