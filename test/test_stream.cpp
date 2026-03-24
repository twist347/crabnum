#include <gtest/gtest.h>

#include <sstream>

#include "crabnum.h"

TEST(Stream, OutputInt) {
    std::ostringstream os;
    os << cn::i32{42};
    EXPECT_EQ(os.str(), "42");
}

TEST(Stream, OutputNegative) {
    std::ostringstream os;
    os << cn::i32{-7};
    EXPECT_EQ(os.str(), "-7");
}

TEST(Stream, OutputFloat) {
    std::ostringstream os;
    os << cn::f64{2.5};
    EXPECT_FALSE(os.str().empty());
}

TEST(Stream, InputInt) {
    std::istringstream is{"123"};
    cn::i32 x;
    is >> x;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(x, cn::i32{123});
}

TEST(Stream, InputFloat) {
    std::istringstream is{"3.14"};
    cn::f64 x;
    is >> x;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(x.value(), double{3.14});
}

TEST(Stream, InputInvalid) {
    std::istringstream is{"abc"};
    cn::i32 x;
    is >> x;
    EXPECT_TRUE(is.fail());
}

TEST(Stream, InputMultiple) {
    std::istringstream is{"10 20"};
    cn::i32 a, b;
    is >> a >> b;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(a, cn::i32{10});
    EXPECT_EQ(b, cn::i32{20});
}
