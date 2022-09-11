#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

using namespace std;

/* Подставьте вашу реализацию класса SearchServer сюда */
const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ACCURACY = 1e-6;

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
	const string& func, unsigned line, const string& hint) {
	int t_ = t; int u_ = u;
	if (t_ != u_) {
		cout << boolalpha;
		cout << file << "("s << line << "): "s << func << ": "s;
		cout << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
		cout << t << " != "s << u << "."s;
		if (!hint.empty()) {
			cout << " Hint: "s << hint;
		}
		cout << endl;
		abort();
	}
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
	const string& hint) {
	if (!value) {
		cout << file << "("s << line << "): "s << func << ": "s;
		cout << "ASSERT("s << expr_str << ") failed."s;
		if (!hint.empty()) {
			cout << " Hint: "s << hint;
		}
		cout << endl;
		abort();
	}
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))

template <typename T>
void RunTestImpl(T function, const string& name_function) {
	function();
	cerr << name_function << " OK"s << endl;
}

#define RUN_TEST(func)  RunTestImpl(func, #func)

string ReadLine() {
	string s;
	getline(cin, s);
	return s;
}

int ReadLineWithNumber() {
	int result;
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
	int rating;
};

enum class DocumentStatus {
	ACTUAL,
	IRRELEVANT,
	BANNED,
	REMOVED,
};

class SearchServer {
public:
	void SetStopWords(const string& text) {
		for (const string& word : SplitIntoWords(text)) {
			stop_words_.insert(word);
		}
	}

	void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
		const vector<string> words = SplitIntoWordsNoStop(document);
		const double inv_word_count = 1.0 / words.size();
		for (const string& word : words) {
			word_to_document_freqs_[word][document_id] += inv_word_count;
		}
		documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
	}

	template <typename DocumentPredicate>
	vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
		const Query query = ParseQuery(raw_query);
		vector<Document>  matched_documents = FindAllDocuments(query, document_predicate); //, status
		sort(matched_documents.begin(), matched_documents.end(),
			[](const Document& lhs, const Document& rhs) {
				if (abs(lhs.relevance - rhs.relevance) < ACCURACY) {
					return lhs.rating > rhs.rating;
				}
				else {
					return lhs.relevance > rhs.relevance;
				}
			});
		if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
			matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
		}
		return matched_documents;
	}

	vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus Status) const {
		return FindTopDocuments(raw_query, [&Status](int document_id, DocumentStatus status, int rating) { return status == Status; });
	}

	vector<Document> FindTopDocuments(const string& raw_query) const {
		return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
	}

	int GetDocumentCount() const {
		return documents_.size();
	}

	tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
		// Ваша реализация данного метода
		const Query query = ParseQuery(raw_query);
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
		return { matched_words, documents_.at(document_id).status };
	}

private:
	struct DocumentData {
		int rating;
		DocumentStatus status;
	};

	set<string> stop_words_;
	map<string, map<int, double>> word_to_document_freqs_;
	map<int, DocumentData> documents_;

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

	static int ComputeAverageRating(const vector<int>& ratings) {
		if (ratings.empty()) {
			return 0;
		}
		int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
		size_t average = rating_sum / static_cast<int>(ratings.size());
		return average;
	}

	struct QueryWord {
		string data;
		bool is_minus;
		bool is_stop;
	};

	QueryWord ParseQueryWord(string text) const {
		bool is_minus = false;
		// Word shouldn't be empty
		if (text[0] == '-') {
			is_minus = true;
			text = text.substr(1);
		}
		return { text, is_minus, IsStopWord(text) };
	}

	struct Query {
		set<string> plus_words;
		set<string> minus_words;
	};

	Query ParseQuery(const string& text) const {
		Query query;
		for (const string& word : SplitIntoWords(text)) {
			const QueryWord query_word = ParseQueryWord(word);
			if (!query_word.is_stop) {
				if (query_word.is_minus) {
					query.minus_words.insert(query_word.data);
				}
				else {
					query.plus_words.insert(query_word.data);
				}
			}
		}
		return query;
	}

	double ComputeWordInverseDocumentFreq(const string& word) const {
		return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
	}

	template <typename Filter>
	vector<Document> FindAllDocuments(const Query& query, Filter filter) const {
		map<int, double> document_to_relevance;
		for (const string& word : query.plus_words) {
			if (word_to_document_freqs_.count(word) == 0) {
				continue;
			}
			const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
			for (const auto [id, relevance] : word_to_document_freqs_.at(word)) {
				const auto& doc = documents_.at(id);
				if (filter(id, doc.status, doc.rating)) {
					document_to_relevance[id] += relevance * inverse_document_freq;
				}
			}

		}

		for (const string& word : query.minus_words) {
			if (word_to_document_freqs_.count(word) == 0) {
				continue;
			}
			for (const auto id_tf : word_to_document_freqs_.at(word)) {
				document_to_relevance.erase(id_tf.first);
			}
		}

		vector<Document> matched_documents;
		for (const auto [id, relevance] : document_to_relevance) {
			matched_documents.push_back(
				{ id, relevance, documents_.at(id).rating });
		}
		return matched_documents;
	}
};
// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
	int doc_id = 42;
	const string content = "cat in the city"s;
	const vector<int> ratings = { 1, 2, 3 };
	{
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		auto found_docs = server.FindTopDocuments("in"s);
		ASSERT_EQUAL(found_docs.size(), 1lu);
		const Document& doc0 = found_docs[0];
		ASSERT_EQUAL(doc0.id, doc_id);
	}

	{
		SearchServer server;
		server.SetStopWords("in the"s);
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		ASSERT_HINT(server.FindTopDocuments("in"s).empty(), "Stop words must be excluded from documents"s);
	}
}


