// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "parse.hpp"

#include "command.hpp"
#include "token.hpp"
#include <uy_shared_ptr.hpp>

#include <exception>
#include <string>
#include <vector>

namespace microshellxx
{
    parser::parser(const std::vector<token>& toks)
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

    uy::shared_ptr<command> parser::next_list()
    {
        uy::shared_ptr<command> tree = next_pipeline();

        while (this->it != this->end && (this->it->is_meta("&&") || this->it->is_meta("||")))
        {
            const token& tok = *this->it;
            this->next();
            uy::shared_ptr<command> first = tree;
            uy::shared_ptr<command> second = next_pipeline();
            tree = uy::make_shared<command_connection>(tok.get(), first, second);
        }

        _assert_syntax(this->it == this->end || this->it->is_meta(")"));

        return tree;
    }

    uy::shared_ptr<command> parser::next_pipeline()
    {
        uy::shared_ptr<command> tree = next_command();

        while (this->next_if_meta("|"))
        {
            uy::shared_ptr<command> first = tree;
            uy::shared_ptr<command> second = next_command();
            tree = uy::make_shared<command_connection>("|", first, second);
        }

        return tree;
    }

    uy::shared_ptr<command> parser::next_command()
    {
        if (this->next_if_meta("("))
        {
            uy::shared_ptr<subshell_command> subshell = uy::make_shared<subshell_command>(this->next_list());
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
                        const token& word = *this->it;
                        _assert_syntax(this->next_if_word());
                        subshell->add_redir(redir(type, word.get()));
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

    uy::shared_ptr<simple_command> parser::next_simple_command()
    {
        uy::shared_ptr<simple_command> cmd = uy::make_shared<simple_command>();

        while (this->it != this->end)
        {
            const token& tok = *this->it;
            if (this->next_if_word())
            {
                cmd->add_word(tok.get());
            }
            else
            {
                redir_type type = _get_redir_type(tok);
                if (type != REDIR_NONE)
                {
                    this->next();
                    if (this->it != this->end)
                    {
                        const token& word = *this->it;
                        _assert_syntax(this->next_if_word());
                        cmd->add_redir(redir(type, word.get()));
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

    uy::shared_ptr<command> parser::do_parse()
    {
        return this->next_list();
    }
}
