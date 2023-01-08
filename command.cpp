// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "command.hpp"

#include <sstream>

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

    std::string subshell_command::to_string() const
    {
        std::ostringstream oss;

        oss << "( " << this->container->to_string() << " )";

        std::size_t i = 0;
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

    std::string command_connection::to_string() const
    {
        std::ostringstream oss;

        oss << this->first->to_string() << " " << this->connector << " " << this->second->to_string();

        return oss.str();
    }
}
