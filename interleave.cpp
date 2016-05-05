
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// template< class I >
// class cyclic_iterator
//  /* : public iterator< bidirectional, yadda yadda > */ {
//     I it, beg, end;
//     int cnt;
//     cyclic_iterator( int c, I f, I l )
//         : it( f ), beg( f ), end( l ), cnt( c ) {}
// public:
//     cyclic_iterator() : it(), beg(), end(), cnt() {}

//     cyclic_iterator &operator++() {
//         ++it;
//         if ( it == end ) {
//             ++cnt;
//             it = beg;
//         }
//     } // etc for --, post-operations

//     friend bool operator==
//         ( cyclic_iterator const &lhs, cyclic_iterator const &rhs )
//         { return lhs.it == rhs.it && lhs.cnt == rhs.cnt; } // etc for !=

//     friend pair< cyclic_iterator, cyclic_iterator > cycle_range
//         ( int c, I f, I l ) {//factory function, better style outside this scope
//         return make_pair( cyclic_iterator( 0, f, l ),
//                           cyclic_iterator( c, f, l ) );
//     }
// };

class Search {
private:
    std::string m_str;
    std::string m_pat1;
    std::string m_pat2;
public:
    Search(const std::string& str, const std::string& pat1,
        const std::string& pat2)
     : m_str(str), m_pat1(pat1), m_pat2(pat2)
    { }

    bool rec(size_t i, size_t j, size_t k) {
        // Base Case: If all strings are empty
        if (i >= m_str.size()) {
            return true;
        }

        // If any of the above mentioned two possibilities is true,
        // then return true, otherwise false
        return ( (m_str.at(i)   == m_pat1.at(j % m_pat1.size())) && this->rec(i + 1, j + 1, k))
               || ((m_str.at(i) == m_pat2.at(k % m_pat2.size())) && this->rec(i + 1, j, k + 1));
    }
};

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
    Search S(str, pat1_str, pat2_str );
    S.rec(0, 0, 0);

    CycleStr pat1(pat1_str);
    CycleStr pat2(pat2_str);

    size_t size = str.size() + 1;
    std::vector<std::vector<bool>> IL(size, std::vector<bool>(size, false));

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j + i < size; ++j) {
            // two empty strings have an empty string as interleaving
            if (i == 0 && j == 0) {
                IL[i][j] = true;
            }
            // Nothing chosen from pat1
            else if (i == 0 && pat2[j - 1] == str[j - 1]) {
                IL[i][j] = IL[i][j - 1];
            }
            // Nothing chosen from pat2
            else if (j == 0 && pat1[i - 1] == str[i - 1]) {
                IL[i][j] = IL[i-1][j];
            }

            if (j == 0 || i == 0) {
                continue;
            }
            char c = str[i + j - 1];
            IL[i][j] = IL[i-1][j] && (pat1[i-1] == c || pat2[j - 1] == c);

            // // Current character of str matches with current character of pat1,
            // // but doesn't match with current character of pat2
            // else if(pat1[i-1] == str[i + j - 1] && pat2[j - 1] != str[i + j - 1]) {
            //     IL[i][j] = IL[i-1][j];
            // }
            // // Current character of C matches with current character of pat2,
            // // but doesn't match with current character of pat1
            // else if (pat1[i - 1] != str[i + j - 1] && pat2[j-1] == str[i + j - 1]) {
            //     IL[i][j] = IL[i][j-1];
            // }

            // // Current character of C matches with that of both pat1 and pat2
            // else if (pat1[i - 1] == str[i + j - 1] && pat2[j - 1] == str[i + j - 1]) {
            //     IL[i][j] = IL[i-1][j] || IL[i][j-1];
            // }
        }
    }
    dump(IL);
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
        Search S(str, pat1, pat2);
        assert(jeSpojenim(str, pat1, pat2) == S.rec(0, 0, 0));
        std::cout << jeSpojenim(str, pat1, pat2) << std::endl;;
        return S.rec(0, 0, 0);
    };
    std::cout << is_interleave("SLW.", "SLOVO", "WORD") << std::endl;
    return 0;
}
