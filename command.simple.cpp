// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "command.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include <iostream>    // temporary
extern char** environ; // temporary
#include <sys/stat.h>  // temporary

namespace magicconch
{
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
        for (const std::string& arg : this->words)
        {
            // TODO: expand arg
            argv.emplace_back(arg);
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
            for (const std::string& s : argv)
            {
                argv_c_str.push_back(s.c_str());
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
        for (const std::string& word : this->words)
        {
            if (i != 0)
            {
                oss << " ";
            }
            oss << "`" << word << "`";
            i++;
        }

        i = 0;
        for (const redir& r : this->redirs)
        {
            if (i != 0)
            {
                oss << " ";
            }
            std::string redir_text;
            switch (r.type)
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
            oss << redir_text << " `" << r.word << "`";
            i++;
        }

        return oss.str();
    }
}
