#include "./filtered_string_view.h"

// Implement here

namespace fsv{
	filter filtered_string_view::default_predicate{[](const char &)->bool { return true; }};
	// default constructors
	filtered_string_view::filtered_string_view() noexcept= default;
	// implicit string constructor
	filtered_string_view::filtered_string_view(const std::string &str) noexcept : ptr_{str.data()}, len_{str.size()} {}
	// predicate constructor
	filtered_string_view::filtered_string_view(const std::string &str, filter predicate) noexcept :
	    ptr_{str.data()}, len_{str.size()}, predicate_func_{predicate}{}
	// implicit Null-terminated string constructor
	filtered_string_view::filtered_string_view(const char *str) noexcept : ptr_{str}, len_{std::strlen(str)} {}
	// Null-terminated constructor
	filtered_string_view::filtered_string_view(const char *str, filter predicate) noexcept :
	    ptr_{str}, len_{std::strlen(str)}, predicate_func_{predicate} {}
	// copy constructor
	filtered_string_view::filtered_string_view(const filtered_string_view &other) noexcept {
		ptr_ = other.ptr_;
		len_ = other.len_;
		predicate_func_ = other.predicate_func_;
	};
	// move constructor
	filtered_string_view::filtered_string_view(filtered_string_view &&other) noexcept {
		ptr_ = other.ptr_;
		len_ = other.len_;
		predicate_func_ = other.predicate_func_;
		other.ptr_ = nullptr;
		other.len_ = 0;
		other.predicate_func_ = default_predicate;
	};

	// copy assignment
	auto filtered_string_view::operator=(const filtered_string_view & other) noexcept -> filtered_string_view & = default;
	// move assignment
	auto filtered_string_view::operator=(filtered_string_view && other) noexcept -> filtered_string_view & {
		ptr_ = other.ptr_;
		len_ = other.len_;
		predicate_func_ = other.predicate_func_;
		other.ptr_ = nullptr;
		other.len_ = 0;
		other.predicate_func_ = default_predicate;
		return *this;
	};

	// destructor
	filtered_string_view::~filtered_string_view() noexcept = default;

	// Subscript
	auto filtered_string_view::operator[](int n)  const -> const char &{
		if (n >= static_cast<int>(len_) || n < 0 || ptr_ == nullptr) {
			std::string err_msg = "filtered_string_view::at(" + std::to_string(n) + "): invalid index";
			throw std::domain_error{err_msg.c_str()};
		}
		int index_ = 0;
		for (std::size_t i = 0; i < len_; ++i) {
			if (predicate_func_(ptr_[i])) {
				if (index_ == n) {
					const char &res_ = ptr_[i];
					return res_;
				}
				++index_;
			}
		}
		std::string err_msg = "filtered_string_view::at(" + std::to_string(n) + "): invalid index";
		throw std::domain_error{err_msg.c_str()};
	}

	// std::string conversion
	[[nodiscard]] filtered_string_view::operator std::string() const noexcept {
		if (ptr_ == nullptr) {
			return std::string{};
		}
		std::string res_;
		for (std::size_t i = 0; i < len_; ++i) {
			if (predicate_func_(ptr_[i])) {
				res_ += ptr_[i];
			}
		}
		return res_;
	}

	// member functions
	// Throws: a std::domain_error{"filtered_string_view::at(<index>): invalid index"},
	// where <index> should be replaced with the actual index passed in if the index is invalid.
	[[nodiscard]] auto filtered_string_view::at(int n) const -> const char&{
		if (n >= static_cast<int>(len_) || n < 0 || ptr_ == nullptr) {
			std::string err_msg = "filtered_string_view::at(" + std::to_string(n) + "): invalid index";
			throw std::domain_error{err_msg.c_str()};
		}
		int index_ = 0;
		for (std::size_t i = 0; i < len_; ++i) {
			if (predicate_func_(ptr_[i])) {
				if (index_ == n) {
					const char &res_ = ptr_[i];
					return res_;
				}
				++index_;
			}
		}
		std::string err_msg = "filtered_string_view::at(" + std::to_string(n) + "): invalid index";
		throw std::domain_error{err_msg.c_str()};
	}

	[[nodiscard]] auto filtered_string_view::size() const noexcept-> std::size_t{
		if (ptr_ == nullptr) {
			return 0;
		}
		std::size_t res_ = 0;
		for (std::size_t i = 0; i < len_; ++i) {
			if (predicate_func_(ptr_[i])) {
				++res_;
			}
		}
		return res_;
	}

