/*
 * Invoca la llamada al sistema getpid().
 */
#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char *argv[])
{
    // Descomentar una vez implementada la llamada al sistema.
    printf(1, "%d\n", getpid());
    exit();
}