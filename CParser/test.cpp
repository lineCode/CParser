// CParser.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Lexer.h"

#define OUTPUT(l, t) printf("[%03d:%03d] %-12s - %s\n", \
    l.get_last_line(), \
    l.get_last_column(), \
    LEX_STRING(l.get_type()).c_str(), \
    l.current().c_str());

#define TEST(l, t, v) \
    assert(l.next() == l_##t); \
    assert(l.get_##t() == v); \
    OUTPUT(l, t);

int main0()
{
    auto str = "ABC \r\n0x123\n\n0.2e8 6e5ul3f1234  a_\t_b '\\0''a' '\\'''\\n''\x41' \"\"\"abc\" \"\\\"\" \n//\n//345\n/**//*ab\nc*//bool";

    printf("# 输入 \n----[[[\n%s\n----]]]\n", str);

    CLexer lexer(str);

    printf("\n# 解析 \n");

    TEST(lexer, identifier, "ABC");
    TEST(lexer, space, 1);
    TEST(lexer, newline, 1);
    TEST(lexer, uint, 0x123);
    TEST(lexer, newline, 2);
    TEST(lexer, double, .2e8);
    TEST(lexer, space, 1);
    TEST(lexer, ulong, 6e5);
    TEST(lexer, float, 3.0f);
    TEST(lexer, int, 1234);
    TEST(lexer, space, 2);
    TEST(lexer, identifier, "a_");
    TEST(lexer, space, 1);
    TEST(lexer, identifier, "_b");
    TEST(lexer, space, 1);
    TEST(lexer, char, '\0');
    TEST(lexer, char, 'a');
    TEST(lexer, space, 1);
    TEST(lexer, char, '\'');
    TEST(lexer, char, '\n');
    TEST(lexer, char, 'A');
    TEST(lexer, space, 1);
    TEST(lexer, string, "");
    TEST(lexer, string, "abc");
    TEST(lexer, space, 1);
    TEST(lexer, string, "\"");
    TEST(lexer, space, 1);
    TEST(lexer, newline, 1);
    TEST(lexer, comment, "");
    TEST(lexer, newline, 1);
    TEST(lexer, comment, "345");
    TEST(lexer, newline, 1);
    TEST(lexer, comment, "");
    TEST(lexer, comment, "ab\nc");
    TEST(lexer, operator, op_divide);
    TEST(lexer, keyword, k_bool);

    std::cout << std::endl << std::endl << std::endl;

    {
        std::ifstream iff(R"(..\test\json11.cpp)");
        std::ostringstream os;
        os << iff.rdbuf();
        CLexer l(os.str());

        while (l.next() != l_end)
        {
            printf("[%03d:%03d] %-12s - %s\n", \
                l.get_last_line(), \
                l.get_last_column(), \
                LEX_STRING(l.get_type()).c_str(), \
                l.current().c_str());
        }
    }
    

    return 0;
}