	auto filtered_string_view::empty() const noexcept -> bool{
		if (ptr_ == nullptr) {
			return true;
		}
		for (std::size_t i = 0; i < len_; ++i) {
			if (predicate_func_(ptr_[i])) {
				return false;
			}
		}
		return true;
	}

	[[nodiscard]] auto filtered_string_view::data() const noexcept -> const char*{
		return ptr_;
	}

	[[nodiscard]] auto filtered_string_view::predicate() const noexcept -> const filter &{
		const filter & res_ = predicate_func_;
		return res_;
	}


	auto operator==(const filtered_string_view &lhs, const filtered_string_view &rhs) -> bool{
		if (lhs.size() != rhs.size()) {
			return false;
		}
		for (int i = 0; i < static_cast<int>(lhs.size()); ++i) {
			if (lhs.at(i) != rhs.at(i)) {
				return false;
			}
		}
		return true;
	}

	auto operator<=>(const filtered_string_view &lhs, const filtered_string_view &rhs) -> std::strong_ordering{
		auto size_ = std::min(lhs.size(), rhs.size());
		for (int i = 0; i < static_cast<int>(size_); ++i) {
			if (lhs.at(i) != rhs.at(i)) {
				return lhs.at(i) <=> rhs.at(i);
			}
		}
		if (lhs.size() != rhs.size()) {
			return rhs.size() <=> lhs.size();
		}
		return std::strong_ordering::equal;
	}

	auto operator<<(std::ostream &os, const filtered_string_view &fsv) -> std::ostream &{
		if (fsv.empty()) {
			return os;
		}
		for (int i = 0; i < static_cast<int>(fsv.size()); ++i) {
			os << fsv.at(i);
			if (fsv.at(i) == '\0') {
				break;
			}
		}
		return os;
	}

	auto compose(const filtered_string_view &fsv, const std::vector<std::function<bool(const char &)>> &filts) -> filtered_string_view{
		auto pred_compose_ = [filts](char c) -> bool{
			for (const auto &filt_ : filts) {
				if (!filt_(c)) {
					return false;
				}
			}
			return true;
		};
		filtered_string_view res_{fsv.data(), pred_compose_};
		return res_;
	}


	filtered_string_view::filtered_string_view(const char *str, std::size_t len, filter predicate) noexcept
		: ptr_{str}, len_{len}, predicate_func_{predicate} {}

	auto split(const filtered_string_view &fsv, const filtered_string_view &tok) -> std::vector<filtered_string_view>{
		std::vector<filtered_string_view> res_;
		if (fsv.empty() || tok.empty() || fsv.size() < tok.size()) {
			res_.push_back(fsv);
			return res_;
		}
		int start_ = 0;
		int end_ = 0;
		for (int i = 0; i < static_cast<int>(fsv.size()); ++i) {
			if (fsv.at(i) == tok.at(0)) {
				bool flag_ = true;
				for (int j = 0; j < static_cast<int>(tok.size()); ++j) {
					if (fsv.at(i + j) != tok.at(j)) {
						flag_ = false;
						break;
					}
				}
				if (flag_) {
					if (start_ != end_) {
						res_.push_back(fsv.substr(start_, end_ - start_));
					} else {
						res_.emplace_back(filtered_string_view{});
					}
					start_ = end_ + static_cast<int>(tok.size());
					i += static_cast<int>(tok.size()) - 1;
				}
			}
			++end_;
		}

		if ((static_cast<int>(fsv.size()) - start_) == 0){
			res_.emplace_back(filtered_string_view{});
		} else {
			res_.push_back(fsv.substr(start_, static_cast<int>(fsv.size()) - start_));
		}
		return res_;
	}

