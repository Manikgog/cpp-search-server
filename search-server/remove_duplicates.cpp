#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    map<int, string> ids_words;
    for (const int document_id : search_server) {
        ids_words.insert(search_server.GetWord(document_id));
    }
    map<string, vector<int>> words_ids;
    for (auto id_words : ids_words) {
        if (words_ids.count(id_words.second) == 0) {
            words_ids[id_words.second].push_back(id_words.first);
        } else {
            words_ids.at(id_words.second).push_back(id_words.first);
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