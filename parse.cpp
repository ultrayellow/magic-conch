// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "parse.hpp"

#include "command.hpp"
#include "token.hpp"

#include <exception>
#include <memory>
#include <string>
#include <vector>

namespace magicconch
{
    parser::parser(std::vector<token>& toks)
        : it(toks.begin()), end(toks.end()) {}

    static void _assert_syntax(bool b)
    {
        if (!b)
        {
            throw syntax_error();
        }
    }

    static redir_type _get_redir_type(const token& tok)
    {
        if (tok.is_meta(">"))
        {
            return REDIR_WRITE;
        }
        else if (tok.is_meta("<"))
        {
            return REDIR_READ;
        }
        else if (tok.is_meta(">>"))
        {
            return REDIR_APPEND;
        }
        else if (tok.is_meta("<<"))
        {
            return REDIR_HEREDOC;
        }
        return REDIR_NONE;
    }

    void parser::next()
    {
        if (this->it == this->end)
        {
            throw end_of_token();
        }
        ++this->it;
    }

    bool parser::next_if_meta(const std::string& meta)
    {
        if (this->it != this->end && this->it->is_meta(meta))
        {
            this->next();
            return true;
        }
        return false;
    }

    bool parser::next_if_word()
    {
        if (this->it != this->end && this->it->is_word())
        {
            this->next();
            return true;
        }
        return false;
    }

    std::shared_ptr<command> parser::next_list()
    {
        std::shared_ptr<command> tree = this->next_pipeline();

        while (this->it != this->end && (this->it->is_meta("&&") || this->it->is_meta("||")))
        {
            token& tok = *this->it;
            this->next();
            std::shared_ptr<command> first = tree;
            std::shared_ptr<command> second = this->next_pipeline();
            tree = std::make_shared<command_connection>(tok.detach_word(), first, second);
        }

        _assert_syntax(this->it == this->end || this->it->is_meta(")"));

        return tree;
    }

    std::shared_ptr<command> parser::next_pipeline()
    {
        std::shared_ptr<command> tree = this->next_command();

        while (this->next_if_meta("|"))
        {
            std::shared_ptr<command> first = tree;
            std::shared_ptr<command> second = this->next_command();
            tree = std::make_shared<command_connection>("|", first, second);
        }

        return tree;
    }

    std::shared_ptr<command> parser::next_command()
    {
        if (this->next_if_meta("("))
        {
            auto subshell = std::make_shared<subshell_command>(this->next_list());
            _assert_syntax(this->next_if_meta(")"));

            // optional redirection list
            while (this->it != this->end)
            {
                const token& tok = *this->it;
                redir_type type = _get_redir_type(tok);
                if (type != REDIR_NONE)
                {
                    this->next();
                    if (this->it != this->end)
                    {
                        token& word = *this->it;
                        _assert_syntax(this->next_if_word());
                        subshell->add_redir(type, word.detach_word());
                        continue;
                    }
                }
                else
                {
                    break;
                }
            }
            return subshell;
        }
        else
        {
            return this->next_simple_command();
        }
    }

    std::shared_ptr<simple_command> parser::next_simple_command()
    {
        auto cmd = std::make_shared<simple_command>();

        while (this->it != this->end)
        {
            token& tok = *this->it;
            if (this->next_if_word())
            {
                cmd->add_word(tok.detach_word());
            }
            else
            {
                redir_type type = _get_redir_type(tok);
                if (type != REDIR_NONE)
                {
                    this->next();
                    if (this->it != this->end)
                    {
                        token& word = *this->it;
                        _assert_syntax(this->next_if_word());
                        cmd->add_redir(type, word.detach_word());
                        continue;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        _assert_syntax(!cmd->is_empty());

        return cmd;
    }

    std::shared_ptr<command> parser::do_parse()
    {
        return this->next_list();
    }
}
