#include <gtest/gtest.h>
#include "block_handler.h"

class BlockHandlerTest : public ::testing::Test {
protected:
    std::string filename = "test_block_device.dat";
    BlockHandler handler;

    BlockHandlerTest() : handler(filename) {}

    virtual void SetUp() {
        // Создаем блочное устройство перед каждым тестом
        handler.create_block_device();
    }

    virtual void TearDown() {
        // Удаляем файл после каждого теста, чтобы не засорять файловую систему
        std::remove(filename.c_str());
    }
};

TEST_F(BlockHandlerTest, TestBlockNumber) {
    ASSERT_EQ(handler.test_get_block_number("test_hash_1"), handler.test_get_block_number("test_hash_1"));
    ASSERT_NE(handler.test_get_block_number("test_hash_1"), handler.test_get_block_number("test_hash_2"));
}

TEST_F(BlockHandlerTest, TestBlockData) {
    char buffer[1024];
    ASSERT_EQ(handler.test_get_block_data(0, buffer, 513), 0);
    ASSERT_EQ(buffer[0], 'A');
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
