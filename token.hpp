// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include <exception>
#include <string>
#include <utility>
#include <vector>

namespace magicconch
{
    class token
    {
    private:
        bool meta;
        std::string str;

    public:
        inline token(bool meta, std::string&& str) : meta(meta), str(str) {}

        inline bool is_word() const { return !this->meta; }
        inline bool is_meta(const std::string& str) const { return this->meta && this->str == str; }

        inline std::string&& detach_word() { return std::move(this->str); }
    };

    std::vector<token> lex(const std::string& str);

    class bad_quote_pair : public std::exception
    {
    public:
        bad_quote_pair() noexcept {}
        virtual ~bad_quote_pair() noexcept {}
        virtual const char* what() const noexcept { return "bad quote pair"; }
    };

    class invalid_token : public std::exception
    {
    public:
        invalid_token() noexcept {}
        virtual ~invalid_token() noexcept {}
        virtual const char* what() const noexcept { return "invalid token"; }
    };
}
