
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

void dump(const std::vector<std::vector<bool>>& array)
{
    for (const auto& row : array) {
        for (bool e : row) {
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }
}

class CycleStr {
private:
    std::string m_str;

public:
    CycleStr(const std::string& str) : m_str(str) {
        assert(!m_str.empty());
    }
    char operator[](size_t i) {
        return m_str[i % m_str.size()];
    }
};

// A simple recursive function to check whether C is an interleaving of A and B
bool jeSpojenim(std::string str, std::string pat1_str, std::string pat2_str)
{
    if (pat1_str.empty() && pat2_str.empty()) {
        return str.empty();
    }
    if (pat1_str.empty()) {
        std::swap(pat1_str, pat2_str);
    }

    CycleStr pat1(pat1_str);
    // Simple when one of patterns is empty
    if (pat2_str.empty()) {
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] != pat1[i]) {
                return false;
            }
        }
        return true;
    }

    CycleStr pat2(pat2_str);
    size_t size = str.size() + 1;
    std::vector<std::vector<bool>> IL(size, std::vector<bool>(size, false));

    IL[0][0] = true;
    for (size_t i = 1; i < size; ++i) {
        // Nothing chosen from pat1
        IL[0][i] = IL[0][i-1] && pat2[i-1] == str[i-1];
        // Nothing chosen from pat2
        IL[i][0] = IL[i-1][0] && pat1[i-1] == str[i-1];
    }

    for (size_t i = 1; i < size; ++i) {
        for (size_t j = 1; j + i < size; ++j) {
            char c = str[i + j - 1];
            IL[i][j] = (IL[i-1][j] && pat1[i-1] == c)
                    || (IL[i][j-1] && pat2[j-1] == c);
        }
    }
    // dump(IL);
    for (size_t i = 0; i < size; ++i) {
        if (IL[i][size - 1 - i]) {
            return true;
        }
    }
    return false;
}


int main(int argc, char const *argv[])
{
    auto is_interleave = [](const auto& str, const auto& pat1, const auto& pat2) {
        return jeSpojenim(str, pat1, pat2);
    };
    std::cout << is_interleave("", "", "") << std::endl;
    return 0;
}
