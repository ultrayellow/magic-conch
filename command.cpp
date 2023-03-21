// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "command.hpp"

#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include <iostream>
extern char** environ; // temporary
#include <sys/stat.h>

namespace magicconch
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

    int simple_command::execute(int pipe_in, int pipe_out)
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
            if (pipe_in != NO_PIPE)
            {
                if (::dup2(pipe_in, STDIN_FILENO) < 0)
                {
                    // TODO: safe dup2?
                    ::exit(EXIT_FAILURE);
                }
                ::close(pipe_in);
            }
            if (pipe_out != NO_PIPE)
            {
                if (::dup2(pipe_out, STDOUT_FILENO) < 0)
                {
                    // TODO: safe dup2?
                    exit(EXIT_FAILURE);
                }
                ::close(pipe_out);
            }
            if (this->next_pipe != NO_PIPE)
            {
                ::close(this->next_pipe);
            }

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
            this->pid = child;
            int status = EXIT_SUCCESS;
            if (pipe_out == NO_PIPE)
            {
                ::waitpid(child, &status, 0);
            }
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
            std::string redir_text;
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
            default:
                redir_text = "(none)";
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

    int subshell_command::execute(int pipe_in, int pipe_out)
    {
        static_cast<void>(pipe_in);
        static_cast<void>(pipe_out);
        std::cout << "Not implemented yet." << std::endl;
        return 0;
    }

    std::string subshell_command::to_string() const
    {
        std::ostringstream oss;

        oss << "( " << this->container->to_string() << " )";

        for (std::vector<redir>::const_iterator it = this->redirs.begin(); it != this->redirs.end(); ++it)
        {
            std::string redir_text;
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
            default:
                redir_text = "(none)";
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

    static int _execute_pipeline(const command_connection& pipeline, int pipe_in, int pipe_out)
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

        int prev = pipe_in;
        int fildes[2];
        int status = EXIT_SUCCESS;
        for (std::vector<uy::shared_ptr<command> >::iterator it = cmdv.begin(); it != cmdv.end(); ++it)
        {
            const uy::shared_ptr<command>& cmd = *it;
            bool last = it + 1 == cmdv.end();
            if (last)
            {
                fildes[0] = NO_PIPE;
                fildes[1] = pipe_out;
            }
            else
            {
                if (::pipe(fildes) < 0)
                {
                    // TODO: safe_pipe?
                    ::exit(EXIT_FAILURE);
                }
            }
            cmd->set_next_pipe(fildes[0]);
            status = cmd->execute(prev, fildes[1]);
            if (prev != NO_PIPE)
            {
                ::close(prev);
            }
            prev = fildes[0];
            ::close(fildes[1]);
        }
        if (pipe_out == NO_PIPE)
        {
            for (std::vector<uy::shared_ptr<command> >::iterator it = cmdv.begin(); it != cmdv.end(); ++it)
            {
                const uy::shared_ptr<command>& cmd = *it;
                int status_internal;
                ::waitpid(cmd->get_pid(), &status_internal, 0);
            }
        }
        return status;
    }

    int command_connection::execute(int pipe_in, int pipe_out)
    {
        if (this->connector == "|")
        {
            return _execute_pipeline(*this, pipe_in, pipe_out);
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
