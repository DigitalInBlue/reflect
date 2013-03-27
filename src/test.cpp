#include <../../../Reflect/include/Reflect.h>
#include <gtest/gtest.h>
#include <functional>

// --gtest_catch_exceptions=0

TEST(Reflect, test_uint8_t)
{
	uint8_t uint8_tTest = 123;
	Reflect r(&uint8_tTest);

	// Easily convert to std::string or uint8_t
	EXPECT_EQ(std::to_string(uint8_tTest), static_cast<std::string>(r));
	EXPECT_EQ(uint8_tTest, static_cast<uint8_t>(r));

	// Treat 'r' as a uint8_t.
	uint8_t uint8_tTest2 = static_cast<uint8_t>(234);
	r = uint8_tTest2;

	EXPECT_EQ(std::to_string(uint8_tTest2), static_cast<std::string>(r));
	EXPECT_EQ(uint8_tTest2, static_cast<uint8_t>(r));

	// the values in uint8_tTest were changed when we changed 'r'
	EXPECT_EQ(uint8_tTest2, uint8_tTest);

	// The values in 'r' will change when we change the original uint8_t's value.
	uint8_tTest = 111;
	EXPECT_EQ(std::to_string(uint8_tTest), static_cast<std::string>(r));
	EXPECT_EQ(uint8_tTest, static_cast<uint8_t>(r));
}

TEST(Reflect, test_string)
{
	std::string stringTest = "value";
	Reflect r(&stringTest);

	EXPECT_EQ(stringTest, static_cast<std::string>(r));
	EXPECT_EQ(stringTest, (std::string)(r));

	std::string stringTest2 = "valueChanged";
	r = stringTest2;

	EXPECT_EQ(stringTest2, static_cast<std::string>(r));
	EXPECT_EQ(stringTest2, (std::string)(r));
}

TEST(Reflect, test_stream)
{
	std::string stringTest = "value";
	Reflect rString(&stringTest);

	float floatTest = 123.456789f;
	Reflect rFloat(&floatTest);

	std::stringstream ss;
	ss << rString << " = " << rFloat;

	EXPECT_EQ("value = 123.456787", ss.str());
}

TEST(Reflect, test_equality)
{
	std::string stringTest = "value";
	Reflect rString(&stringTest);

	EXPECT_TRUE(rString == "value");
	EXPECT_TRUE(rString == std::string("value"));
}

TEST(Reflect, test_scope)
{
	std::function<Reflect()> outOfScope = 
		[]()->Reflect
		{
			std::string stringTest = "value";
			Reflect rString(&stringTest);
			return rString;
		};

	auto invalidResult = outOfScope();

	// We should get a nice exception here accessing some out of scope reflection information.
	EXPECT_ANY_THROW(invalidResult == std::string("value"));
}

TEST(Reflect, test_typeID)
{
	uint8_t uint8_tTest = 123;
	Reflect r(&uint8_tTest);

	EXPECT_EQ(r.type(), typeid(uint8_t));
	EXPECT_NE(r.type(), typeid(uint16_t));
}

TEST(Reflect, test_lessThan)
{
	uint8_t small = 1;
	Reflect rSmall(&small);

	uint8_t big = 100;
	Reflect rBig(&big);

	EXPECT_LT(small, big);
	EXPECT_LT(rSmall, rBig);
}
