// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "command.hpp"
#include "parse.hpp"
#include "token.hpp"

#include <readline/readline.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main()
{
    int status = EXIT_SUCCESS;
    for (;;)
    {
        char* str = readline("shell$ ");
        if (str)
        {
            try
            {
                std::vector<magicconch::token> toks = magicconch::lex(str);
                std::shared_ptr<magicconch::command> cmd = magicconch::parser(toks).do_parse();
                std::cout << "Result: " << cmd->to_string() << std::endl;
                status = cmd->execute(NO_PIPE, NO_PIPE);
            }
            catch (const std::exception& ex)
            {
                std::cout << "Error! " << ex.what() << std::endl;
            }
            free(str);
        }
        else
        {
            std::cout << "exit" << std::endl;
            break;
        }
    }
    return status;
}
