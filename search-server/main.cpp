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

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double ACCURACY = 1e-6;

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
	const string& func, unsigned line, const string& hint) {
<<<<<<< HEAD
	if (t != u) {
=======
	int t_= t; int u_ = u;
	if (t_ != u_) {
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
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
		// Ваша реализация данного метода
		for (const string& word : SplitIntoWords(text)) {
			stop_words_.insert(word);
		}
	}

	void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
		// Ваша реализация данного метода
		const vector<string> words = SplitIntoWordsNoStop(document);
		const double inv_word_count = 1.0 / words.size();
		for (const string& word : words) {
			word_to_document_freqs_[word][document_id] += inv_word_count;
		}
		documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
	}

	template <typename DocumentPredicate>
	vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
		// Ваша реализация данного метода
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

	vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus st) const {
		// Ваша реализация данного метода
		return FindTopDocuments(raw_query, [&st](int document_id, DocumentStatus status, int rating) { return status == st; });
	}

	vector<Document> FindTopDocuments(const string& raw_query) const {
		return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
	}

	int GetDocumentCount() const {
		// Ваша реализация данного метода
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
	// Реализация приватных методов вашей поисковой системы
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
<<<<<<< HEAD
			for (const auto id_relevance : word_to_document_freqs_.at(word)) {
				const auto& doc = documents_.at(id_relevance.first);
				if (filter(id_relevance.first, doc.status, doc.rating)) {
					document_to_relevance[id_relevance.first] += id_relevance.second * inverse_document_freq;
				}
			}
			/*
=======
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
			for (const auto[id, relevance] : word_to_document_freqs_.at(word)) {
				const auto& doc = documents_.at(id);
				if (filter(id, doc.status, doc.rating)) {
					document_to_relevance[id] += relevance * inverse_document_freq;
				}
			}
<<<<<<< HEAD
			*/
=======
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
		}

		for (const string& word : query.minus_words) {
			if (word_to_document_freqs_.count(word) == 0) {
				continue;
			}
			for (const auto id : word_to_document_freqs_.at(word)) {
				document_to_relevance.erase(id.first);
			}
		}

		vector<Document> matched_documents;
