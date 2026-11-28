#include <allegro5/allegro.h>
#include "fase.h"

int main(void)
{
    if (!al_init())
        return -1;

    final();

    return 0;
}