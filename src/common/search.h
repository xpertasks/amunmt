#pragma once

#include <memory>
#include <boost/shared_ptr.hpp>
#include "common/scorer.h"
#include "common/sentence.h"
#include "common/base_best_hyps.h"
#include "common/history.h"


class Search {
  public:
    Search(size_t threadId);
    boost::shared_ptr<Histories> Decode(const Sentences& sentences);

  private:
    Search(const Search &) = delete;

    size_t MakeFilter(const std::set<Word>& srcWords, size_t vocabSize);
    void InitScorers(const Sentences& sentences, States& states, States& nextStates);

    std::vector<ScorerPtr> scorers_;
    Words filterIndices_;
    BestHypsBase &bestHyps_;
};
