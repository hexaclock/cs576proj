#include "keylocker.h"

/* pre: takes in a std::string question and std::string ans_default which MUST
 *      be either 'yes' or 'no'
 * post: prompts the user with question followed by a (yes/no) string which will
 *      be altered to (YES/no) if ans_default is yes and (yes/NO) if ans_default
 *      is no, otherwise ans_default should be ""
 * return: true if the user responds yes and false if the user responds no
 */
bool prompt_y_n(std::string question, std::string ans_default)
{
    std::string res;
    std::string yn;
    bool default_action;

    if (ans_default == "yes")
    {
        yn = " (YES/no) ";
        default_action = true;
    }
    else if (ans_default == "no")
    {
        yn = " (yes/NO) ";
        default_action = false;
    }
    else if (ans_default.empty())
    {
        yn = " (yes/no) ";
        default_action = false;
    }
    else
        std::cout << "Provided incorrect parameters to function `prompt_y_n`!"
                  << std::endl;

    //prompt user
    std::cout << question + yn;

    getline(std::cin, res);
    std::cin.sync();
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);

    do
    {
        // this allows the user to shortcut by just hitting enter when there is
        // a default action defined
        if (res.empty() && !ans_default.empty())
        {
            return default_action;
        }
        else if (res == "yes" || res == "y")
            return true;
        else if (res == "no" || res == "n")
            return false;
        else
            std::cout << "Please type 'yes' or 'no': ";

        getline(std::cin, res);
        std::cin.sync();
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    } while (1);
}
