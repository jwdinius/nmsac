#pragma once
//! c/c++ headers
#include <map>
#include <utility>
//! dependency headers
//! project headers
#include "types.hpp"

namespace correspondences {
/**
 * @interface CorrespondencesBase
 *
 * @brief very thin interface for correspondences algorithms
 */
class CorrespondencesBase {
 public:
   CorrespondencesBase() { }
   ~CorrespondencesBase() { }  // must define this for abstract base classes
   //! every child must implement the following method
   virtual status_e calc_correspondences(correspondences_t & correspondences) = 0;
};
}  // namespace correspondences
