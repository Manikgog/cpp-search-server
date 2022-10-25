#include "search_server.h"
#include <cmath>
#include <algorithm>
#include <numeric>

using namespace std;

SearchServer::SearchServer(const string& stop_words_text) : SearchServer(SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
{
}

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
                 const vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    document_ids_.push_back(document_id);
}

vector<int>::iterator SearchServer::begin() {
    return document_ids_.begin();
}

vector<int>::iterator SearchServer::end() {
    return document_ids_.end();
}

map<string, double> list_word_tf;
const map<string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    list_word_tf.clear();
    for (const auto word_id_tf : word_to_document_freqs_) {
        for (const auto id_tf : word_id_tf.second) {
            if (document_id == id_tf.first) {
                list_word_tf.insert({word_id_tf.first, id_tf.second});
            }
        }
    }
    return list_word_tf;
}

pair<int, string> id_words;
pair<int, string> SearchServer::GetWord(int document_id) const {
    set<string> words;
    string str;
    for (const auto word_id_tf : word_to_document_freqs_) {
        for (const auto id_tf : word_id_tf.second) {
            if (document_id == id_tf.first) {
                words.insert(word_id_tf.first);
            }
        }
    }
    for (string word : words) {
        str+=word;
        str+=' ';
    }
    id_words = {document_id, str};
    return id_words;
}

void SearchServer::RemoveDocument(int document_id) {
    // удаление document_id из вектора id документов
    vector<int> ids;
    for (auto id : document_ids_) {
        if (id != document_id) {
            ids.push_back(id);
        }
    }
    document_ids_.clear();
    document_ids_ = ids;
    // удаление id и данных документа с указанным document_id
    map<int, DocumentData> docs;
    for (auto id_docdata : documents_) {
        if (id_docdata.first != document_id) {
            docs.insert(id_docdata);
        }
    }
    documents_.clear();
    documents_ = docs;
    // удаление документа с его частотой слова из map<string, map<int, double>> word_to_document_freqs_;
    map<string, map<int, double>> word_to_doc_freqs;
    for (const auto word_id_tf : word_to_document_freqs_) {
        for (const auto id_tf : word_id_tf.second) {
            if (document_id != id_tf.first) {
                word_to_doc_freqs[word_id_tf.first][id_tf.first] += id_tf.second;
            }
        }
    }
    word_to_document_freqs_.clear();
    word_to_document_freqs_ = word_to_doc_freqs;
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}


tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query);
    vector<string> matched_words;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return {matched_words, documents_.at(document_id).status};
}

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!(IsValidWord(word))) {
            throw invalid_argument("Word "s + word + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
	size_t average = rating_sum / static_cast<int>(ratings.size());
	return average;
}

SearchServer::QueryWord SearchServer::ParseQueryWord(const string& text) const {
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !(IsValidWord(word))) {
        throw invalid_argument("Query word "s + text + " is invalid");
    }
    return {word, is_minus, IsStopWord(word)};
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query result;
    for (const string& word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.insert(query_word.data);
            } else {
                result.plus_words.insert(query_word.data);
            }
        }
    }
    return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

