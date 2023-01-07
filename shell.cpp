// ******************************* //
//            @COPYLEFT            //
//       ALL WRONGS RESERVED       //
// ******************************* //

#include "token.hpp"

#include <readline/readline.h>

#include <string>
#include <vector>

#include <iostream>

int main()
{
    for (;;)
    {
        char* str = readline("shell$ ");
        if (str)
        {
            try
            {
                std::vector<token> tokens = lex(str);
                for (const token& t : tokens)
                {
                    std::cout << (t.meta ? "META: " : "word: ") << t.str << std::endl;
                }
            }
            catch (const std::exception& ex)
            {
                std::cout << "Error! " << ex.what() << std::endl;
            }
            free(str);
        }
        else
        {
            std::cout << "!! Input is NULL !!" << std::endl;
            break;
        }
    }
    return 0;
}
