#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

struct Node {
    std::string word;
    Node* left;
    Node* right;
};

class Dictionary {
public:
    Dictionary() : root(nullptr) {}

    void buildDictionaryFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Не вдалося відкрити файл: " << filename << std::endl;
            return;
        }

        std::string line, word;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            while (iss >> word) {
                insert(word);
            }
        }

        file.close();
    }

    void printDictionary() {
        if (!root) {
            std::cout << "Словник порожній" << std::endl;
            return;
        }
        printDictionaryRecursive(root);
    }

    bool search(const std::string& word) {
        return searchRecursive(root, word);
    }

    void addWord(const std::string& word) {
        insert(word);
        saveToFile(word);
    }

    void deleteWord(const std::string& word) {
        deleteWordRecursive(root, word);
        removeFromFile(word);
    }

    void compareSearchTime(const std::string& word) {
        auto startTree = std::chrono::steady_clock::now();
        bool foundInTree = search(word);
        auto endTree = std::chrono::steady_clock::now();
        std::chrono::duration<double> treeTime = endTree - startTree;
        std::cout << "Час пошуку у словнику: " << treeTime.count() << " секунд" << std::endl;

        auto startFile = std::chrono::steady_clock::now();
        bool foundInFile = searchInFile(word);
        auto endFile = std::chrono::steady_clock::now();
        std::chrono::duration<double> fileTime = endFile - startFile;
        std::cout << "Час пошуку у файлі: " << fileTime.count() << " секунд" << std::endl;

        if (foundInTree != foundInFile) {
            std::cout << "Результати пошуку відрізняються між словником і файлом" << std::endl;
        }
    }

private:
    Node* root;

    void insert(const std::string& word) {
        root = insertRecursive(root, word);
    }

    Node* insertRecursive(Node* current, const std::string& word) {
        if (current == nullptr) {
            Node* newNode = new Node;
            newNode->word = word;
            newNode->left = nullptr;
            newNode->right = nullptr;
            return newNode;
        }

        if (word < current->word) {
            current->left = insertRecursive(current->left, word);
        }
        else if (word > current->word) {
            current->right = insertRecursive(current->right, word);
        }

        return current;
    }

    void printDictionaryRecursive(Node* current) {
        if (current == nullptr) {
            return;
        }

        printDictionaryRecursive(current->left);
        std::cout << current->word << std::endl;
        printDictionaryRecursive(current->right);
    }

    bool searchRecursive(Node* current, const std::string& word) {
        if (current == nullptr) {
            return false;
        }
        if (current->word == word) {
            return true;
        }
        if (word < current->word) {
            return searchRecursive(current->left, word);
        }
        return searchRecursive(current->right, word);
    }

    void deleteWordRecursive(Node*& current, const std::string& word) {
        if (current == nullptr) {
            return;
        }

        if (word < current->word) {
            deleteWordRecursive(current->left, word);
        }
        else if (word > current->word) {
            deleteWordRecursive(current->right, word);
        }
        else {
            if (current->left == nullptr && current->right == nullptr) {
                delete current;
                current = nullptr;
            }
            else if (current->left == nullptr) {
                Node* temp = current;
                current = current->right;
                delete temp;
            }
            else if (current->right == nullptr) {
                Node* temp = current;
                current = current->left;
                delete temp;
            }
            else {
                Node* minRight = findMin(current->right);
                current->word = minRight->word;
                deleteWordRecursive(current->right, minRight->word);
            }
        }
    }

    Node* findMin(Node* current) {
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    void saveToFile(const std::string& word) {
        std::ofstream file("dictionary.txt", std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Не вдалося відкрити файл для запису" << std::endl;
            return;
        }
        file << word << std::endl;
        file.close();
    }

    bool searchInFile(const std::string& word) {
        std::ifstream file("dictionary.txt");
        if (!file.is_open()) {
            std::cerr << "Не вдалося відкрити файл для читання" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line == word) {
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    void removeFromFile(const std::string& word) {
        std::ifstream inFile("dictionary.txt");
        if (!inFile.is_open()) {
            std::cerr << "Не вдалося відкрити файл для читання" << std::endl;
            return;
        }

        std::ofstream outFile("temp.txt");
        if (!outFile.is_open()) {
            std::cerr << "Не вдалося відкрити файл для запису" << std::endl;
            inFile.close();
            return;
        }

        std::string line;
        while (std::getline(inFile, line)) {
            if (line != word) {
                outFile << line << std::endl;
            }
        }

        inFile.close();
        outFile.close();

        if (remove("dictionary.txt") != 0) {
            std::cerr << "Помилка видалення файлу" << std::endl;
            return;
        }

        if (rename("temp.txt", "dictionary.txt") != 0) {
            std::cerr << "Помилка перейменування файлу" << std::endl;
        }
    }
};

int main() 
{
    setlocale(LC_ALL, "Ukr");
    Dictionary dict;
    dict.buildDictionaryFromFile("text.txt");

    std::cout << "Словник:" << std::endl;
    dict.printDictionary();

    std::string wordToFind = "яблуко";
    std::cout << "Пошук слова '" << wordToFind << "':" << std::endl;
    if (dict.search(wordToFind)) {
        std::cout << "Слово знайдено у словнику" << std::endl;
    }
    else {
        std::cout << "Слово не знайдено у словнику" << std::endl;
        dict.addWord(wordToFind);
    }

    std::string wordToDelete = "банан";
    std::cout << "Видалення слова '" << wordToDelete << "' зі словника" << std::endl;
    dict.deleteWord(wordToDelete);

    std::string wordToCompare = "виноград";
    dict.compareSearchTime(wordToCompare);

    return 0;
}
