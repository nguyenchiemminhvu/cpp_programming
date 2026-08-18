#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

class engine1
{
};

class engine2
{
};

template <typename T>
class diagnostic
{
public:
    void run_diagnostic()
    {
        // Perform diagnostic on the engine
        // This is a placeholder implementation
        // You can add specific diagnostic logic based on the engine type T
    }
};

// tell compiler that the diagnostic class template will be explicitly instantiated for engine1 and engine2 somewhere else (in a .cpp file), so it doesn't need to instantiate it here
extern template class diagnostic<engine1>;
extern template class diagnostic<engine2>;

#endif // DIAGNOSTIC_H