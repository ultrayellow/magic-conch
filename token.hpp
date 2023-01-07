// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include <exception>
#include <string>
#include <vector>

struct token
{
    bool meta;
    std::string str;

    token(bool meta, const std::string& str) : meta(meta), str(str) {}
};

std::vector<token> lex(const std::string& str);

class bad_quote_pair : public std::exception
{
public:
    bad_quote_pair() throw() {}
    virtual ~bad_quote_pair() throw() {}
    virtual const char* what() const throw() { return "bad quote pair"; }
};

class invalid_token : public std::exception
{
public:
    invalid_token() throw() {}
    virtual ~invalid_token() throw() {}
    virtual const char* what() const throw() { return "invalid token"; }
};
