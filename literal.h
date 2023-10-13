#include <cstdint>
#include <string>
#include <tuple>
#include <utility>

class Literal {};

/*template <typename... T>
class Literal {
 public:
  template <typename... Args>
  Literal(Args &&...args) : name_(std::forward<Args>(args)...) {}

  friend bool operator==(const Literal &lhs, const Literal &rhs) {
    return lhs.name_ == rhs.name_;
  }

  std::tuple<T...> GetName() const { return name_; }

  static constexpr auto stringifyer = [](std::string str, auto &&...values) {
    auto combiner = [&str](auto &&val) {
      str += "a";
      std::cout << "Again!\n";
    };

    (combiner(std::forward<decltype(values)>(values)), ...);
    return str;
  };

  std::string ToString() {
    using TupleT = std::tuple<T...>;

    std::string str;
    auto fn = [&str](const auto &val) {
      using CompareT = typename std::remove_const<
          typename std::remove_reference<decltype(val)>::type>::type;
      if constexpr (std::is_same_v<std::string, CompareT>) {
        str += val;
        return;
      }
      if constexpr (std::is_arithmetic_v<CompareT>) {
        str += std::to_string(val);
        return;
      }
      str += "<unsupported_type_used>";
    };

    std::apply([&fn]<typename... TE>(
                   TE &&...args) { (fn(std::forward<TE>(args)), ...); },
               std::forward<TupleT>(name_));

    return str;
  }

 private:
  std::tuple<T...> name_;
};*/

// Credit to:
// https://www.linkedin.com/pulse/generic-tuple-hashing-modern-c-alex-dathskovsky/
/*struct LiteralHasher {
  static constexpr auto hasher = [](size_t hash, auto &&...values) -> size_t {
    auto combiner = [&hash](auto &&val) {
      hash ^= std::hash<std::decay_t<decltype(val)>>{}(val) + 0Xeeffddcc +
              (hash << 5) + (hash >> 3);
    };

    (combiner(std::forward<decltype(values)>(values)), ...);
    return hash;
  };

  template <typename... T>
  size_t operator()(const Literal<T...> &t) const {
    size_t hash = 0;
    std::apply(hasher, std::tuple_cat(std::tuple(0), t.GetName()));
    return hash;
  }
};*/