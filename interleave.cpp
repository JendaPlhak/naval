#include "interleave.h"


int main(int argc, char const *argv[])
{
    auto is_interleave = [](const auto& str, const auto& pat1, const auto& pat2) {
        return jeSpojenim(str, pat1, pat2);
    };
    std::cout << is_interleave("", "", "") << std::endl;
    return 0;
}
