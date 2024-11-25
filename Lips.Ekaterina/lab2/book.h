#pragma once

#include <string>
#include <vector>

struct Book {
    std::string name;
    int count;
};

const std::vector<Book> books = {
    {"Сборник стихов Пушкина", 7},
    {"Сборник стихов Есенина", 10},
    {"Детские сказки", 0},
    {"Журнал Мой Друг", 15}
};