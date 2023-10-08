#pragma once

#include <array>
#include <cstdint>
#include <ostream>

// Srdcia, piky, listy, kara
static constexpr std::array<std::array<std::string_view, 4>, 2> kColorNames = {
    {{{"\u2665", "\u2663", "\u2660", "\u2666"}},
     {{"\u2661", "\u2667", "\u2664", "\u2662"}}}};

static constexpr std::array<std::string_view, 14> kValueNames = {
    "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "Joker"};

struct Card {
  bool type;  // 0 - 1
  int col;    // 0 - 3
  int val;    // 0 - 12, 13 for J

  std::string_view val_str() const { return kValueNames[val]; }
  // Operations:
  auto operator<=>(const Card &) const = default;
  /*friend bool operator==(const Card &lhs, const Card &rhs) {
    return lhs.type == rhs.type && lhs.col == rhs.col && lhs.val == rhs.val;
  }*/
  friend std::ostream &operator<<(std::ostream &out, const Card &c) {
    out << kColorNames[c.type][c.col] << ":" << c.val_str();
    return out;
  }
};

struct CardHasher {
  std::size_t operator()(const Card &c) const {
    return std::hash<bool>()(c.type) ^ std::hash<int>()(c.col) ^
           std::hash<int>()(c.val);
  }
};