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
#include "togasat.hpp"

struct ValidCombination {
  enum class Type { run, triplet };
  auto operator<=>(const ValidCombination&) const = default;

  friend std::ostream& operator<<(std::ostream& out,
                                  const ValidCombination& c) {
    std::string type_str = (c.t == Type::run) ? "run" : "triplet";
    out << type_str << " ";
    out << c.cards[0];
    for (size_t i = 1; i < c.cards.size(); ++i) {
      out << " " << c.cards[i];
    }
    return out;
  }

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
  return std::vector<ValidCombination>{dedup.begin(), dedup.end()};
}

int main() {
  CardSet cards_set;
  for (Card c; std::cin >> c; cards_set.Add(c))
    ;

  /*for (bool type : {true, false})
    for (int color = 0; color < 4; ++color)
      for (int val = 0; val < 13; ++val) {
        cards_set.Add(Card{.type = type, .col = color, .val = val});
      }*/

  std::cout << "Cards:";
  for (Card c : cards_set) std::cout << " " << c;
  std::cout << "\n";

  std::vector<ValidCombination> combs = generate_combinations(cards_set);

  std::vector<Card> sorted_cards(cards_set.begin(), cards_set.end());
  std::map<Card, std::vector<int64_t>> card_to_combs;
  for (int64_t i = 0; i < combs.size(); ++i) {
    // std::cout << "Combination is " << combs[i] << "\n";
    for (Card c : combs[i].cards) {
      card_to_combs[c].push_back(i);
    }
  }

  auto comb_to_literal_id = [](int64_t idx) -> int64_t { return idx + 1; };

  // A => B         <=>       !A or B
  std::vector<std::vector<int>> clauses;
  for (const auto& [card, comb_idxs] : card_to_combs) {
    clauses.push_back({});
    for (int64_t idx : comb_idxs) {
      clauses.back().push_back(comb_to_literal_id(idx));
    }
    for (int64_t idx1 : comb_idxs) {
      for (int64_t idx2 : comb_idxs) {
        if (idx1 == idx2) continue;
        clauses.push_back(
            {(int)-comb_to_literal_id(idx1), (int)-comb_to_literal_id(idx2)});
      }
    }
  }

  togasat::Solver solver;
  for (std::vector<int> c : clauses) {
    solver.addClause(c);
  }

  togasat::lbool status = solver.solve();

  if (status != 0) {
    std::cout << "Unsolvable!\n";
    return 0;
  }

  std::vector<int> answer = solver.getAnswer();
  for (int idx : answer) {
    if (idx < 0) continue;
    std::cout << "Combination: " << combs[idx - 1] << "\n";
  }

  /*std::cout << status << "\n";
  std::cout << "p cnf " << combs.size() << " " << clauses.size() << "\n";
  for (std::vector<int64_t> c : clauses) {
    std::cout << c[0];
    for (size_t i = 1; i < c.size(); ++i) {
      std::cout << " " << c[i];
    }
    std::cout << " 0\n";
  }*/

  return 0;
}