#include <gtest/gtest.h>
#include "construct.h"
#include <string>

class TestObject {
public:
    TestObject() : value_(0) {}
    explicit TestObject(int v) : value_(v) {}
    TestObject(const TestObject& other) : value_(other.value_) {}
    ~TestObject() { value_ = -1; }
    
    int getValue() const { return value_; }
private:
    int value_;
};

TEST(ConstructTest, DefaultConstruct) {
    alignas(TestObject) char space[sizeof(TestObject)];
    TestObject* ptr = reinterpret_cast<TestObject*>(&space);
    
    mystl::construct(ptr);
    EXPECT_EQ(ptr->getValue(), 0);
    
    mystl::destroy(ptr);
    // Note: We can't really test the value after destruction as it would be undefined behavior
}

TEST(ConstructTest, ValueConstruct) {
    alignas(TestObject) char space[sizeof(TestObject)];
    TestObject* ptr = reinterpret_cast<TestObject*>(&space);
    
    mystl::construct(ptr, 42);
    EXPECT_EQ(ptr->getValue(), 42);
    
    mystl::destroy(ptr);
}

TEST(ConstructTest, CopyConstruct) {
    TestObject original(42);
    alignas(TestObject) char space[sizeof(TestObject)];
    TestObject* ptr = reinterpret_cast<TestObject*>(&space);
    
    mystl::construct(ptr, original);
    EXPECT_EQ(ptr->getValue(), 42);
    
    mystl::destroy(ptr);
}

TEST(ConstructTest, RangeDestroy) {
    const int size = 5;
    TestObject* objects = new TestObject[size];
    for (int i = 0; i < size; ++i) {
        mystl::construct(&objects[i], i);
    }
    
    // Verify construction
    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(objects[i].getValue(), i);
    }
    
    // Test range destroy
    mystl::destroy(objects, objects + size);
    delete[] objects;
} 