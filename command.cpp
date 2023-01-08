// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "command.hpp"

#include <unistd.h>
#include <wait.h>

#include <sstream>

#include <iostream>
extern char** environ; // temporary
#include <sys/stat.h>

namespace microshellxx
{
    bool simple_command::is_empty() const
    {
        return this->words.empty() && this->redirs.empty();
    }

    const std::vector<std::string>& simple_command::get_words() const
    {
        return this->words;
    }

    const std::vector<redir>& simple_command::get_redirs() const
    {
        return this->redirs;
    }

    void simple_command::add_word(const std::string& word)
    {
        this->words.push_back(word);
    }

    void simple_command::add_redir(const redir& r)
    {
        this->redirs.push_back(r);
    }

    static void _resolve_path(std::string& path)
    {
        // TODO: ...
        std::string path_env = getenv("PATH");
        std::string::size_type beg = 0;
        for (;;)
        {
            std::string::size_type end = path_env.find(':', beg);
            std::string final_path = path_env.substr(beg, end - beg).append(1, '/').append(path);

            struct stat st;
            if (stat(final_path.c_str(), &st) == 0)
            {
                path = final_path;
                break;
            }

            if (end == std::string::npos)
            {
                break;
            }
            beg = end + 1;
        }
    }

    int simple_command::execute() const
    {
        std::vector<std::string> argv;
        for (std::vector<std::string>::const_iterator it = this->words.begin(); it != this->words.end(); ++it)
        {
            std::string arg = *it;
            // TODO: expand arg
            argv.push_back(arg);
        }

        if (argv.empty())
        {
            // TODO: ...
            return 0;
        }

        std::string path = argv[0];
        _resolve_path(path);

        pid_t child = ::fork();
        if (child == 0)
        {
            std::vector<const char*> argv_c_str;
            argv_c_str.reserve(argv.size() + 1);
            for (std::vector<std::string>::const_iterator it = argv.begin(); it != argv.end(); ++it)
            {
                argv_c_str.push_back(it->c_str());
            }
            argv_c_str.push_back(NULL);
            ::execve(path.c_str(), const_cast<char**>(&argv_c_str[0]), environ);
            // TODO: ...
            ::exit(0);
        }
        else
        {
            // TODO: ...
            int status;
            ::waitpid(child, &status, 0);
            return status;
        }
    }

    std::string simple_command::to_string() const
    {
        std::ostringstream oss;

        std::size_t i = 0;
        for (std::vector<std::string>::const_iterator it = this->words.begin(); it != this->words.end(); ++it)
        {
            if (i != 0)
            {
                oss << " ";
            }
            oss << "`" << *it << "`";
            i++;
        }

        i = 0;
        for (std::vector<redir>::const_iterator it = this->redirs.begin(); it != this->redirs.end(); ++it)
        {
            if (i != 0)
            {
                oss << " ";
            }
            std::string redir_text = "[NONE]";
            switch (it->type)
            {
            case REDIR_WRITE:
                redir_text = ">";
                break;
            case REDIR_READ:
                redir_text = "<";
                break;
            case REDIR_APPEND:
                redir_text = ">>";
                break;
            case REDIR_HEREDOC:
                redir_text = "<<";
                break;
            }
            oss << redir_text << " `" << it->word << "`";
            i++;
        }

        return oss.str();
    }

    void subshell_command::add_redir(const redir& r)
    {
        this->redirs.push_back(r);
    }

    const uy::shared_ptr<command>& subshell_command::get_container() const
    {
        return this->container;
    }

    int subshell_command::execute() const
    {
        std::cout << "Not implemented yet." << std::endl;
        return 0;
    }

    std::string subshell_command::to_string() const
    {
        std::ostringstream oss;

        oss << "( " << this->container->to_string() << " )";

        for (std::vector<redir>::const_iterator it = this->redirs.begin(); it != this->redirs.end(); ++it)
        {
            std::string redir_text = "[NONE]";
            switch (it->type)
            {
            case REDIR_WRITE:
                redir_text = ">";
                break;
            case REDIR_READ:
                redir_text = "<";
                break;
            case REDIR_APPEND:
                redir_text = ">>";
                break;
            case REDIR_HEREDOC:
                redir_text = "<<";
                break;
            }
            oss << " " << redir_text << " `" << it->word << "`";
        }

        return oss.str();
    }

    const std::string& command_connection::get_connector() const
    {
        return this->connector;
    }

    const uy::shared_ptr<command>& command_connection::get_first() const
    {
        return this->first;
    }

    const uy::shared_ptr<command>& command_connection::get_second() const
    {
        return this->second;
    }

    static int _execute_pipeline(const command_connection& pipeline)
    {
        std::vector<uy::shared_ptr<command> > cmdv;
        cmdv.push_back(pipeline.get_second());
        uy::shared_ptr<command> child = pipeline.get_first();
        for (;;)
        {
            uy::shared_ptr<command_connection> child_tree = uy::dynamic_pointer_cast<command_connection>(child);
            if (child_tree)
            {
                cmdv.push_back(child_tree->get_second());
                child = child_tree->get_first();
            }
            else
            {
                cmdv.push_back(child);
                break;
            }
        }
        std::reverse(cmdv.begin(), cmdv.end());

        for (const uy::shared_ptr<command>& cmd : cmdv)
        {
            // TODO: exec
            std::cout << "PIPE: " << cmd->to_string() << std::endl;
        }
        return 0;
    }

    int command_connection::execute() const
    {
        if (this->connector == "|")
        {
            return _execute_pipeline(*this);
        }
        else
        {
            std::cout << "Not implemented yet. : " << this->connector << std::endl;
            return -1;
        }
    }

    std::string command_connection::to_string() const
    {
        std::ostringstream oss;

        oss << this->first->to_string() << " " << this->connector << " " << this->second->to_string();

        return oss.str();
    }
}
