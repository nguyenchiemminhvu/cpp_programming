# C++20 › Calendar & Time Zone Extensions (`<chrono>`)

## Overview
C++20 massively extends `<chrono>` with a human-readable **calendar system**
(year, month, day, weekday) and **time zone** support, replacing `<ctime>` for
calendar arithmetic.

## New Types

| Type | Represents |
|------|-----------|
| `std::chrono::year` | A calendar year |
| `std::chrono::month` | A calendar month (1–12) |
| `std::chrono::day` | A day of month (1–31) |
| `std::chrono::year_month_day` | Full calendar date |
| `std::chrono::weekday` | Day of week (Sunday=0 … Saturday=6) |
| `std::chrono::hh_mm_ss<D>` | Hour/minute/second breakdown |
| `std::chrono::time_zone` | IANA time zone |
| `std::chrono::zoned_time<D>` | Time point in a specific time zone |

## Date Literals and Construction
```cpp
using namespace std::chrono;
using namespace std::chrono_literals;

auto d = 2024y / January / 15d;   // year_month_day
auto d2 = 15d / January / 2024y;  // same

bool ok = d.ok();                  // validates date

// Convert to/from time_point
auto tp = sys_days{d};             // days since epoch
auto back = year_month_day{tp};    // recover calendar date
```

## Time Zones
```cpp
auto tz = std::chrono::locate_zone("Europe/Paris");
auto now = std::chrono::system_clock::now();
std::chrono::zoned_time zt{tz, now};
std::cout << std::format("{:%Y-%m-%d %H:%M:%S %Z}", zt);
```

## Study Checklist
- [ ] Calculate the number of days between two dates using `sys_days`
- [ ] Find what weekday a given date falls on using `std::chrono::weekday`
- [ ] Convert UTC `time_point` to local time in multiple time zones
- [ ] Validate user-provided dates with `year_month_day::ok()`
- [ ] Format dates/times with `std::format` using `%Y-%m-%d %H:%M:%S`

## References
- [cppreference — Calendar](https://en.cppreference.com/w/cpp/chrono#Calendar)
- [cppreference — Time zones](https://en.cppreference.com/w/cpp/chrono#Time_zones)
