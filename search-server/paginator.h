#pragma once
#include <iostream>
#include <vector>
using namespace std;

template<typename Iterator>
class IteratorRange {
private:
    Iterator first; //начало
    Iterator last; //конец

public:

    //конструктор
    IteratorRange(Iterator beg, Iterator end) {
        first = beg;
        last = end;
    }

    //начало
    auto begin() const {
        return first;
    }

    //конец
    auto end() const {
        return last;
    }

    //размер
    int size() const {
        return distance(first, last);
    }

    //перегрузка вывода в поток
    friend ostream& operator<<(ostream& output, const IteratorRange& d) {
        for (Iterator it = d.begin(); it != d.end(); it++) {
            cout << *it;
        }
        return output;
    }
};

template <typename Iterator>
class Paginator {
private:
    vector<IteratorRange<Iterator>> pages; //данные
public:
    //конструктор
    Paginator(Iterator beg, Iterator end, size_t size) {
        int dist = distance(beg, end);
        int a = dist / size; //целых частей
        int b = dist % size; //остаток
        int i = 0; //для подсчета
        auto it = beg; //инициализация
        while (i != a * size) {
            auto prev = it; //начало
            advance(it, size); //сдвигаем на size позиций вперед
            pages.push_back({ prev, it }); //добавляем
            i += size;
        }
        //добавляем остаток если есть
        if (b) {
            auto prev = it;
            advance(it, b);
            pages.push_back({ prev, it });
        }
    }

    //начало
    auto begin() const {
        return pages.begin();
    }

    //конец
    auto end() const {
        return pages.end();
    }
};

//функцию Paginate не менять
template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator<typename Container::const_iterator>(begin(c), end(c), page_size); //без явного указания параметра шаблона не работает
}
