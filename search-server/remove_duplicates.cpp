#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    vector<int> ids;
    map<int, set<string>> ids_words;
    for (const int document_id : search_server) {
        ids.push_back(document_id);
        const auto str_tf = search_server.GetWordFrequencies(document_id);
        // перемещаемся по мапу str_tf и записываем в set слова, которые есть в документе с document_id
        set<string> words;
        for (auto word_tf : str_tf) {
            words.insert(word_tf.first);
        }
        ids_words.insert({document_id, words});
        words.clear();
    }
    set<int> duplicates;
    for (auto it_i = ids.begin(); it_i != ids.end(); ++it_i) {
        
        for (auto it_j = it_i; it_j != ids.end(); ++it_j) {
            if (*it_i == *it_j) continue;
            else if (ids_words.at(*it_i) == ids_words.at(*it_j)) {
                duplicates.insert(*it_j);
            } 
        }
    }
    for (const int n : duplicates) {
        cout << "Found duplicate document id " << n << endl;
        search_server.RemoveDocument(n);
    }
}