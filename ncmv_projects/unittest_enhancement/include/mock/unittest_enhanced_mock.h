class Mockunittest_enhanced : public unittest_enhanced {
    public:
        MOCK_METHOD0(mock_checkpoint_void, void());
        MOCK_METHOD0(mock_checkpoint_bool, bool());
        MOCK_METHOD0(mock_checkpoint_int, int32_t());
        MOCK_METHOD0(mock_checkpoint_uint, uint32_t());
        MOCK_METHOD0(mock_checkpoint_double, double());
        MOCK_METHOD0(mock_checkpoint_string, std::string());
};

Mockunittest_enhanced *M_unittest_enhanced;

unittest_enhanced::unittest_enhanced()
{
}


unittest_enhanced::~unittest_enhanced() {
}

void unittest_enhanced::mock_checkpoint_void() {
    M_unittest_enhanced->mock_checkpoint_void();
}

bool unittest_enhanced::mock_checkpoint_bool() {
    return M_unittest_enhanced->mock_checkpoint_bool();
}

int32_t unittest_enhanced::mock_checkpoint_int() {
    return M_unittest_enhanced->mock_checkpoint_int();
}

uint32_t unittest_enhanced::mock_checkpoint_uint() {
    return M_unittest_enhanced->mock_checkpoint_uint();
}

double unittest_enhanced::mock_checkpoint_double() {
    return M_unittest_enhanced->mock_checkpoint_double();
}

std::string unittest_enhanced::mock_checkpoint_string() {
    return M_unittest_enhanced->mock_checkpoint_string();
}


