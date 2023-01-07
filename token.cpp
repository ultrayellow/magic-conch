// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "token.hpp"

#include <string>
#include <vector>

std::vector<token> lex(const std::string& str)
{
    std::vector<token> ret_val;

    std::string::const_iterator it = str.begin(), beg = str.end();
    do
    {
        bool split = true;
        std::string::size_type n = 1;
        if (it == str.end())
        {
            n = 0;
        }
        else
        {
            switch (*it)
            {
            case ' ':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
                n = 0;
                break;

            case '&':
            case '|':
            case '<':
            case '>':
                if (it + 1 != str.end() && *(it + 1) == *it)
                {
                    n = 2;
                }
                break;

            case '(':
            case ')':
                break;

            default:
                split = false;
                break;
            }
        }

        if (split)
        {
            if (beg != str.end())
            {
                ret_val.push_back(token(false, std::string(beg, it)));
                beg = str.end();
            }
            if (n != 0)
            {
                std::string meta(it, it + n);
                ret_val.push_back(token(true, meta));
                if (meta == "&")
                {
                    throw invalid_token();
                }
                it += n - 1;
            }
        }
        else
        {
            if (beg == str.end())
            {
                beg = it;
            }
            if (*it == '\'' || *it == '\"')
            {
                char c = *it++;
                while (it != str.end() && *it != c)
                {
                    ++it;
                }
                if (it == str.end())
                {
                    throw bad_quote_pair();
                }
            }
        }
    } while (it++ != str.end());

    return ret_val;
}
