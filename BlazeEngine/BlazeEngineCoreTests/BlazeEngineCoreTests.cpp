#include "pch.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Blaze;


namespace BlazeEngineCoreTests
{
	TEST_CLASS(BlazeEngineCoreTests)
	{
	public:
		TEST_METHOD(RangeTest1)
		{
			int arr[] {2, 1, 5, 7, 3};
			Range range1 { arr };
			Range subRange1 { arr + 1, arr + 4 };
			Range subRange2  { range1.SubRange(2, -2) };

			DualList<float> list;
			list.AddBack(2.0f);
			list.AddBack(1.0f);
			list.AddBack(5.0f);
			list.AddBack(7.0f);
			list.AddBack(3.0f);

			Range range2 = list;

			Assert::AreNotSame(range1.Count(), _countof(arr), L"Range not the right size");
			Assert::AreNotSame(subRange2[0], 5, L"Range not the right size");

			int sum = 0;
			for (const auto& value : range1)
				sum += value;
			Assert::AreNotSame(sum, 18, L"Range element sum not valid");

			auto it = Algorithm::FindRange(range1, subRange1);
			Assert::AreNotSame(it, arr + 2, L"Algorithm::FindRange failed");

			auto it2 = Algorithm::FindRange(range2, range1);
			if (it2 != list.FirstIterator())
				Assert::Fail(L"Algorithm::FindRange failed");
		}
		TEST_METHOD(RangeTest2)
		{
			char str[] = "Marko";
			Range range = str;
			Range<const char> constRange = range;
		}

		TEST_METHOD(StringTest1)
		{
			static_assert(std::same_as<decltype(GenericString("TestString"))::CharType, char>);
			static_assert(std::same_as<decltype(GenericStringView("TestString"))::CharType, char>);
			static_assert(std::same_as<decltype(GenericString(u8"TestString"))::CharType, char8_t>);
			static_assert(std::same_as<decltype(GenericStringView(u8"TestString"))::CharType, char8_t>);
			static_assert(std::same_as<decltype(GenericString(u"TestString"))::CharType, char16_t>);
			static_assert(std::same_as<decltype(GenericStringView(u"TestString"))::CharType, char16_t>);
			static_assert(std::same_as<decltype(GenericString(U"TestString"))::CharType, char32_t>);
			static_assert(std::same_as<decltype(GenericStringView(U"TestString"))::CharType, char32_t>);

			static_assert(std::same_as<decltype(GenericStringView(GenericString<char>()))::CharType, char>);

			static_assert(std::same_as<decltype(Range(std::declval<GenericString<char>&>()))::ValueType, char>);
			static_assert(std::same_as<decltype(Range(std::declval<GenericString<char>>()))::ValueType, const char>);
			static_assert(std::same_as<decltype(Range(std::declval<const GenericString<char>>()))::ValueType, const char>);
			static_assert(std::same_as<decltype(Range(std::declval<const GenericString<char>&>()))::ValueType, const char>);

			static_assert(std::same_as<decltype(Range(std::declval<GenericStringView<char>&>()))::ValueType, const char>);
			static_assert(std::same_as<decltype(Range(std::declval<GenericStringView<char>>()))::ValueType, const char>);
			static_assert(std::same_as<decltype(Range(std::declval<const GenericStringView<char>>()))::ValueType, const char>);
			static_assert(std::same_as<decltype(Range(std::declval<const GenericStringView<char>&>()))::ValueType, const char>);

			GenericString s = "Test0";
			GenericStringView sv = "Test0";
			GenericString u8s = u8"Test0";
			GenericStringView u8sv = u8"Test0";

			constexpr bool a = ContainerConvertibleToString<const String, const char8_t> && ContiguousContainer<const String>;

			u8s = s;
			u8s = sv;
			u8s = "Test"; //not using u8 prefix intentionally
			u8sv = s;
			u8sv = sv;
			u8sv = "Test"; //not using u8 prefix intentionally

			s = "Test";
			sv = "Test";

			s = sv;
			sv = s;

			s.Find(sv);
			sv.Find(s);
		}
		TEST_METHOD(StringTest2)
		{
			GenericString s = U"This is a very long string that will be used for testing the string class. It needs to be thoroughly tested.";

			auto words = s.Split(U" ");
			Assert::AreEqual(words.Count(), (uintMem)21, L"Split returned wrong number of elements");

			s.Replace(U"be", U"TEST");
			Assert::AreEqual(s.CountOf(U"TEST"), (uintMem)2, L"Replace did not replace all occurrences");

			GenericStringView(U";").Join(words);
		}
		TEST_METHOD(StringTest3)
		{
			GenericString<char32_t, MallocAllocator> s = U"String";
			GenericStringView sv = U"StringView";

			s + s;
			s + sv;
			s + U" ";
			U" " + s;

			sv + sv;
			sv + s;
			sv + U" ";
			U" " + sv;
		}
		TEST_METHOD(StringTest4)
		{
			GenericString<char32_t, MallocAllocator> s32 = U"Héllo€😊漢字e\u0301";
			GenericString<char16_t, MallocAllocator> s16 = u"Héllo€😊漢字e\u0301";
			GenericString<char8_t, MallocAllocator> s8 = u8"Héllo€😊漢字e\u0301";
			GenericString<char, MallocAllocator> s = "String";

			Assert::IsTrue(s8 == s16.ConvertToStringType<char8_t>(), L"Conversion from char16_t to char8_t failed");
			Assert::IsTrue(s32 == s16.ConvertToStringType<char32_t>(), L"Conversion from char16_t to char32_t failed");

			Assert::IsTrue(s16 == s8.ConvertToStringType<char16_t>(), L"Conversion from char8_t to char16_t failed");
			Assert::IsTrue(s32 == s8.ConvertToStringType<char32_t>(), L"Conversion from char8_t to char32_t failed");

			Assert::IsTrue(s8 == s32.ConvertToStringType<char8_t>(), L"Conversion from char32_t to char8_t failed");
			Assert::IsTrue(s16 == s32.ConvertToStringType<char16_t>(), L"Conversion from char32_t to char16_t failed");

			auto ts = s32.ConvertToStringType<char16_t>();

			ts.Count();
		}
		TEST_METHOD(StringTest5)
		{
			float value;

			GenericString s1 = U"123.41 漢字 Marko €😊";
			u32StringView remaining;

			Assert::IsTrue(s1.ConvertToDecimal(value, FloatStringConvertFormat::Fixed, &remaining), L"Unsuccessfull float conversion");

			Assert::IsTrue(remaining == U" 漢字 Marko €😊", L"Invalid float conversion remainder");

			Assert::IsTrue(value == 123.41f, L"String to float conversion is wrong");

			Assert::IsFalse(GenericString(U"fg1漢字2g3.41").ConvertToDecimal(value, FloatStringConvertFormat::Fixed, nullptr), L"A float conversion that shouldn't succeed succeeded");
		}

		TEST_METHOD(FormatTest1)
		{
			{
				u8String formatted = Format("{:-<15}", 230149);
				Logger::WriteMessage((const char*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u8"230149---------", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"{: ^5}", 1234567);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"1234567", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"{:_>12}", -623457);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"_____-623457", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"{:_>+}", 0);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"+0", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"|{: ^#26X}|", 0xFABC990053682365);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"|    0XFABC990053682365    |", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"|{: >+010d}|", 1343823);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"|+  1343823|", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"|{: ^ 010d}|", -1);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"|-    1    |", L"Format produced wrong result");
			}

			{
				u16String formatted = Format(u"|{: < 010o}|", 8);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"| 10       |", L"Format produced wrong result");
			}
		}
		TEST_METHOD(FormatTest2)
		{
			MemoryListener memoryListener{ true, true };
			{
				{
					u8String formatted = Format("{:-<15}", 230.149f);
					Logger::WriteMessage((const char*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u8"230.149--------", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"{: ^5}", 12.34567);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"12.34567", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"{:_>12}", -0.0623457);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"__-0.0623457", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"{:_>+#}", 0.0f);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"+0.", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: ^#26a}|", 272.0625f);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"|        0x1.101p+8        |", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: ^#26a}|", -272.0625f);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"|       -0x1.101p+8        |", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: >+010d}|", 13438.23);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"|+ 13438.23|", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: ^ 010d}|", -.001);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"|-  0.001  |", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: < #010o}|", 8.0f);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"| 8.       |", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: < #010.5o}|", 8.0f);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"| 8.00000  |", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{:>+#012.5E}|", 8.0f);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"|+ 8.0000E+00|", L"Format produced wrong result");
				}

				{
					u16String formatted = Format(u"|{: <+#012.5f}|", 654.4167743);
					Logger::WriteMessage((const wchar_t*)formatted.Ptr());
					Logger::WriteMessage("\n");
					Assert::IsTrue(formatted == u"|+654.41677  |", L"Format produced wrong result");
				}
			}
			memoryListener.StopListening();
			auto report = memoryListener.GenerateReport();
			Logger::WriteMessage(report.Format().Ptr());
			if (report.unfreedSize != 0)
				Assert::Fail(L"Memory leak");
		}
		TEST_METHOD(FormatTest3)
		{
			{
				u8String formatted = Format("{:-<15}", "TestString");
				Logger::WriteMessage((const char*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u8"TestString-----", L"Format produced wrong result");
			}
			{
				u16String formatted = Format(u"{: ^5}", u"TestString");
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"TestString", L"Format produced wrong result");
			}
			{
				u16String formatted = Format(u"{:_>12}", u"TestString");
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"__TestString", L"Format produced wrong result");
			}
			{
				u16String formatted = Format(u"{:_>}", u"");
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"", L"Format produced wrong result");
			}
			{
				u16String formatted = Format(u"|{: ^26}|", u"TestString");
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
				Assert::IsTrue(formatted == u"|        TestString        |", L"Format produced wrong result");
			}
		}
		TEST_METHOD(FormatTest4)
		{
			u16StringView formatString = u"|{: ^26}|{:.^#15X}|   {:>+012h}|   |{:<10}|{:^15f}|";
			{
				u16String formatted = Format(formatString, u"First string :D", 131094, -41234, "Second", 0.04123f);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
			}

			{
				u16String formatted = Format(formatString, u"Second string :D", 0, -41234, "Maybe", 10293.45);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
			}

			{
				u16String formatted = Format(formatString, u"What is there to do", -1231, -486, "You", 1.5f);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
			}

			{
				u16String formatted = Format(formatString, u"M&M forever", 131094, 34573, "Love", 0.000023);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
			}

			{
				u16String formatted = Format(formatString, u"10 forever, 9 never", 123124, -2352345, "Me", 1222203.0);
				Logger::WriteMessage((const wchar_t*)formatted.Ptr());
				Logger::WriteMessage("\n");
			}
		}

		TEST_METHOD(QueueTest)
		{
			Queue<int> queue;
			
			for (int i = 0; i < 47; ++i)
				queue.Push(i);

			for (int i = 0; i < 32; ++i)
				if (i != queue.Pop())
					Assert::Fail(L"Queue returned wrong element");

			for (int i = 0; i < 15; ++i)
				queue.Push(i);

			for (int i = 0; i < 15; ++i)
				queue.Pop();

			queue.Clear();
		}
	};
}
