// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "command.hpp"
#include "parse.hpp"
#include "simple_shared_ptr.hpp"
#include "token.hpp"

#include <readline/readline.h>

#include <iostream>
#include <string>
#include <vector>

int main()
{
    for (;;)
    {
        char* str = readline("shell$ ");
        if (str)
        {
            try
            {
                std::vector<microshellxx::token> toks = microshellxx::lex(str);
                uy::shared_ptr<microshellxx::command> cmd = microshellxx::parser(toks).do_parse();
                std::cout << "Result: " << cmd->to_string() << std::endl;
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
    return 0;
}
