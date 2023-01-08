// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include "simple_shared_ptr.hpp"

#include <string>
#include <vector>

namespace microshellxx
{
    enum redir_type
    {
        REDIR_NONE,
        REDIR_WRITE,
        REDIR_READ,
        REDIR_APPEND,
        REDIR_HEREDOC
    };

    struct redir
    {
        redir_type type;
        std::string word;

        redir(redir_type type, const std::string& word) : type(type), word(word) {}
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

        bool is_empty() const { return this->words.empty() && this->redirs.empty(); }
        void add_word(const std::string& word) { this->words.push_back(word); }
        void add_redir(const redir& r) { this->redirs.push_back(r); }
    };

    class subshell_command : public command
    {
    private:
        uy::shared_ptr<command> container;
        std::vector<redir> redirs;

    public:
        subshell_command(const uy::shared_ptr<command>& container) : container(container) {}

        void add_redir(const redir& r) { this->redirs.push_back(r); }
    };

    class command_connection : public command
    {
    private:
        std::string connector;
        uy::shared_ptr<command> first;
        uy::shared_ptr<command> second;

    public:
        command_connection(const std::string& connector, uy::shared_ptr<command> first, uy::shared_ptr<command> second) : connector(connector), first(first), second(second) {}

        const std::string& get_connector() const { return this->connector; }
    };
}
