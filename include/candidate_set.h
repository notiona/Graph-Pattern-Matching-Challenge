/**
 * @file candidate_set.h
 *
 */

#ifndef CANDIDATE_SET_H_
#define CANDIDATE_SET_H_

#include "common.h"

class CandidateSet {
 public:
  explicit CandidateSet(const std::string& filename);
  ~CandidateSet();

  inline size_t GetCandidateSize(Vertex u) const;
  inline Vertex GetCandidate(Vertex u, size_t i) const;
  inline size_t GetCSSize() const;

 private:
  std::vector<std::vector<Vertex>> cs_;
  size_t cs_size;
};

/**
 * @brief Returns the number of data vertices that may be mapped to query vertex
 * u.
 *
 * @param u query vertex id.
 * @return size_t
 */
inline size_t CandidateSet::GetCandidateSize(Vertex u) const {
  return cs_[u].size();
}
/**
 * @brief Returns the i-th candidate from query vertex u's candidate set.
 *
 * @param u query vertex id.
 * @param i index in half-open interval [0, GetCandidateSize(u)).
 * @return Vertex
 */
inline Vertex CandidateSet::GetCandidate(Vertex u, size_t i) const {
  return cs_[u][i];
}

/**
 * @brief Returns the size of whole nodes in CS
 * @return size_t
 * @author Jinhyeong Kim
 */
inline size_t CandidateSet::GetCSSize() const{
  return cs_size;
}

#endif  // CANDIDATE_SET_H_
