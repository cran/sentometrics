
#include <Rcpp.h>
#include <RcppParallel.h>
#include "utils.h"
#include "SentimentScorerBigrams.h"
#include "SentimentScorerClusters.h"
// [[Rcpp::depends(RcppParallel)]]

using namespace std;
using namespace Rcpp;
using namespace RcppParallel;

// [[Rcpp::export]]
Rcpp::NumericMatrix compute_sentiment_valence(std::vector< std::vector<std::string> > texts,
                                              Rcpp::List lexicons,
                                              std::string how) {

  int nTexts = texts.size(); // already tokenized texts
  int nL = lexicons.size() - 1;
  Rcpp::CharacterVector colNames = prepare_column_names(lexicons.names(), nL);

  std::unordered_map< std::string, std::vector<double> > lexiconMap = make_lexicon_map(lexicons, nL);

  Rcpp::List valenceList = lexicons[nL];
  Rcpp::CharacterVector valenceCols = valenceList.names();
  std::unordered_map< std::string, double > valenceMap = make_valence_map(valenceList);

  Rcpp::NumericMatrix sentScores(nTexts, nL + 1); // output matrix of word count and sentiment scores
  if (valenceCols[1] == "y") {
    SentimentScorerBigrams sentimentScorer(texts, lexiconMap, valenceMap, how, nL, sentScores);
    parallelFor(0, nTexts, sentimentScorer);
  } else if (valenceCols[1] == "t") {
    SentimentScorerClusters sentimentScorer(texts, lexiconMap, valenceMap, how, nL, sentScores);
    parallelFor(0, nTexts, sentimentScorer);
  }

  colnames(sentScores) = colNames;

  return(sentScores);
}

