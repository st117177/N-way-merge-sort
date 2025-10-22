#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cstdio> // Для std::remove

// Временная сложность алгоритма: O(M log N), где M - общее количество элементов, N - количество списков.

// Структура, представляющая элемент в минимальной куче.
// Используется для отслеживания наименьшего доступного числа из N списков.
struct Element {
    int value;
    size_t list_index;      // Индекс исходного списка (использован size_t).
    size_t element_index;   // Позиция элемента внутри списка (использован size_t).

    // Перегружает оператор > для создания min-кучи в std::priority_queue.
    bool operator>(const Element& other) const {
        return value > other.value;
    }
};

// Класс для выполнения N-стороннего слияния отсортированных списков.
class NWayMerger {
public:
    // Выполняет N-стороннее слияние и управляет файловым вводом/выводом.
    // Читает N отсортированных списков из входного файла, выполняет слияние
    // и записывает один отсортированный список в выходной файл.
    void MergeAndWrite(const std::string& input_file_path,
        const std::string& output_file_path) {

        // Удалено дублирующее объявление: теперь это единственное объявление и определение
        // функции MergeAndWrite.
        std::vector<std::vector<int>> sorted_lists = ReadInputFile(input_file_path);
        std::vector<int> result = MergeLists(sorted_lists);
        WriteOutputFile(output_file_path, result);
    }

private:
    // Читает входной файл и парсит данные в вектор отсортированных списков.
    // Возвращает: Вектор векторов чисел (N отсортированных списков).
    std::vector<std::vector<int>> ReadInputFile(const std::string& file_path) {
        std::ifstream input_file(file_path);
        if (!input_file.is_open()) {
            throw std::runtime_error("Не удалось открыть входной файл: " + file_path);
        }

        int n;
        if (!(input_file >> n)) {
            // Проверяем на пустой файл или некорректный N.
            if (input_file.eof()) return {};
            throw std::runtime_error("Не удалось прочитать количество списков N.");
        }

        // Пропускаем символ новой строки после N
        input_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::vector<std::vector<int>> sorted_lists;
        std::string line;

        for (int i = 0; i < n; ++i) {
            if (std::getline(input_file, line)) {
                std::vector<int> current_list;
                std::stringstream ss(line);
                int number;
                while (ss >> number) {
                    current_list.push_back(number);
                }
                sorted_lists.push_back(std::move(current_list));
            }
            else {
                throw std::runtime_error("Непредвиденный конец файла при чтении списков.");
            }
        }

        return sorted_lists;
    }

    // Выполняет слияние N отсортированных списков с использованием минимальной кучи.
    // Возвращает: Один отсортированный вектор, содержащий все элементы.
    std::vector<int> MergeLists(const std::vector<std::vector<int>>& lists) {
        // Минимальная куча, использующая Element и std::greater<Element>
        std::priority_queue<Element, std::vector<Element>, std::greater<Element>> min_heap;

        // 1. Инициализация: Добавляем первые элементы каждого списка в кучу
        for (size_t i = 0; i < lists.size(); ++i) {
            if (!lists[i].empty()) {
                min_heap.push({
                    lists[i][0],            // value
                    i,                      // list_index (size_t)
                    0                       // element_index (size_t)
                    });
            }
        }

        std::vector<int> result;
        // 2. Извлечение и вставка: Пока куча не пуста
        while (!min_heap.empty()) {
            // Извлекаем минимальный элемент
            Element min_el = min_heap.top();
            min_heap.pop();

            // Добавляем значение в результат
            result.push_back(min_el.value);

            // 3. Вставляем следующий элемент из того же списка, если он существует
            size_t next_element_index = min_el.element_index + 1;
            size_t list_idx = min_el.list_index;

            // Сравнение size_t < size_t теперь чистое и безопасное.
            if (next_element_index < lists[list_idx].size()) {
                min_heap.push({
                    lists[list_idx][next_element_index],
                    list_idx,
                    next_element_index
                    });
            }
        }

        return result;
    }
    // Записывает отсортированный результат в выходной файл.
    // Если файл не открывается, выбрасывает std::runtime_error.
    void WriteOutputFile(const std::string& file_path, const std::vector<int>& data) {
        std::ofstream output_file(file_path);
        if (!output_file.is_open()) {
            throw std::runtime_error("Не удалось открыть выходной файл: " + file_path);
        }

        for (size_t i = 0; i < data.size(); ++i) {
            output_file << data[i];
            if (i < data.size() - 1) {
                output_file << " "; // Разделитель пробелом
            }
        }
        output_file << "\n"; // Добавляем новую строку в конце, как принято
    }
};

