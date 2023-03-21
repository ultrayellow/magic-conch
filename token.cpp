// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "token.hpp"

#include <string>
#include <vector>

namespace magicconch
{
    static bool _string_contains(const std::string& str, std::string::value_type val)
    {
        return str.find(val) != std::string::npos;
    }

    static bool _accept_meta(std::string::const_iterator it, std::string::const_iterator end, std::string::size_type& out_len)
    {
        if (_string_contains(" \f\n\r\t\v", *it))
        {
            out_len = 0;
        }
        else if (_string_contains("()", *it))
        {
            out_len = 1;
        }
        else if (_string_contains("&|<>", *it))
        {
            if (it + 1 != end && *(it + 1) == *it)
            {
                out_len = 2;
            }
            else
            {
                out_len = 1;
            }
        }
        else
        {
            return false;
        }
        return true;
    }

    static std::string _make_meta_string(std::string::const_iterator it, std::string::size_type len)
    {
        std::string ret_val(it, it + len);

        if (ret_val == "&")
        {
            throw invalid_token();
        }

        return ret_val;
    }

    static std::string::const_iterator _skip_quote(std::string::const_iterator it, std::string::const_iterator end)
    {
        if (*it == '\'' || *it == '\"')
        {
            char c = *it++;

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
        std::vector<token> ret_val;

        std::string::const_iterator it = str.begin(), beg = str.end();
        do
        {
            std::string::size_type n = 0;
            if (it == str.end() || _accept_meta(it, str.end(), n))
            {
                if (beg != str.end())
                {
                    ret_val.push_back(token(false, std::string(beg, it)));
                    beg = str.end();
                }
                if (n != 0)
                {
                    ret_val.push_back(token(true, _make_meta_string(it, n)));
                    it += n - 1;
                }
            }
            else
            {
                if (beg == str.end())
                {
                    beg = it;
                }
                it = _skip_quote(it, str.end());
            }
        } while (it++ != str.end());

        return ret_val;
    }
}
