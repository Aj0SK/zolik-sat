#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "card.h"
#include "card_set.h"
#include "literal.h"

struct ValidCombination {
  enum class Type { run, triplet };

  auto operator<=>(const ValidCombination&) const = default;

 public:
  Type t;
  std::vector<Card> cards;
};

std::vector<ValidCombination> generate_runs(const CardSet& cards,
                                            const Card& card) {
  std::vector<ValidCombination> out;

  std::vector<std::vector<Card>> run_cands(5);
  run_cands[0].push_back(card);

  size_t max_shift = 1;
  for (; max_shift < 5; ++max_shift) {
    Card c;
    c.col = card.col;
    int shifted_val = card.val + max_shift;
    c.val = shifted_val % 13;
    if (c.val == 1 && shifted_val >= 13) {
      break;
    }
    for (const bool type : {true, false}) {
      c.type = type;
      if (cards.Contains(c)) {
        run_cands[max_shift].push_back(c);
      }
    }
    if (run_cands[max_shift].empty()) {
      break;
    }
  }

  run_cands.resize(max_shift);
  if (run_cands.size() < 3) return {};

  for (size_t run_size = 3; run_size <= run_cands.size(); ++run_size) {
    for (uint64_t mask = 0; mask < (1ull << run_size); ++mask) {
      std::vector<Card> run = {run_cands[0][0]};
      if ((mask & 1) == 1) {
        continue;
      }
      bool valid_run = true;
      for (size_t i = 1; i < run_size; ++i) {
        if (bool sel = mask & (1ull << i); sel != 0) {
          run.push_back(run_cands[i][0]);
        } else if (sel == 0 && run_cands[i].size() > 1) {
          run.push_back(run_cands[i][1]);
        } else {
          valid_run = false;
          break;
        }
      }
      if (!valid_run) continue;

      ValidCombination comb;
      comb.t = ValidCombination::Type::run;
      for (const Card& c : run) comb.cards.push_back(c);
      out.push_back(comb);
    }
  }

  return out;
}

std::vector<ValidCombination> generate_triplets(const CardSet& cards,
                                                const Card& card) {
  std::vector<ValidCombination> out;

  std::map<int, std::vector<Card>> tripl_cands;
  tripl_cands[card.col].push_back(card);

  Card candidate;
  candidate.val = card.val;
  for (const bool type : {true, false}) {
    candidate.type = type;
    for (int col = 0; col < 4; ++col) {
      if (col == card.col) continue;
      candidate.col = col;
      if (cards.Contains(candidate)) {
        tripl_cands[col].push_back(candidate);
      }
    }
  }

  if (tripl_cands.size() < 3) return {};
  for (uint64_t mask = 0; mask < (1ull << tripl_cands.size()); ++mask) {
    std::vector<Card> triple = {tripl_cands[0][0]};
    if ((mask & 1) == 1) {
      continue;
    }
    bool valid_triple = true;
    for (size_t i = 1; i < tripl_cands.size(); ++i) {
      if (bool sel = mask & (1ull << i); sel != 0) {
        triple.push_back(tripl_cands[i][0]);
      } else if (sel == 0 && tripl_cands[i].size() > 1) {
        triple.push_back(tripl_cands[i][1]);
      } else {
        valid_triple = false;
        break;
      }
    }
    if (!valid_triple) continue;
    if (triple.size() < 3) continue;
    for (size_t mask2 = 3; mask2 < (1 << triple.size()); ++mask2) {
      // We must include the first card.
      if ((mask2 & 1) == 0) continue;
      if (__builtin_popcountll(mask2) < 3) continue;

      ValidCombination comb;
      comb.t = ValidCombination::Type::triplet;

      for (size_t i = 0; i < triple.size(); ++i) {
        if ((mask2 & (1ull << i)) != 0) {
          comb.cards.push_back(triple[i]);
        }
      }
      if (std::is_sorted(comb.cards.begin(), comb.cards.end())) {
        out.push_back(comb);
      }
    }
  }
  return out;
}

std::vector<ValidCombination> generate_combinations_for_card(
    const CardSet& cards, const Card& card) {
  std::vector<ValidCombination> runs = generate_runs(cards, card);
  std::vector<ValidCombination> triplets = generate_triplets(cards, card);

  std::vector<ValidCombination> aggreg;
  aggreg.reserve(runs.size() + triplets.size());
  aggreg.insert(aggreg.end(), runs.begin(), runs.end());
  aggreg.insert(aggreg.end(), triplets.begin(), triplets.end());

  return aggreg;
}

std::vector<ValidCombination> generate_combinations(const CardSet& cards) {
  std::vector<ValidCombination> all_comb;
  for (const Card& c : cards) {
    // Skipping Jokers for now.
    if (c.val_str() == "Joker") continue;
    std::vector<ValidCombination> comb =
        generate_combinations_for_card(cards, c);
    all_comb.insert(all_comb.end(), comb.begin(), comb.end());
  }

  std::set<ValidCombination> dedup(all_comb.begin(), all_comb.end());
  all_comb.assign(dedup.begin(), dedup.end());
  return all_comb;
}

int main() {
  CardSet cards;
  /*for (Card c; std::cin >> c.type >> c.col >> c.val; cards.Add(c))
    ;*/

  for (bool type : {true, false})
    for (int color = 0; color < 4; ++color)
      for (int val = 0; val < 13; ++val) {
        cards.Add(Card{.type = type, .col = color, .val = val});
      }

  const std::vector<ValidCombination> map = generate_combinations(cards);

  /*
  std::map<Card, std::vector<Literal<bool, int, int, int>>> card_to_clausules;
  for (const auto& [card, combs] : map) {
    for (const ValidCombination& comb : combs) {
    }
  }*/

  for (const ValidCombination& comb : map) {
    for (Card c : comb.cards) {
      std::cout << c << " ";
    }
    std::cout << "\n";
  }

  // std::unordered_set<Literal<int, std::string>, LiteralHasher> literals;
  // Literal<int, std::string, float, bool> l1(10, "oko", 12.3, true);
  // std::cout << l1.ToString() << "\n";
  // Literal<int, std::string> l2(1, "b");
  return 0;
}

/*
class Clause {
 public:
  void Negate() {
    for (LiteralInfo& li : clauses_) {
      li.sign = !li.sign;
    }
  }

 private:
  struct LiteralInfo {
    bool sign;
    int id;
  };

 private:
  std::vector<LiteralInfo> clauses_;
};
*/