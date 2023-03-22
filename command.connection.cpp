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
    static int _execute_pipeline(const command_connection& pipeline, int pipe_in, int pipe_out)
    {
        std::vector<std::shared_ptr<command>> cmdv;
        cmdv.emplace_back(pipeline.get_second());
        std::shared_ptr<command> child = pipeline.get_first();
        for (;;)
        {
            auto child_tree = std::dynamic_pointer_cast<command_connection>(child);
            if (child_tree)
            {
                cmdv.emplace_back(child_tree->get_second());
                child = child_tree->get_first();
            }
            else
            {
                cmdv.emplace_back(child);
                break;
            }
        }
        std::reverse(cmdv.begin(), cmdv.end());

        int prev = pipe_in;
        int fildes[2];
        int status = EXIT_SUCCESS;
        for (auto it = cmdv.cbegin(); it != cmdv.cend(); ++it)
        {
            auto&& cmd = *it;
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
            for (const std::shared_ptr<command>& cmd : cmdv)
            {
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
