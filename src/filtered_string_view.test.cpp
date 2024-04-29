#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <set>
#include <sstream>

TEST_CASE("static member test") {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		REQUIRE(fsv::filtered_string_view::default_predicate(c));
	}
}

TEST_CASE("constructor") {

	SECTION("default constructor") {
		fsv::filtered_string_view fsv;
		REQUIRE(fsv.size() == 0);
	}

	SECTION("implicit string constructor") {
		auto s_ = std::string{"Cat"};
		auto sv_ = fsv::filtered_string_view{s_};
		REQUIRE(sv_.size() == 3);
	}

	SECTION("string predicate constructor") {
		auto s_ = std::string{"Cat"};
		auto pred_ = [](const char &c) { return c == 't'; };
		auto sv_ = fsv::filtered_string_view{s_, pred_};
		REQUIRE(sv_.size() == 1);
	}

	SECTION("Null-terminated string constructor") {
		auto sv_ = fsv::filtered_string_view{"Cat"};
		REQUIRE(sv_.size() == 3);
	}

	SECTION("Null-terminated string predicate constructor") {
		auto pred_ = [](const char &c) { return c == 't'; };
		auto sv_ = fsv::filtered_string_view{"Cat", pred_};
		REQUIRE(sv_.size() == 1);
	}

	SECTION("copy constructor") {
		auto sv_ = fsv::filtered_string_view{"Cat"};
		auto copy = sv_;
		REQUIRE(copy.size() == 3);
	}

	SECTION("move constructor") {
		auto sv_ = fsv::filtered_string_view{"Cat"};
		auto move = std::move(sv_);
		REQUIRE(move.size() == 3);
		REQUIRE(sv_.data() == nullptr);
	}
}

TEST_CASE("Member Operators"){

	SECTION("copy assignment"){
		auto sv_ = fsv::filtered_string_view{"Cat Ragdoll", 
			[](const char &c) { return c == 'a' || c == 'o'|| c == 'e'; }};
		auto copy = fsv::filtered_string_view{};
		copy = sv_;
		REQUIRE(copy == sv_);
	}

	SECTION("move assignment"){
		auto sv_ = fsv::filtered_string_view{"Cat Ragdoll", 
			[](const char &c) { return c == 'a' || c == 'o'|| c == 'e'; }};
		auto move = fsv::filtered_string_view{};
		move = std::move(sv_);
		
		REQUIRE(sv_.size() == 0);
		REQUIRE(sv_.data() == nullptr);
	}

	SECTION("subscript operator"){
		auto sv_ = fsv::filtered_string_view{"The best breed of cats is Ragdoll", 
			[](const char &c) { return c == 'a' || c == 'o'|| c == 'e'; }};
		REQUIRE(sv_[0] == 'e');
		REQUIRE(sv_[1] == 'e');
		REQUIRE(sv_[2] == 'e');
		REQUIRE(sv_[3] == 'e');
		REQUIRE(sv_[4] == 'o');
		REQUIRE(sv_[5] == 'a');
	}

	SECTION("String Type Conversion"){
		auto sv_ = fsv::filtered_string_view{"Ragdoll"};
		auto s_ = static_cast<std::string>(sv_);
		REQUIRE_FALSE(sv_.data() == s_.data());
	}

}

