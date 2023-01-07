// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include "simple_shared_ptr.hpp"

#include <string>
#include <vector>

enum redir_type
{
    REDIR_WRITE,
    REDIR_READ,
    REDIR_APPEND,
    REDIR_HEREDOC
};

struct redir
{
    redir_type type;
    std::string word;
};

class command
{
public:
    virtual ~command() {}
};

class simple_command : public command
{
private:
    std::vector<std::string> words;
    std::vector<redir> redirs;

public:
    simple_command() : words(), redirs() {}
};

class subshell_command : public command
{
private:
    shared_ptr<command> container;

public:
    subshell_command(const shared_ptr<command>& container) : container(container) {}
};

class command_connection : public command
{
private:
    std::string connector;
    shared_ptr<command> first;
    shared_ptr<command> second;

public:
    command_connection(const std::string& connector, shared_ptr<command> first, shared_ptr<command> second) : connector(connector), first(first), second(second) {}

    const std::string& get_connector() const { return this->connector; }
};
