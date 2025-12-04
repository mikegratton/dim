#pragma once
#include "unit.hpp"
#include <stdexcept>
#include <string>

namespace dim
{

/**
 * @brief Exception for when types have different dimensions.
 */
class incommensurable_exception : public std::runtime_error
{
  public:
    template <class S1, class S2, DIM_IS_SYSTEM(S1)>
    incommensurable_exception(dynamic_unit<S1> const& u1, dynamic_unit<S2> const& u2, char const* i_message)
        : std::runtime_error(print_error(u1, u2, i_message))
    {
    }

    template <class U1, class U2, DIM_IS_UNIT(U1)>
    incommensurable_exception(U1 const& u1, U2 const&, char const* i_message)
        : incommensurable_exception(index(u1), index<U2>(), i_message)
    {
    }

    template <class U1, class S2, DIM_IS_UNIT(U1)>
    incommensurable_exception(U1 const& u1, dynamic_unit<S2> const& u2, char const* i_message)
        : incommensurable_exception(index(u1), u2, i_message)
    {
    }

    template <class U1, class S2, DIM_IS_UNIT(U1)>
    incommensurable_exception(dynamic_unit<S2> const& u2, U1 const& u1, char const* i_message)
        : incommensurable_exception(u2, index(u1), i_message)
    {
    }

  private:
    // This is defined in io_detail.hpp
    template <class S1, class S2>
    static std::string print_error(dynamic_unit<S1> const& u1, dynamic_unit<S2> const& u2, char const* i_message);
};
} // namespace dim