	auto substr(const filtered_string_view &fsv, int pos, int count) -> filtered_string_view{
		auto rcount = count <= 0 ? static_cast<int>(fsv.size()) - pos : count;
		if (pos < 0 || pos >= static_cast<int>(fsv.size()) || rcount < 0) {
			std::string err_msg = "filtered_string_view::substr(" + std::to_string(pos) +", " +std::to_string(pos)+  "): invalid index";
			throw std::domain_error{err_msg.c_str()};
		}

		const char * new_ptr_ = fsv.ptr_;
		std::size_t new_len_ = 0;
		auto count_ = std::min( (static_cast<int>(fsv.size()) - pos), rcount);

		std::size_t i;
		int index_ = 0;
		for (i = 0; i < fsv.len_; ++i) {
			if (fsv.predicate_func_(fsv.ptr_[i])) {
				if (index_ == pos) {
					new_ptr_ = fsv.ptr_ + i;
					break ;
				}
				++index_;
			}
		}
		auto i_ = i;
		index_ = 0;
		for (; i < fsv.len_; ++i) {
			if (index_ == count_) break ;
			if (fsv.predicate_func_(fsv.ptr_[i])) {
				++index_;
			}
		}
		new_len_ = i-i_;
		return filtered_string_view {new_ptr_,new_len_, fsv.predicate_func_};
	}

	auto filtered_string_view::substr(int pos, int count) const -> filtered_string_view {
		return fsv::substr(*this, pos, count);
	}

	fsv::filtered_string_view::iter::iter() = default;
	fsv::filtered_string_view::iter::iter(const char *ptr, const filter &pred, const std::size_t len, bool ending) noexcept{
		iter_ptr_ = std::remove_cv_t<char *>(ptr);
		begin_ = ptr;
		end_ = ptr;
		pred_ = pred;

		for (auto i = ptr; i < ptr + len * sizeof(char); ++i) {
			if (pred_(*i)) {
				begin_ = i;
				iter_ptr_ = std::remove_cv_t<char *>(i);
				break;
			}
		}
		for (auto i = ptr; i < ptr + len * sizeof(char); ++i) {
			if (pred_(*i)) {
				end_ = i;
			}
		}

		if (ending && len != 0) {
			iter_ptr_ = std::remove_cv_t<char *>(end_) + 1 * sizeof(char);
		}

	}

	auto fsv::filtered_string_view::iter::operator*() const -> reference {
		return *iter_ptr_;
	}

	auto fsv::filtered_string_view::iter::operator->() const -> void {
	}

	auto fsv::filtered_string_view::iter::operator++() -> iter & {
		if (iter_ptr_==end_) {
			iter_ptr_ = std::remove_cv_t<char *>(end_) + 1 * sizeof(char);
		}
		for (auto i = iter_ptr_ + 1 * sizeof(char); i <= end_; ++i) {
			if (pred_(*i)) {
				iter_ptr_ = i;
				break;
			}
		}
		return *this;
	}

	auto fsv::filtered_string_view::iter::operator++(int) -> iter {
		iter temp (*this);
		++*this;
		return temp;
	}

	auto fsv::filtered_string_view::iter::operator--() -> iter & {

		for (auto i = iter_ptr_ - 1 * sizeof(char); i >= begin_; --i) {
			if (pred_(*i)) {
				iter_ptr_ = i;
				break;
			}
		}
		return *this;
	}

	auto fsv::filtered_string_view::iter::operator--(int) -> iter {
		iter temp (*this);
		--*this;
		return temp;
	}

	auto operator==(const fsv::filtered_string_view::iterator &lhs, const fsv::filtered_string_view::iterator &rhs) -> bool {
		return lhs.iter_ptr_ == rhs.iter_ptr_;
	}

	auto operator!=(const fsv::filtered_string_view::iterator &lhs, const fsv::filtered_string_view::iterator &rhs) -> bool {
		return !(lhs == rhs);
	}

	auto fsv::filtered_string_view::begin() const noexcept-> iterator {
		return iterator{data(), predicate(), len_};
	}

	auto fsv::filtered_string_view::end() const noexcept-> iterator {
		return iterator{data(), predicate(), len_, true};
	}

	auto fsv::filtered_string_view::cbegin() const noexcept-> const_iterator {
		return iterator{data(), predicate(), len_};
	}

	auto fsv::filtered_string_view::cend() const noexcept-> const_iterator {
		return iterator{data(), predicate(), len_, true};
	}

	auto fsv::filtered_string_view::rbegin() const noexcept-> reverse_iterator {
		return reverse_iterator{end()};
	}

	auto fsv::filtered_string_view::rend() const noexcept-> reverse_iterator {
		return reverse_iterator{begin()};
	}

	auto fsv::filtered_string_view::crbegin() const noexcept-> const_reverse_iterator {
		return reverse_iterator{cend()};
	}

	auto fsv::filtered_string_view::crend() const noexcept-> const_reverse_iterator {
		return reverse_iterator{cbegin()};
	}

}