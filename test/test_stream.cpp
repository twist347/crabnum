#include <gtest/gtest.h>

#include <sstream>
#include <iomanip>

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

// --- output honours the stream's formatting state ---

TEST(Stream, OutputRespectsBase) {
    std::ostringstream os;
    os << std::hex << cn::u32{255};
    EXPECT_EQ(os.str(), "ff");
}

TEST(Stream, OutputRespectsWidthAndFill) {
    std::ostringstream os;
    os << std::setw(6) << std::setfill('0') << cn::i32{42};
    EXPECT_EQ(os.str(), "000042");
}

TEST(Stream, OutputRespectsPrecision) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(3) << cn::f64{2.5};
    EXPECT_EQ(os.str(), "2.500");
}

TEST(Stream, OutputRespectsShowpos) {
    std::ostringstream os;
    os << std::showpos << cn::i32{7};
    EXPECT_EQ(os.str(), "+7");
}

// char-sized integers must print as numbers, not as characters
TEST(Stream, OutputByteSizedIntsAreNumeric) {
    std::ostringstream os;
    os << cn::i8{65} << ' ' << cn::u8{200} << ' ' << cn::i8{-1};
    EXPECT_EQ(os.str(), "65 200 -1");
}

// --- input stops at the end of the number ---

TEST(Stream, InputStopsAtFirstNonNumericChar) {
    std::istringstream is{"12abc"};
    cn::i32 x;
    is >> x;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(x, cn::i32{12});

    std::string rest;
    is >> rest;
    EXPECT_EQ(rest, "abc");
}

TEST(Stream, InputFloatStopsAtFirstNonNumericChar) {
    std::istringstream is{"3.5xyz"};
    cn::f64 x;
    is >> x;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(x.value(), 3.5);

    std::string rest;
    is >> rest;
    EXPECT_EQ(rest, "xyz");
}

TEST(Stream, InputFloatAcceptsInf) {
    std::istringstream is{"inf"};
    cn::f64 x;
    is >> x;
    EXPECT_FALSE(is.fail());
    EXPECT_TRUE(x.is_inf());
}

// a stream that is already in a failed state must not be touched
TEST(Stream, InputOnFailedStreamIsNoOp) {
    std::istringstream is{"123"};
    is.setstate(std::ios::failbit);
    cn::i32 x{7};
    is >> x;
    EXPECT_TRUE(is.fail());
    EXPECT_EQ(x, cn::i32{7});
}

TEST(Stream, InputRoundTrip) {
    std::ostringstream os;
    os << cn::i32{-12345};
    std::istringstream is{os.str()};
    cn::i32 x;
    is >> x;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(x, cn::i32{-12345});
}
