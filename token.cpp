// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "token.hpp"

#include <string>
#include <utility>
#include <vector>

namespace magicconch
{
    static bool _string_contains(const std::string& str, std::string::value_type val)
    {
        return str.find(val) != std::string::npos;
    }

    static std::string::size_type _accept_meta(std::string::const_iterator it, std::string::const_iterator end)
    {
        if (it == end || _string_contains(" \f\n\r\t\v", *it))
        {
            return 0;
        }
        else if (_string_contains("()", *it))
        {
            return 1;
        }
        else if (_string_contains("&|<>", *it))
        {
            if (it + 1 != end && *(it + 1) == *it)
            {
                return 2;
            }
            else
            {
                return 1;
            }
        }

        return std::string::npos;
    }

    static std::string::const_iterator _skip_quote(std::string::const_iterator it, std::string::const_iterator end)
    {
        if (*it == '\'' || *it == '\"')
        {
            std::string::value_type c = *it++;

            while (it != end && *it != c)
            {
                ++it;
            }

            if (it == end)
            {
                throw bad_quote_pair();
            }
        }

        return it;
    }

    std::vector<token> lex(const std::string& str)
    {
        std::vector<token> tokens;

        const auto end = str.end();
        for (auto it = str.begin(), word_begin = end; it != end || word_begin != end; ++it)
        {
            const std::string::size_type n = _accept_meta(it, end);
            if (n == std::string::npos)
            {
                if (word_begin == end)
                {
                    // set begin position
                    word_begin = it;
                }
                it = _skip_quote(it, end);
            }
            else
            {
                if (word_begin != end)
                {
                    // commit word
                    tokens.emplace_back(false, std::string(word_begin, it));
                    // reset begin position
                    word_begin = end;
                    if (it == end)
                    {
                        break;
                    }
                }

                if (n != 0)
                {
                    std::string meta(it, it + n);

                    if (meta == "&")
                    {
                        throw invalid_token();
                    }

                    tokens.emplace_back(true, std::move(meta));
                    it += n - 1; // consider `++it`
                }
            }
        }

        return tokens;
    }
}
