#include <boost/thread/tss.hpp>
#include "translation_task.h"
#include "search.h"
#include "output_collector.h"
#include "printer.h"

void TranslationTask(boost::shared_ptr<Sentences> sentences, size_t taskCounter, size_t maxBatchSize) {
  //std::cerr << "TranslationTaskStart" << std::endl;
  thread_local std::unique_ptr<Search> search;
  if(!search) {
    LOG(info) << "Created Search for thread " << std::this_thread::get_id();
    search.reset(new Search(taskCounter));
  }

  try {
    Histories allHistories;
    sentences->SortByLength();

    size_t bunchId = 0;
    boost::shared_ptr<Sentences> decodeSentences(new Sentences(taskCounter, bunchId++));
    for (size_t i = 0; i < sentences->size(); ++i) {
      decodeSentences->push_back(sentences->at(i));

      if (decodeSentences->size() >= maxBatchSize) {
        assert(decodeSentences->size());
        boost::shared_ptr<Histories> histories = search->Decode(*decodeSentences);
        allHistories.Append(*histories.get());

        decodeSentences.reset(new Sentences(taskCounter, bunchId++));
      }
    }

    if (decodeSentences->size()) {
      boost::shared_ptr<Histories> histories = search->Decode(*decodeSentences);
      allHistories.Append(*histories.get());
    }

    allHistories.SortByLineNum();

    std::stringstream strm;
    Printer(allHistories, strm);

    OutputCollector &outputCollector = God::GetOutputCollector();
    outputCollector.Write(taskCounter, strm.str());
    //std::cerr << "TranslationTaskEnd" << std::endl;
  }
  catch(thrust::system_error &e)
  {
    std::cerr << "CUDA error during some_function: " << e.what() << std::endl;
    abort();
  }
  catch(std::bad_alloc &e)
  {
    std::cerr << "Bad memory allocation during some_function: " << e.what() << std::endl;
    abort();
  }
  catch(std::runtime_error &e)
  {
    std::cerr << "Runtime error during some_function: " << e.what() << std::endl;
    abort();
  }
  catch(...)
  {
    std::cerr << "Some other kind of error during some_function" << std::endl;
    abort();

    // no idea what to do, so just rethrow the exception
    //throw;
  }

}

