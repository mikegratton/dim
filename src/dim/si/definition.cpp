#include "definition.hpp"

namespace dim {
namespace si {
const char* system::kSymbol[] = {"m", "s", "kg", "rad", "K", "mol", "A", "cd"};
const long system::id = 26980L;
}  // namespace si

template class dynamic_unit<si::system>;

}  // namespace dim
