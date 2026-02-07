#ifndef PERFORMANCETEST_H
#define PERFORMANCETEST_H

#define N_TIME 1000000

typedef void (*callback_function)(void);

class performance_test
{
public:
    static void run(int n_time, callback_function func, const char* func_name = nullptr);
};

#endif // PERFORMANCETEST_H
