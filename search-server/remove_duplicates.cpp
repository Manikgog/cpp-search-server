#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    vector<int> ids;
    map<int, set<string>> ids_words;
    for (const int document_id : search_server) {
        ids.push_back(document_id);
        ids_words.insert(search_server.GetWord(document_id));
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
