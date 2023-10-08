#include <cstdint>
#include <tuple>

template <typename... T> class Literal {
public:
  template <typename... Args>
  Literal(Args &&... args) : name_(std::forward<Args>(args)...) {}

  friend bool operator==(const Literal &lhs, const Literal &rhs) {
    return lhs.name_ == rhs.name_;
  }

  std::tuple<T...> GetName() const { return name_; }

private:
  std::tuple<T...> name_;
};

// Credit to:
// https://www.linkedin.com/pulse/generic-tuple-hashing-modern-c-alex-dathskovsky/
struct LiteralHasher {
  static constexpr auto hasher = [](size_t hash, auto &&... values) -> size_t {
    auto combiner = [&hash](auto &&val) {
      hash ^= std::hash<std::decay_t<decltype(val)>>{}(val) + 0Xeeffddcc +
              (hash << 5) + (hash >> 3);
    };

    (combiner(std::forward<decltype(values)>(values)), ...);
    return hash;
  };

  template <typename... T> size_t operator()(const Literal<T...> &t) const {
    size_t hash = 0;
    std::apply(hasher, std::tuple_cat(std::tuple(0), t.GetName()));
    return hash;
  }
};