#pragma once

#include <set>

#include "card.h"

class CardSet {
  using ContainerType = std::set<Card>;

 public:
  void Add(Card c) { storage_.insert(c); }
  bool Contains(Card c) const { return storage_.find(c) != storage_.end(); }
  ContainerType::const_iterator begin() const { return storage_.cbegin(); }
  ContainerType::const_iterator end() const { return storage_.cend(); }

 private:
  ContainerType storage_;
};