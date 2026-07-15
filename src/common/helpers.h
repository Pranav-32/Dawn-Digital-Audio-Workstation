#pragma once
#include <cstddef>

namespace dawn::common::helpers
{
    template <typename>
    [[nodiscard]]
    constexpr const void* typeToken() noexcept
    {
        static constexpr int token = 0;
        return &token;
    }
} // namespace dawn::common::helpers