// Функция для создания входного файла для тестирования.
void CreateInputFile(const std::string& file_path, int n, const std::vector<std::string>& lists) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось создать тестовый файл.");
    }
    file << n << "\n";
    for (const auto& list : lists) {
        file << list << "\n";
    }
}

// Функция для чтения содержимого файла для проверки.
std::string ReadOutputFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Простой модульный тест для проверки функциональности.
void RunTest(const std::string& test_name, int n, const std::vector<std::string>& input_lists,
    const std::string& expected_output) {

    std::string input_file = "input_" + test_name + ".txt";
    std::string output_file = "output_" + test_name + ".txt";

    try {
        CreateInputFile(input_file, n, input_lists);

        NWayMerger merger;
        merger.MergeAndWrite(input_file, output_file);

        std::string actual_output = ReadOutputFile(output_file);

        if (actual_output.size() > 0 && actual_output.back() == '\n') {
            actual_output.pop_back(); // Удаляем конечный \n для сравнения
        }

        if (actual_output == expected_output) {
            std::cout << "[Успешно] Тест '" << test_name << "'.\n";
        }
        else {
            std::cout << "[Провал] Тест '" << test_name << "'.\n";
            std::cout << "  Ожидалось: '" << expected_output << "'\n";
            std::cout << "  Получено:  '" << actual_output << "'\n";
        }

        // В реальных тестах здесь можно удалить временные файлы:
        // std::remove(input_file.c_str());
        // std::remove(output_file.c_str());

    }
    catch (const std::exception& e) {
        std::cout << "[Ошибка] Тест '" << test_name << "'. Исключение: " << e.what() << "\n";
    }
}

// Главная функция, запускающая модульные тесты и демонстрирующая использование.
int main() {
    // Устанавливаем локаль для корректного вывода
    std::locale::global(std::locale(""));
    std::cout << "--- Запуск модульных тестов для N-Way Merge ---\n";

    // Тест 1: Базовое слияние трех списков
    RunTest("Базовый", 3,
        { "1 4 5", "2 6 8 9", "0 3 7 10 11" },
        "0 1 2 3 4 5 6 7 8 9 10 11");

    // Тест 2: Списки с отрицательными числами и дубликатами
    RunTest("Отрицательные и Дубликаты", 4,
        { "-5 -1 0", "-10 0 0 1", "5 10", "2 3 3" },
        "-10 -5 -1 0 0 0 1 2 3 3 5 10");

    // Тест 3: С одним пустым списком
    RunTest("Пустой Список", 3,
        { "10 20", "", "5 15" },
        "5 10 15 20");

    // Тест 4: Слияние одного списка (N=1)
    RunTest("Один Список", 1,
        { "1 2 3 4 5" },
        "1 2 3 4 5");

    // Тест 5: Все списки пустые
    RunTest("Все Пустые", 2,
        { "", "" },
        "");

    std::cout << "\n--- Пример использования с реальными файлами (см. output_Пример.txt) ---\n";
    try {
        // Создаем входной файл для примера
        CreateInputFile("input_Пример.txt", 3,
            { "-5 0 5 10 15", "1 2 3", "7 9 11" });

        NWayMerger merger_example;
        merger_example.MergeAndWrite("input_Пример.txt", "output_Пример.txt");

        std::cout << "Успешное слияние. Результат записан в файл 'output_Пример.txt'.\n";
        // Выводим содержимое для удобства проверки в консоли
        std::cout << "Содержимое 'output_Пример.txt':\n" << ReadOutputFile("output_Пример.txt");

    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
    }

    return 0;
}