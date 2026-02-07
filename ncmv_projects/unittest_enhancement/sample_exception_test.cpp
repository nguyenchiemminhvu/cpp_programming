#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "src/sample_exception.cpp"

#include "mock/unittest_enhanced_mock.h"

class SampleExceptionTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Set up the mock object
        M_unittest_enhanced = new Mockunittest_enhanced();
    }

    void TearDown() override
    {
        // Clean up the mock object
        delete M_unittest_enhanced;
        M_unittest_enhanced = nullptr;
    }
};

TEST_F(SampleExceptionTest, Constructor)
{
    SampleException sample;
    // Check if the constructor initializes the member variables correctly
    ASSERT_EQ(sample.m_intPtr, nullptr);
    ASSERT_EQ(sample.m_intPtr2, nullptr);
    ASSERT_EQ(sample.m_intPtr3, nullptr);
}

TEST_F(SampleExceptionTest, Destructor)
{
    SampleException sample;
    // Check if the destructor is called correctly
    // This is usually checked by ensuring no memory leaks occur
    // or by using a memory leak detection tool.
}

TEST_F(SampleExceptionTest, InitComponents)
{
    EXPECT_CALL(*M_unittest_enhanced, mock_checkpoint_void())
        .Times(3)
        .WillOnce(testing::Return())
        .WillOnce(testing::Return())
        .WillOnce(testing::Return());

    SampleException sample;
    sample.initComponents();

    // Check if the components are initialized correctly
    ASSERT_NE(sample.m_intPtr, nullptr);
    ASSERT_NE(sample.m_intPtr2, nullptr);
    ASSERT_NE(sample.m_intPtr3, nullptr);
}

TEST_F(SampleExceptionTest, InitComponentsWithBadAlloc)
{
    EXPECT_CALL(*M_unittest_enhanced, mock_checkpoint_void())
        .Times(1)
        .WillOnce(testing::Throw(std::bad_alloc()));

    SampleException sample;
    sample.initComponents();

    // Check if the components are initialized correctly
    ASSERT_EQ(sample.m_intPtr, nullptr);
    ASSERT_EQ(sample.m_intPtr2, nullptr);
    ASSERT_EQ(sample.m_intPtr3, nullptr);
}

TEST_F(SampleExceptionTest, InitComponentsWithBadAlloc2)
{
    EXPECT_CALL(*M_unittest_enhanced, mock_checkpoint_void())
        .Times(2)
        .WillOnce(testing::Return())
        .WillOnce(testing::Throw(std::bad_alloc()));

    SampleException sample;
    sample.initComponents();

    // Check if the components are initialized correctly
    ASSERT_EQ(sample.m_intPtr, nullptr);
    ASSERT_EQ(sample.m_intPtr2, nullptr);
    ASSERT_EQ(sample.m_intPtr3, nullptr);
}

TEST_F(SampleExceptionTest, InitComponentsWithBadAlloc3)
{
    EXPECT_CALL(*M_unittest_enhanced, mock_checkpoint_void())
        .Times(3)
        .WillOnce(testing::Return())
        .WillOnce(testing::Return())
        .WillOnce(testing::Throw(std::bad_alloc()));

    SampleException sample;
    sample.initComponents();

    // Check if the components are initialized correctly
    ASSERT_EQ(sample.m_intPtr, nullptr);
    ASSERT_EQ(sample.m_intPtr2, nullptr);
    ASSERT_EQ(sample.m_intPtr3, nullptr);
}