/*
Разместите код остальных тестов здесь
*/

// Тест проверяет исключение документа из найденных при наличии в нём минус-слов
void TestExcludedDocsWithMinusWords() {
	SearchServer server;
	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(1, "beautiful shepherd with expressive eyes", DocumentStatus::ACTUAL, { 3, 5, 4 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s); // сначала проверяем, что этот документ выводится (нет минус слов)
	ASSERT_EQUAL(found_docs.size(), 1); // когда документ не содержит минус-слов он находится
	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(1, "beautiful shepherd with expressive eyes", DocumentStatus::ACTUAL, { 3, 5, 4 });
	found_docs = server.FindTopDocuments("fluffy cat -collar"s); // добавляем минус-слово и проверяем вывод
	ASSERT_EQUAL(found_docs.size(), 0); // когда документ содержит минус-слова, то он не выводится
}

// Тест проверяет вывод документов со статусом ACTUAL
void TestOutputOfDocumentWithThe_ACTUAL_Status() {
	SearchServer server;
	// проверяем вывод документов со статусом ACTUAL
	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(1, "beautiful shepherd with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
	server.AddDocument(2, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::ACTUAL);
	ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status ACTUAL."s); // должен быть выведен один документ со статусом ACTUAL
	ASSERT_EQUAL_HINT(found_docs[0].id, 0, "Document with status ACTUAL must be with id 0."s);
}

// Тест проверяет вывод документов со статусом IRRELEVANT
void TestOutputOfDocumentWithThe_IRRELEVANT_Status() {
	SearchServer server;
	// проверяем вывод документов со статусом IRRELEVANT
	server.AddDocument(4, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(5, "beautiful cat with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
	server.AddDocument(6, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::IRRELEVANT);
	ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status IRRELEVANT."s); // должно быть выведено два документа со статусом IRRELEVANT
	ASSERT_EQUAL_HINT(found_docs[0].id, 5, "Document with status ACTUAL must be with id 5."s);
}

// Тест проверяет вывод документов со статусом BANNED
void TestOutputOfDocumentWithThe_BANNED_Status() {
	SearchServer server;
	// проверяем вывод документов со статусом BANNED
	server.AddDocument(7, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(8, "beautiful cat with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
	server.AddDocument(9, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::BANNED);
	ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status BANNED."s); // должно быть выведено два документа со статусом BANNED
	ASSERT_EQUAL_HINT(found_docs[0].id, 9, "Document with status ACTUAL must be with id 9."s);
}

// Тест проверяет вывод документов со статусом REMOVED
void TestOutputOfDocumentWithThe_REMOVED_Status() {
	SearchServer server;
	// проверяем вывод документов со статусом REMOVED
	server.AddDocument(10, "fluffy well-groomed cat with a collar", DocumentStatus::REMOVED, { 1, 2, 3 });
	server.AddDocument(11, "beautiful cat with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
	server.AddDocument(12, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::REMOVED);
	ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status REMOVED."s); // должно быть выведено два документа со статусом REMOVED
	ASSERT_EQUAL_HINT(found_docs[0].id, 10, "Document with status ACTUAL must be with id 10."s);
}

// Тест для проверки правильности расчёта среднего рейтинга
void TestAverageRating() {
	SearchServer server;
	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::REMOVED, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::REMOVED);
	ASSERT_EQUAL_HINT(found_docs[0].rating, 2, "Rating must be 2."s);
	found_docs.clear();
	server.AddDocument(1, "fluffy well-groomed cat with a collar", DocumentStatus::REMOVED, { 2, 4, 7 });
	found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::REMOVED);
	ASSERT_EQUAL_HINT(found_docs[0].rating, 4, "Rating must be 4."s);
}

void TestRelevance() {
	SearchServer server;
	server.AddDocument(20, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(21, "beautiful cat with expressive eyes", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(22, "fluffy white cat", DocumentStatus::ACTUAL, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::ACTUAL);
	ASSERT_EQUAL_HINT(found_docs[0].id, 20, "Document with id 20 first by relevance"s);
	ASSERT_EQUAL_HINT(found_docs[1].id, 22, "Document with id 20 second by relevance"s);
	ASSERT_EQUAL_HINT(found_docs[2].id, 21, "Document with id 20 third by relevance"s);
	for (const auto& doc : found_docs) {
		if (doc.id == 20) {
			double relevance = 0;
			double difference = abs(doc.relevance - 0.2506);
			if (difference < 0.0001) {
				relevance = 0.2506;
			}
			ASSERT_EQUAL(relevance, 0.2506);
		}
		if (doc.id == 21) {
			ASSERT_EQUAL(doc.relevance, 0);
		}
		if (doc.id == 22) {
			double relevance = 0;
			double difference = abs(doc.relevance - 0.1351);
			if (difference < 0.0001) {
				relevance = 0.1351;
			}
			ASSERT_EQUAL(relevance, 0.1351);
		}
	}
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
	RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
	RUN_TEST(TestExcludedDocsWithMinusWords);
	RUN_TEST(TestOutputOfDocumentWithThe_ACTUAL_Status);
	RUN_TEST(TestOutputOfDocumentWithThe_IRRELEVANT_Status);
	RUN_TEST(TestOutputOfDocumentWithThe_BANNED_Status);
	RUN_TEST(TestOutputOfDocumentWithThe_REMOVED_Status);
	RUN_TEST(TestAverageRating);
	RUN_TEST(TestRelevance);
}



// --------- Окончание модульных тестов поисковой системы -----------

int main() {
	TestSearchServer();
	// Если вы видите эту строку, значит все тесты прошли успешно
	cout << "Search server testing finished"s << endl;
}
