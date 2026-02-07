#include "unittest_enhanced.h"

#include <iostream>
#include <memory>

class SampleException : public unittest_enhanced
{
public:
    SampleException();
    virtual ~SampleException();

    void initComponents();

PRIVATE:
    std::shared_ptr<int> m_intPtr;
    std::shared_ptr<int> m_intPtr2;
    std::shared_ptr<int> m_intPtr3;
};

SampleException::SampleException()
    : m_intPtr(nullptr)
    , m_intPtr2(nullptr)
    , m_intPtr3(nullptr)
{
}

SampleException::~SampleException()
{
}

void SampleException::initComponents()
{
    bool initialized = true;
    try
    {
        m_intPtr = std::make_shared<int>(42);
#ifdef __UNITTEST__
        mock_checkpoint_void();
#endif // __UNITTEST__

        m_intPtr2 = std::make_shared<int>(100);
#ifdef __UNITTEST__
        mock_checkpoint_void();
#endif // __UNITTEST__

        m_intPtr3 = std::make_shared<int>(200);
#ifdef __UNITTEST__
        mock_checkpoint_void();
#endif // __UNITTEST__
    }
    catch (const std::bad_alloc& e)
    {
        m_intPtr.reset();
        m_intPtr2.reset();
        m_intPtr3.reset();
        initialized = false;
    }

    if (!initialized)
    {
        // std::cerr << "Failed to initialize components" << std::endl;
    }
}