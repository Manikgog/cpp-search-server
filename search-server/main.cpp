#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
	string s;
	getline(cin, s);
	return s;
}

int ReadLineWithNumber() {
	int result = 0;
	cin >> result;
	ReadLine();
	return result;
}

vector<string> SplitIntoWords(const string& text) {
	vector<string> words;
	string word;
	for (const char c : text) {
		if (c == ' ') {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
		}
		else {
			word += c;
		}
	}
	if (!word.empty()) {
		words.push_back(word);
	}

	return words;
}

struct Document {
	int id;
	double relevance;
};



class SearchServer {
public:

	int document_count_ = 0;

	// преобразование строки стоп-слов в контейнер set stop_words_ в поле класса SearchServer 
	void SetStopWords(const string& text) {
		if (text.length() == 0) return;
		for (const string& word : SplitIntoWords(text)) {
			stop_words_.insert(word);
		}
	}
	// преобразование строки из входного потока в контейнер map word_to_document_freqs_
	void AddDocument(int document_id, const string& document) {
		if (document.length() != 0) {
			const vector<string> words = SplitIntoWordsNoStop(document);
			// проверка на пустоту документа после исключения стоп-слов
			if (!words.empty()) {
				int words_size = words.size(); // количество слов в документе
				for (const string& word : words) {
					// добавляем word в word_to_document_freqs_ в качестве ключа,
					// а в качестве значения map<document_id, TF>
					// TF частота появления слова в документе (строке)
					word_to_document_freqs_[word][document_id] += 1.0 / words_size;
				}
			}
		}
	}

	vector<Document> FindTopDocuments(const string& raw_query) const {
		const Query query_words = ParseQuery(raw_query);
		auto matched_documents = FindAllDocuments(query_words);
		
		sort(matched_documents.begin(), matched_documents.end(),
			[](const Document& lhs, const Document& rhs) {
				return lhs.relevance > rhs.relevance;
			});
		if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
			matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
		}
		return matched_documents;
	}

private:

	struct Query {
		set<string> plus_words;
		set<string> minus_words;
	};

	// контейнер в котором содержатся слова в качестве ключа и контейнер map<int, double> 
	// в качестве значения. В этом контейнере ключ - id документа, а значение - его релевантность
	map<string, map<int, double>> word_to_document_freqs_;

	set<string> stop_words_;

	

	bool IsStopWord(const string& word) const {
		return stop_words_.count(word) > 0;
	}

	vector<string> SplitIntoWordsNoStop(const string& text) const {
		vector<string> words;
		for (const string& word : SplitIntoWords(text)) {
			if (!IsStopWord(word)) {
				words.push_back(word);
			}
		}
		return words;
	}

	Query ParseQuery(const string& text) const {
		Query query;
		for (const string& word : SplitIntoWordsNoStop(text)) {
			if (word[0] == '-') { // если в слове запроса первый символ "-" тогда это минус-слово
				word.substr(1); // удаляем "-"
				if (!IsStopWord(word)) { // проверяем есть ли минус-слово в стоп-словах
					query.minus_words.insert(word); // добавляем в контейтер минус-слов в структуре query
				}
			}
			else {
				query.plus_words.insert(word);  // добавляем в контейтер плюс-слов в структуре query
			}
		}
		return query;
	}

	vector<Document> FindAllDocuments(const Query& query_words) const {
		vector<Document> matched_documents;
		map<int, double> document_to_relevance; // ключ - id документа, значение - релевантность документа
		double IDF = 0.0;
		map<string, double> word_IDF; // контейнер из пар слово_запроса - TF

		for (const auto& word : query_words.plus_words) {
			// если слово word есть в контейнере word_to_document_freqs_, то это слово вообще есть в документах
			// есть смысл с ним работать
			if (word_to_document_freqs_.count(word) > 0) {
				double number_docs = word_to_document_freqs_.at(word).size();
				IDF = log(document_count_/number_docs);
				word_IDF[word] = IDF;
			}
		}
			   		
		// перебираем номера документов
		for (int i = 0; i < document_count_; i++) {
		// перебираем слова запроса
		for (const auto& word_IDF_ : word_IDF) {
			// если есть минус-слово, то переходим к следующему слову
			if (query_words.minus_words.count(word_IDF_.first) > 0) continue;

			for (const auto& id_TF : word_to_document_freqs_.at(word_IDF_.first)) {
				
				if (id_TF.first == i) {
					double TF_IDF = (double)id_TF.second * (double)word_IDF_.second;
					if (document_to_relevance[i] == 0) {
							double rel = TF_IDF;
							document_to_relevance[i] = rel;
						}
						else {
							double rel = document_to_relevance.at(i) + TF_IDF;
							document_to_relevance.at(i) = rel;
						}
				}
				}
			}
		}

		for (const auto& a : document_to_relevance) {
			matched_documents.push_back({ a.first, a.second });
		}
		return matched_documents;
	}
};

SearchServer CreateSearchServer() {
	SearchServer search_server;
	search_server.SetStopWords(ReadLine()); // обращение функции SetStopWords в классе search_server
	search_server.document_count_ = ReadLineWithNumber(); // считывание из входного потока количества документов
	for (int document_id = 0; document_id < search_server.document_count_; ++document_id) {
		search_server.AddDocument(document_id, ReadLine()); // добавление документа
	}
	return search_server;
}

int main() {
	setlocale(LC_ALL, "rus");
	const SearchServer search_server = CreateSearchServer();

	const string query = ReadLine(); // строка поискового запроса
	if (query.length() == 0) return 0;
	for (const Document& doc : search_server.FindTopDocuments(query)) {
		//if (doc.relevance == 0) return 0;
		cout << "{ document_id = "s << doc.id << ", "
			<< "relevance = "s << doc.relevance << " }"s << endl;
	}
}

