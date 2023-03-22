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

#include <iostream> // temporary

namespace magicconch
{
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

        for (const redir& r : this->redirs)
        {
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
            oss << " " << redir_text << " `" << r.word << "`";
        }

        return oss.str();
    }
}
