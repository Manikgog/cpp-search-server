#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    map<int, string> ids_words;
    map<string, vector<int>> words_ids;
    for (const int document_id : search_server) {
        pair<string, int> d = search_server.GetWord(document_id);
        vector<int> v;
        v.push_back(d.second);
        if (words_ids.count(d.first) == 0) {
            words_ids.insert({d.first, v});
        } else {
            words_ids.at(d.first).push_back(d.second);
        }
    }
    
    set<int> duplicates;
    for (auto str_ids : words_ids) {
        if (str_ids.second.size() > 1) {
            for (int i = 1; i < str_ids.second.size(); ++i) {
                duplicates.insert(str_ids.second[i]);
            }
        }
    }

    for (const int n : duplicates) {
        cout << "Found duplicate document id " << n << endl;
        search_server.RemoveDocument(n);
    }
}