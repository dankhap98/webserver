#include "../includes/Webserver.hpp"

int     main()
{
    Webserver wb(8080);

    wb.start();
}