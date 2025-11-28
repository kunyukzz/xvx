#include "app.h"

int main(void)
{
    application_t app;
    application_init(&app);
    application_run(&app);
    return 0;
}