<<<<<<< HEAD
		for (const auto id_relevance : document_to_relevance) {
			matched_documents.push_back({ id_relevance.first, id_relevance.second, documents_.at(id_relevance.first).rating });
=======
		for (const auto[id, relevance] : document_to_relevance) {
			matched_documents.push_back({ id, relevance, documents_.at(id).rating });
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
		}
		/*
		for (const auto[id, relevance] : document_to_relevance) {
			matched_documents.push_back({ id, relevance, documents_.at(id).rating });
		}
		*/
		return matched_documents;
	}
};
// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
	const int doc_id = 42;
	const string content = "cat in the city"s;
	const vector<int> ratings = { 1, 2, 3 };
	{
		SearchServer server;
		server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
		const auto found_docs = server.FindTopDocuments("in"s);
		ASSERT_EQUAL(found_docs.size(), 1u);
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


<<<<<<< HEAD
/*
Разместите код остальных тестов здесь
*/

=======
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
// Тест проверяет исключение документа из найденных при наличии в нём минус-слов
void TestExcludedDocsWithMinusWords() {
	SearchServer server;
	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(1, "beautiful shepherd with expressive eyes", DocumentStatus::ACTUAL, { 3, 5, 4 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s); // сначала проверяем, что этот документ выводится (нет минус слов)
	ASSERT_EQUAL(found_docs.size(), 1u); // когда документ не содержит минус-слов он находится
	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(1, "beautiful shepherd with expressive eyes", DocumentStatus::ACTUAL, { 3, 5, 4 });
	found_docs = server.FindTopDocuments("fluffy cat -collar"s); // добавляем минус-слово и проверяем вывод
	ASSERT_EQUAL(found_docs.size(), 0); // когда документ содержит минус-слова, то он не выводится
}

<<<<<<< HEAD
// Тест проверяет вывод документов со статусом ACTUAL
void TestOutputOfDocumentWithThe_ACTUAL_Status() {
	SearchServer server;
	
}

// Тест проверяет вывод документов со статусом IRRELEVANT
=======
// Тест проверяет вывод документов по статусу
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
void TestingCorrectOutputStatus() {
	SearchServer server;
	{
		// проверяем вывод документов со статусом ACTUAL
		SearchServer server_1;
		server_1.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
		server_1.AddDocument(1, "beautiful shepherd with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
		server_1.AddDocument(2, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
		auto found_docs = server_1.FindTopDocuments("fluffy cat collar"s, DocumentStatus::ACTUAL);
		ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status ACTUAL."s); // должен быть выведен один документ со статусом ACTUAL
		ASSERT_EQUAL_HINT(found_docs[0].id, 0, "Document with status ACTUAL must be with id 0."s);
	}
	{
		// проверяем вывод документов со статусом IRRELEVANT
		SearchServer server_2;
		server_2.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
		server_2.AddDocument(1, "beautiful cat with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
		server_2.AddDocument(2, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
		auto found_docs = server_2.FindTopDocuments("fluffy cat collar"s, DocumentStatus::IRRELEVANT);
		ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status IRRELEVANT."s); // должен быть выведен один документ со статусом IRRELEVANT
		ASSERT_EQUAL_HINT(found_docs[0].id, 1, "Document with status ACTUAL must be with id 5."s);
	}
	{
		// проверяем вывод документов со статусом BANNED
		SearchServer server_3;
		server_3.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
		server_3.AddDocument(1, "beautiful cat with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
		server_3.AddDocument(2, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
		auto found_docs = server_3.FindTopDocuments("fluffy cat collar"s, DocumentStatus::BANNED);
		ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status BANNED."s); // должен быть выведен один документ со статусом BANNED
		ASSERT_EQUAL_HINT(found_docs[0].id, 2, "Document with status ACTUAL must be with id 9."s);
	}
	{
		// проверяем вывод документов со статусом REMOVED
		SearchServer server_4;
		server_4.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::REMOVED, { 1, 2, 3 });
		server_4.AddDocument(1, "beautiful cat with expressive eyes", DocumentStatus::IRRELEVANT, { 3, 5, 4 });
		server_4.AddDocument(2, "fluffy cat", DocumentStatus::BANNED, { 1, 2, 3 });
		auto found_docs = server_4.FindTopDocuments("fluffy cat collar"s, DocumentStatus::REMOVED);
		ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown document with status REMOVED."s); // должен быть выведен один документ со статусом REMOVED
		ASSERT_EQUAL_HINT(found_docs[0].id, 0, "Document with status ACTUAL must be with id 10."s);
	}
}


// Тест для проверки правильности расчёта среднего рейтинга
void TestFindCorrectAverageRating() {
	SearchServer server;

	server.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::REMOVED, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::REMOVED);
	ASSERT_EQUAL_HINT(found_docs[0].rating, ((1+2+3)/3), "Rating must be 2."s);
	found_docs.clear();

	server.AddDocument(1, "fluffy well-groomed cat with a collar", DocumentStatus::REMOVED, { 2, 4, 7 });
	found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::REMOVED);
	ASSERT_EQUAL_HINT(found_docs[0].rating, ((2+4+7)/3), "Rating must be 4."s);
}

void TestFindCorrectRelevance() {
	SearchServer server;
	server.AddDocument(20, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(21, "beautiful cat with expressive eyes", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(22, "fluffy white cat", DocumentStatus::ACTUAL, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::ACTUAL);
<<<<<<< HEAD
	
=======
	// проверка правильности расстановки документов согласно их релевантности
	ASSERT_EQUAL_HINT(found_docs[0].id, 20, "Document with id 20 first by relevance"s);
	ASSERT_EQUAL_HINT(found_docs[1].id, 22, "Document with id 22 second by relevance"s);
	ASSERT_EQUAL_HINT(found_docs[2].id, 21, "Document with id 21 third by relevance"s);

>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
	// проверка правильности выводимой программой релевантности
	// расчёт tf для слова "cat" для каждого документа и расчёт idf для слова "cat"
	double tf_id_20_cat = 1.0 / 6.0;
	double tf_id_21_cat = 1.0 / 5.0;
	double tf_id_22_cat = 1.0 / 3.0;
	double idf_cat = log(3.0/3.0);
	// расчёт tf для слова "fluffy" для каждого документа и расчёт idf для слова "fluffy"
	double tf_id_20_fluffy = 1.0 / 6.0;
	double tf_id_21_fluffy = 0.0 / 5.0;
	double tf_id_22_fluffy = 1.0 / 3.0;
	double idf_fluffy = log(3.0 / 2.0);
	// расчёт tf для слова "collar" для каждого документа и расчёт idf для слова "collar"
	double tf_id_20_collar = 1.0 / 6.0;
	double tf_id_21_collar = 0.0 / 5.0;
	double tf_id_22_collar = 0.0 / 3.0;
	double idf_collar = log(3.0 / 1.0);
	// расчёт требуемой релевантности для каждого документа
	double id_20_relevance_correct = tf_id_20_cat * idf_cat + tf_id_20_fluffy* idf_fluffy+ tf_id_20_collar* idf_collar;
	double id_21_relevance_correct = tf_id_21_cat * idf_cat + tf_id_21_fluffy * idf_fluffy + tf_id_21_collar * idf_collar;
	double id_22_relevance_correct = tf_id_22_cat * idf_cat + tf_id_22_fluffy * idf_fluffy + tf_id_22_collar * idf_collar;
	for (const auto& doc : found_docs) {
		if (doc.id == 20) {
			ASSERT_EQUAL(doc.relevance, id_20_relevance_correct);
		}
		if (doc.id == 21) {
			ASSERT_EQUAL(doc.relevance, id_21_relevance_correct);
		}
		if (doc.id == 22) {
			ASSERT_EQUAL(doc.relevance, id_22_relevance_correct);
		}
	}
}

<<<<<<< HEAD
// проверка правильности расстановки документов согласно их релевантности
void TestCheckCorrectSortOfRelevance() {
	SearchServer server;
	server.AddDocument(20, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(21, "beautiful cat with expressive eyes", DocumentStatus::ACTUAL, { 1, 2, 3 });
	server.AddDocument(22, "fluffy white cat", DocumentStatus::ACTUAL, { 1, 2, 3 });
	auto found_docs = server.FindTopDocuments("fluffy cat collar"s, DocumentStatus::ACTUAL);
	ASSERT_EQUAL_HINT(found_docs[0].id, 20, "Document with id 20 first by relevance"s);
	ASSERT_EQUAL_HINT(found_docs[1].id, 22, "Document with id 22 second by relevance"s);
	ASSERT_EQUAL_HINT(found_docs[2].id, 21, "Document with id 21 third by relevance"s);
}

// проверка правильности расстановки документов согласно предикату
void TestCheckCorrectPredicate() {
	// предикат - рейтинг
	{
		SearchServer server_1;
		server_1.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 }); // рейтинг 2
		server_1.AddDocument(1, "beautiful cat with expressive eyes", DocumentStatus::ACTUAL, { 1, 2, 3, 4 }); // рейтинг 2
		server_1.AddDocument(2, "fluffy white cat", DocumentStatus::ACTUAL, { 1, 2, 3, 5, 6 }); // рейтинг 3
		server_1.AddDocument(3, "smooth-haired ginger cat with a short tail", DocumentStatus::ACTUAL, { 1, 2, 3, 1, 2 }); // рейтинг 1
		server_1.AddDocument(4, "a cat with big ears", DocumentStatus::ACTUAL, { 1, 2, 3, 2, 2 }); // рейтинг 2
		server_1.AddDocument(5, "cat of the breed Russian blue", DocumentStatus::ACTUAL, { 1, 2, 3, 3, 2 }); // рейтинг 2 
		server_1.AddDocument(6, "striped gray cat with white mustache and blue eyes", DocumentStatus::ACTUAL, { 1, 2, 3, 4, 3 }); // рейтинг 2
		server_1.AddDocument(7, "black cat with white tips of paws and muzzle", DocumentStatus::ACTUAL, { 1, 2, 3, 7 }); // рейтинг 3
		server_1.AddDocument(8, "a bald cat with big ears and eyes", DocumentStatus::ACTUAL, { 1, 2, 3, 8 }); // рейтинг 3
		auto found_docs = server_1.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return rating == 2; });
		ASSERT_EQUAL_HINT(found_docs.size(), 5, "Must be shown 5 documents with rating 2."s);
	}

	// предикат - статус
	{
		SearchServer server_2;
		server_2.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
		server_2.AddDocument(1, "beautiful cat with expressive eyes", DocumentStatus::BANNED, { 1, 2, 3, 4 });
		server_2.AddDocument(2, "fluffy white cat", DocumentStatus::IRRELEVANT, { 1, 2, 3, 5, 6 });
		server_2.AddDocument(3, "smooth-haired ginger cat with a short tail", DocumentStatus::REMOVED, { 1, 2, 3, 1, 2 });
		server_2.AddDocument(4, "a cat with big ears", DocumentStatus::ACTUAL, { 1, 2, 3, 2, 2 });
		server_2.AddDocument(5, "cat of the breed Russian blue", DocumentStatus::ACTUAL, { 1, 2, 3, 3, 2 });
		server_2.AddDocument(6, "striped gray cat with white mustache and blue eyes", DocumentStatus::BANNED, { 1, 2, 3, 4, 3 });
		server_2.AddDocument(7, "black cat with white tips of paws and muzzle", DocumentStatus::ACTUAL, { 1, 2, 3, 7 });
		server_2.AddDocument(8, "a bald cat with big ears and eyes", DocumentStatus::ACTUAL, { 1, 2, 3, 8 });
		{
			auto found_docs = server_2.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; });
			ASSERT_EQUAL_HINT(found_docs.size(), 5, "Must be shown 6 documents with status ACTUAL."s);
		}
		{
			auto found_docs = server_2.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::BANNED; });
			ASSERT_EQUAL_HINT(found_docs.size(), 2, "Must be shown 2 documents with status BANNED."s);
		}
		{
			auto found_docs = server_2.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::IRRELEVANT; });
			ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown 1 documents with status IRRELEVANT."s);
		}
		{
			auto found_docs = server_2.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::REMOVED; });
			ASSERT_EQUAL_HINT(found_docs.size(), 1, "Must be shown 1 documents with status REMOVED."s);
		}
	}
	// предикат - номер документа
	{
		SearchServer server_3;
		server_3.AddDocument(0, "fluffy well-groomed cat with a collar", DocumentStatus::ACTUAL, { 1, 2, 3 });
		server_3.AddDocument(1, "beautiful cat with expressive eyes", DocumentStatus::BANNED, { 1, 2, 3, 4 });
		server_3.AddDocument(2, "fluffy white cat", DocumentStatus::IRRELEVANT, { 1, 2, 3, 5, 6 });
		server_3.AddDocument(3, "smooth-haired ginger cat with a short tail", DocumentStatus::REMOVED, { 1, 2, 3, 1, 2 });
		server_3.AddDocument(4, "a cat with big ears", DocumentStatus::ACTUAL, { 1, 2, 3, 2, 2 });
		server_3.AddDocument(5, "cat of the breed Russian blue", DocumentStatus::ACTUAL, { 1, 2, 3, 3, 2 });
		server_3.AddDocument(6, "striped gray cat with white mustache and blue eyes", DocumentStatus::BANNED, { 1, 2, 3, 4, 3 });
		server_3.AddDocument(7, "black cat with white tips of paws and muzzle", DocumentStatus::ACTUAL, { 1, 2, 3, 7 });
		server_3.AddDocument(9, "a bald cat with big ears and eyes", DocumentStatus::ACTUAL, { 1, 2, 3, 8 });
		{
			auto found_docs = server_3.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return document_id%2 != 0; });
			ASSERT_EQUAL_HINT(found_docs.size(), 5, "Must be shown 5 documents with odd id."s);
		}
		{
			auto found_docs = server_3.FindTopDocuments("fluffy cat collar"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; });
			ASSERT_EQUAL_HINT(found_docs.size(), 4, "Must be shown 4 documents with even id."s);
		}
	}
}

=======
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
	RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
	RUN_TEST(TestExcludedDocsWithMinusWords);
	RUN_TEST(TestingCorrectOutputStatus);
	RUN_TEST(TestFindCorrectAverageRating);
	RUN_TEST(TestFindCorrectRelevance);
<<<<<<< HEAD
	RUN_TEST(TestCheckCorrectSortOfRelevance);
	RUN_TEST(TestCheckCorrectPredicate);
=======
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
}



// --------- Окончание модульных тестов поисковой системы -----------

int main() {
	TestSearchServer();
	// Если вы видите эту строку, значит все тесты прошли успешно
	cout << "Search server testing finished"s << endl;
<<<<<<< HEAD
}
=======
}
>>>>>>> d7257e45c87e5dfb6a143bf389be5a3c8358c31d
