// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#define NO_PIPE (-1)

namespace magicconch
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

        inline redir(redir_type type, std::string&& word) : type(type), word(word) {}
    };

    class command
    {
    protected:
        pid_t pid;
        int next_pipe;

    public:
        inline command() : pid(), next_pipe(NO_PIPE) {}
        inline virtual ~command() {}

        inline pid_t get_pid() const { return this->pid; }
        inline void set_next_pipe(int next_pipe) { this->next_pipe = next_pipe; }

        virtual int execute(int pipe_in, int pipe_out) = 0;
        virtual std::string to_string() const = 0;
    };

    class simple_command : public command
    {
    private:
        std::vector<std::string> words;
        std::vector<redir> redirs;

    public:
        inline simple_command() : words(), redirs() {}

        inline bool is_empty() const { return this->words.empty() && this->redirs.empty(); }
        inline const std::vector<std::string>& get_words() const { return this->words; }
        inline const std::vector<redir>& get_redirs() const { return this->redirs; }

        inline void add_word(std::string&& word) { this->words.emplace_back(std::move(word)); }
        inline void add_redir(redir_type type, std::string&& word) { this->redirs.emplace_back(type, std::move(word)); }

        int execute(int pipe_in, int pipe_out);
        std::string to_string() const;
    };

    class subshell_command : public command
    {
    private:
        std::shared_ptr<command> container;
        std::vector<redir> redirs;

    public:
        inline subshell_command(const std::shared_ptr<command>& container) : container(container) {}

        inline void add_redir(redir_type type, std::string&& word) { this->redirs.emplace_back(type, std::move(word)); }
        inline const std::shared_ptr<command>& get_container() const { return this->container; }

        int execute(int pipe_in, int pipe_out);
        std::string to_string() const;
    };

    class command_connection : public command
    {
    private:
        std::string connector;
        std::shared_ptr<command> first;
        std::shared_ptr<command> second;

    public:
        inline command_connection(std::string&& connector, std::shared_ptr<command> first, std::shared_ptr<command> second) : connector(connector), first(first), second(second) {}

        inline const std::string& get_connector() const { return this->connector; }
        inline const std::shared_ptr<command>& get_first() const { return this->first; }
        inline const std::shared_ptr<command>& get_second() const { return this->second; }

        int execute(int pipe_in, int pipe_out);
        std::string to_string() const;
    };
}
