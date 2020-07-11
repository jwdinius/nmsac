#pragma once
//! c/c++ headers
#include <map>
#include <utility>
//! dependency headers
//! project headers
#include "types.hpp"

namespace correspondences {
// very thin wrapper
class CorrespondencesBase {
 public:
   CorrespondencesBase() { };
   ~CorrespondencesBase() { };  // gotta define this for abstract base classes
   virtual status_e calc_correspondences(correspondences_t & correspondences) = 0;  // every child must implement this method
};
}  // namespace correspondences
