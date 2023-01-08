// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include "command.hpp"
#include "simple_shared_ptr.hpp"
#include "token.hpp"

#include <exception>
#include <string>
#include <vector>

namespace microshellxx
{
    class parser
    {
    public:
        typedef std::vector<token>::const_iterator iterator;

    private:
        iterator it;
        iterator end;

    public:
        parser(const std::vector<token>& toks);

    private:
        void next();
        bool next_if(const std::string& meta);

        uy::shared_ptr<command> next_list();
        uy::shared_ptr<command> next_pipeline();
        uy::shared_ptr<command> next_command();
        uy::shared_ptr<simple_command> next_simple_command();

    public:
        uy::shared_ptr<command> do_parse();
    };

    class end_of_token : public std::exception
    {
    public:
        end_of_token() throw() {}
        virtual ~end_of_token() throw() {}
        virtual const char* what() const throw() { return "reached end of token"; }
    };

    class syntax_error : public std::exception
    {
    public:
        syntax_error() throw() {}
        virtual ~syntax_error() throw() {}
        virtual const char* what() const throw() { return "syntax error"; }
    };
}