TEST_CASE("Member Functions"){
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char &c){ return vowels.contains(c); };
	auto sv = fsv::filtered_string_view{"The best breed of cats is Ragdoll", is_vowel};
	
	SECTION("at"){
		// at
		REQUIRE(sv.at(0) == 'e');
		REQUIRE(sv.at(1) == 'e');
		REQUIRE(sv.at(2) == 'e');
		REQUIRE(sv.at(3) == 'e');
		REQUIRE(sv.at(4) == 'o');
		REQUIRE(sv.at(5) == 'a');
	}
	
	SECTION("size"){
		// size
		REQUIRE(sv.size() == 9);
	}

	SECTION("empty"){
		// empty
		REQUIRE_FALSE(sv.empty());
		auto empty_sv = fsv::filtered_string_view{};
		REQUIRE(empty_sv.empty());
	}

	SECTION("data"){
		// data
		REQUIRE(sv.data() != nullptr);
		auto s = "Cat: Ragdoll";
		auto sv1 = fsv::filtered_string_view{s, []([[maybe_unused]]const char &c){ return false; }};
		std::size_t i=0;
		for (auto ptr = sv1.data(); *ptr; ++ptr) {
			REQUIRE(*ptr==s[i]);
			i++;
		}
	}
	
	SECTION("predaicate()"){
		// predicate()
		const auto print_and_return_true = [](const char & x) {
			if (x == ' ') return false;
			return true;
		};
		const auto sx = fsv::filtered_string_view{"Ragdoll Cat", print_and_return_true};

		const auto& predicate = sx.predicate();
		REQUIRE(predicate('R'));
		REQUIRE(predicate('a'));
		REQUIRE_FALSE(predicate(' '));
	}

	SECTION("ostream"){
		// ostream
		auto sv1 = fsv::filtered_string_view{"Ragdoll Cat"};
		std::ostringstream os;
		os << sv1;
		REQUIRE(os.str() == "Ragdoll Cat");
	}
	
}

TEST_CASE("Non-Member Operators"){
	
	SECTION("equality"){
		// equality
		auto sv1 = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv2 = fsv::filtered_string_view{"Ragdoll Cat"};
		REQUIRE(sv1 == sv2);
	}

	SECTION("inequality"){
		// inequality
		auto sv3 = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv4 = fsv::filtered_string_view{"Ragdoll"};
		REQUIRE(sv3 != sv4);
	}

	SECTION("less than"){
		// less than
		auto sv5 = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv6 = fsv::filtered_string_view{"Ragdoll"};
		REQUIRE(sv6 > sv5);
		REQUIRE(sv6 >= sv5);
		REQUIRE_FALSE(sv6 < sv5);
	}
	
	SECTION("less than or equal to"){
		// less than or equal to
		auto sv7 = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv8 = fsv::filtered_string_view{"Ragdoll Cat"};
		REQUIRE(sv7 <= sv8);
	}

	SECTION("greater than"){
		// greater than
		auto sv9 = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv10 = fsv::filtered_string_view{"Ragdoll"};
		REQUIRE(sv9 < sv10);
		REQUIRE(sv9 <= sv10);
		REQUIRE_FALSE(sv9 > sv10);
	}

	SECTION("greater than or equal to"){
		// greater than or equal to
		auto sv11 = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv12 = fsv::filtered_string_view{"Ragdoll Cat"};
		REQUIRE(sv11 >= sv12);
	}

}

