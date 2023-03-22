// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include "command.hpp"
#include "token.hpp"

#include <exception>
#include <memory>
#include <string>
#include <vector>

namespace magicconch
{
    class parser
    {
    public:
        typedef std::vector<token>::iterator iterator;

    private:
        iterator it;
        iterator end;

    public:
        parser(std::vector<token>& toks);

    private:
        void next();
        bool next_if_meta(const std::string& meta);
        bool next_if_word();

        std::shared_ptr<command> next_list();
        std::shared_ptr<command> next_pipeline();
        std::shared_ptr<command> next_command();
        std::shared_ptr<simple_command> next_simple_command();

    public:
        std::shared_ptr<command> do_parse();
    };

    class end_of_token : public std::exception
    {
    public:
        end_of_token() noexcept {}
        virtual ~end_of_token() noexcept {}
        virtual const char* what() const noexcept { return "reached end of token"; }
    };

    class syntax_error : public std::exception
    {
    public:
        syntax_error() noexcept {}
        virtual ~syntax_error() noexcept {}
        virtual const char* what() const noexcept { return "syntax error"; }
    };
}
