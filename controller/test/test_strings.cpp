#include <unity.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_true_is_true()
{
    bool a = true;

    TEST_ASSERT_TRUE(a);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_true_is_true);

    UNITY_END();
}