TEST_CASE("Non-Member Utility Functions"){
	SECTION("compose"){
		// compose
		using filter = std::function<bool(const char &)>;
		auto best_languages = fsv::filtered_string_view{"java!=c++"};
		auto vf = std::vector<filter>{
			[](const char &c){ return c != '!'; },
			[](const char &c){ return c > ' '; },
			[](const char &){ return true; }
		};

		auto sv = fsv::compose(best_languages, vf);
		REQUIRE(sv.size() == 8);
		REQUIRE(sv[0] == 'j');
		REQUIRE(sv[1] == 'a');
		REQUIRE(sv[2] == 'v');
		REQUIRE(sv[3] == 'a');
		REQUIRE(sv[4] == '=');
		REQUIRE(sv[5] == 'c');
		REQUIRE(sv[6] == '+');
		REQUIRE(sv[7] == '+');
	}

	SECTION("split"){
		// split
		auto sv_t = fsv::filtered_string_view{"The best breed of cats is Ragdoll"};
		auto tok_t = fsv::filtered_string_view{" "};
		auto v_t = fsv::split(sv_t, tok_t);
		auto expected_t = std::vector<fsv::filtered_string_view>{"The", "best", "breed", "of", "cats", "is", "Ragdoll"};
		REQUIRE(v_t == expected_t);

		auto sv_s = fsv::filtered_string_view{"0x00"};
		auto tok  = fsv::filtered_string_view{"0"};
		auto v = fsv::split(sv_s, tok);
		auto expected = std::vector<fsv::filtered_string_view>{"", "x", "", ""};
		REQUIRE(v == expected);

		auto sv_s2 = fsv::filtered_string_view{"00"};
		auto v2 = fsv::split(sv_s2, tok);
		auto expected2 = std::vector<fsv::filtered_string_view>{"", "", ""};
		REQUIRE(v2 == expected2);
	}

	SECTION("substr"){
		// substr
		auto sv_sub = fsv::filtered_string_view{"Ragdoll Cat"};
		auto sv_1 = fsv::substr(sv_sub, 8);
		REQUIRE(sv_1.size() == 3);
		REQUIRE(sv_1[0] == 'C');
		REQUIRE(sv_1[1] == 'a');
		REQUIRE(sv_1[2] == 't');

		auto is_upper = [](const char &c) { return std::isupper(static_cast<unsigned char>(c));};
		auto sv_2 = fsv::filtered_string_view{"Ragdoll Cat", is_upper};
		REQUIRE(sv_2.size() == 2);
		REQUIRE(sv_2[0] == 'R');
		REQUIRE(sv_2[1] == 'C');
	}

}

TEST_CASE("iterator && range") {

	SECTION("cbegin && cend && begin && end"){
		auto sv_ = fsv::filtered_string_view{"Ragdoll Cat"};
		auto it = sv_.cbegin();
		auto it2 = sv_.cend();
		REQUIRE(*it == 'R');
		REQUIRE(*--it2 == 't');

		auto sv = fsv::filtered_string_view{"Ragdoll"};
		char cc[8] = {'\0'};
		std::string expected = "Ragdoll";
		std::copy(sv.begin(), sv.end(), cc);
		REQUIRE(strcmp(cc,expected.c_str()) == 0);

		const auto s = fsv::filtered_string_view{"Ragdoll", 
				[](const char &c){ return !(c == 'a' || c == 'o'); }};
		auto v = std::vector<char>{s.begin(), s.end()};
		auto expected_v = std::vector<char> {'R', 'g', 'd', 'l', 'l'};
		REQUIRE(v.size() == 5);
		REQUIRE(v == expected_v);
	}
	
	SECTION("crbegin && crend && rbegin && rend"){
		const auto s = fsv::filtered_string_view{"Ragdoll", 
					[](const char &c){ return !(c == 'a' || c == 'o'); }};

		auto v_r = std::vector<char>{s.rbegin(), s.rend()};
		auto expected_v_r = std::vector<char> {'l', 'l', 'd', 'g', 'R'};
		REQUIRE(v_r.size() == 5);
		REQUIRE(v_r == expected_v_r);

		auto v_r_c = std::vector<char>{s.crbegin(), s.crend()};
		auto expected_v_r_c = std::vector<char> {'l', 'l', 'd', 'g', 'R'};
		REQUIRE(v_r_c.size() == 5);
		REQUIRE(v_r_c == expected_v_r);
	}

	SECTION("operator between begin && end"){
		auto sv2 = fsv::filtered_string_view{"Ragdoll Cat",
	        [](const char c){ return c == 'a' || c == 'o'; }};
		auto it = sv2.begin();
		auto it_e = sv2.end();
		// operator
		REQUIRE(*it == 'a');
		REQUIRE(*std::next(it) == 'o');
		REQUIRE(*std::next(it, 2) == 'a');
		REQUIRE(*++it == 'o');
		REQUIRE(*--it == 'a');
		REQUIRE(*it++ == 'a');
		REQUIRE(*it++ == 'o');
		// it -> 'a'(second)
		auto it_c = sv2.crbegin();
		REQUIRE(*it_c == *it);
		++it;
		REQUIRE(it == it_e);
	}

}