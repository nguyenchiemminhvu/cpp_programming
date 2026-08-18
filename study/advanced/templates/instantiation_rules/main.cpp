#include <iostream>
#include "diagnostic.h"

// place of explicit instantiation of the diagnostic class template for engine1 and engine2
template class diagnostic<engine1>;
template class diagnostic<engine2>;

int main()
{
    diagnostic<engine1> diag1;
    diag1.run_diagnostic();

    diagnostic<engine2> diag2;
    diag2.run_diagnostic();

    return 0;
}