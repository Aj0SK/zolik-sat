#include <cstdint>
#include <iostream>
#include <map>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "card.h"
#include "card_set.h"
#include "literal.h"

void generate_runs(const CardSet& cards, const Card& card) {
  std::vector<std::vector<Card> > run_cands(5);
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
  if (run_cands.size() < 3) return;

  for (uint64_t mask = 0; mask < (1ull << run_cands.size()); ++mask) {
    std::vector<Card> run = {run_cands[0][0]};
    bool valid_run = true;
    for (size_t i = 1; i < run_cands.size(); ++i) {
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
    for (size_t run_size = 3; run_size <= run.size(); ++run_size) {
      std::cout << "Generated run: ";
      for (size_t i = 0; i < run_size; ++i) std::cout << run[i] << " ";
      std::cout << "\n";
    }
  }
}

void generate_triplets(const CardSet& cards, const Card& card) {
  std::map<int, std::vector<Card> > tripl_cands;
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

  if (tripl_cands.size() < 3) return;
  for (uint64_t mask = 0; mask < (1ull << tripl_cands.size()); ++mask) {
    std::vector<Card> triple = {tripl_cands[0][0]};
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
      std::cout << "Generated triplet: ";
      for (size_t i = 0; i < triple.size(); ++i) {
        if ((mask2 & (1ull << i)) != 0) {
          std::cout << triple[i] << " ";
        }
      }
      std::cout << "\n";
    }
  }
}

void generate_combinations_for_card(const CardSet& cards, const Card& card) {
  generate_runs(cards, card);
  generate_triplets(cards, card);
}

void generate_combinations(const CardSet& cards) {
  for (const Card& c : cards) {
    // Skipping Jokers for now.
    if (c.val_str() == "Joker") continue;
    generate_combinations_for_card(cards, c);
  }
}

int main() {
  CardSet cards;
  for (Card c; std::cin >> c.type >> c.col >> c.val; cards.Add(c)) {
  }

  for (const Card& c : cards) {
    std::cout << c << "\n";
  }

  generate_combinations(cards);

  // std::unordered_set<Literal<int, std::string>, LiteralHasher> literals;
  // Literal<int, std::string> l1(0, "a");
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