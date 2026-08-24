#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// fixed_buffer<N> — stack-allocated, bounded, append-only string builder.
// ─────────────────────────────────────────────────────────────────────────────

/**
 * A fixed-capacity character buffer used to assemble log messages and records
 * without any heap allocation.
 *
 *   • Capacity N is a compile-time constant — total footprint is N+1 bytes
 *     plus book-keeping (size + truncated flag).  The extra byte reserves
 *     room for a trailing NUL so c_str() is always valid.
 *   • append() silently truncates on overflow and sets truncated() = true.
 *     A trailing "..." sentinel replaces the last 3 bytes so clipping is
 *     visible to the reader.
 *   • view() returns a std::string_view for zero-copy consumption.
 *
 * Not thread-safe on its own; the logger serialises access via its mutex.
 */
template <std::size_t N>
class fixed_buffer
{
    static_assert(N >= 16, "fixed_buffer capacity must leave room for prefix + ellipsis");

public:
    fixed_buffer() noexcept { data_[0] = '\0'; }

    static constexpr std::size_t capacity() noexcept { return N; }

    std::size_t size()      const noexcept { return size_; }
    bool        empty()     const noexcept { return size_ == 0; }
    bool        truncated() const noexcept { return truncated_; }

    const char*      c_str() const noexcept { return data_.data(); }
    std::string_view view()  const noexcept { return { data_.data(), size_ }; }

    void clear() noexcept
    {
        size_      = 0;
        truncated_ = false;
        data_[0]   = '\0';
    }

    // ── append overloads ────────────────────────────────────────────────

    void append(std::string_view sv) noexcept { write_raw(sv.data(), sv.size()); }

    void append(const char* s) noexcept
    {
        if (s == nullptr) { append(std::string_view{"(null)"}); return; }
        write_raw(s, std::strlen(s));
    }

    void append(char c) noexcept { write_raw(&c, 1); }

    void append(const std::string& s) noexcept { write_raw(s.data(), s.size()); }

    // Integers — formatted into a small stack scratch, then copied.
    void append(long long v)          noexcept { append_fmt("%lld", v); }
    void append(unsigned long long v) noexcept { append_fmt("%llu", v); }
    void append(long v)               noexcept { append_fmt("%ld",  v); }
    void append(unsigned long v)      noexcept { append_fmt("%lu",  v); }
    void append(int v)                noexcept { append_fmt("%d",   v); }
    void append(unsigned int v)       noexcept { append_fmt("%u",   v); }
    void append(short v)              noexcept { append_fmt("%d",   static_cast<int>(v)); }
    void append(unsigned short v)     noexcept { append_fmt("%u",   static_cast<unsigned>(v)); }

    // Floating point — "%g" defaults (6 sig figs).
    void append(double v) noexcept { append_fmt("%g", v); }
    void append(float  v) noexcept { append_fmt("%g", static_cast<double>(v)); }

    // Pointers
    void append(const void* p) noexcept { append_fmt("%p", p); }

    // Bool
    void append(bool v) noexcept { append(std::string_view{ v ? "true" : "false" }); }

private:
    void append_fmt(const char* fmt, ...) noexcept
    {
        // 32 bytes is enough for any 64-bit integer / %g double / %p.
        char    scratch[32];
        va_list ap;
        va_start(ap, fmt);
        const int n = std::vsnprintf(scratch, sizeof(scratch), fmt, ap);
        va_end(ap);
        if (n > 0)
        {
            const std::size_t len =
                (static_cast<std::size_t>(n) < sizeof(scratch))
                    ? static_cast<std::size_t>(n)
                    : sizeof(scratch) - 1;
            write_raw(scratch, len);
        }
    }

    void write_raw(const char* src, std::size_t len) noexcept
    {
        if (truncated_) return;                     // Nothing more fits.

        const std::size_t room = (size_ < N) ? (N - size_) : 0;
        if (len <= room)
        {
            std::memcpy(data_.data() + size_, src, len);
            size_ += len;
        }
        else
        {
            if (room > 0)
            {
                std::memcpy(data_.data() + size_, src, room);
                size_ += room;
            }
            mark_truncated();
        }
        data_[size_] = '\0';
    }

    void mark_truncated() noexcept
    {
        truncated_ = true;
        // Overwrite the last 3 bytes with an ellipsis so clipping is visible.
        constexpr std::size_t k = 3;
        if (size_ >= k)
        {
            const std::size_t pos = size_ - k;
            data_[pos + 0] = '.';
            data_[pos + 1] = '.';
            data_[pos + 2] = '.';
        }
        data_[size_] = '\0';
    }

    std::array<char, N + 1> data_{};
    std::size_t             size_      = 0;
    bool                    truncated_ = false;
};
