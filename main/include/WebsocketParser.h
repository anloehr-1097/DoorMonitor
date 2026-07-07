#pragma once
#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
concept Stringifiable =
    std::is_constructible_v<std::string, T> || requires(T a) {
      { a.to_string() } -> std::same_as<std::string>;
    };

template <Stringifiable T> class GenericWSParser {
public:
  std::optional<std::string> parse(T &&value) {
    if constexpr (!std::is_convertible_v<T, std::string>) {
      std::cout << "T not convertible to string";
      return std::nullopt;
    }

    std::string data{};
    if constexpr (std::is_constructible_v<std::string, T>) {
      data = std::string{std::forward<T>(value)};
    } else {
      // constraint ensures that is has a to_string method
      data = std::string{value.to_string()};
    }

    std::cout << "Data: " << data << std::endl;
    // timestamp
    auto ts{std::chrono::system_clock::now()};
    auto sts{std::format("{:%Y-%m-%d %H:%M:%S}", ts)};
    std::cout << "Timestamp: " << sts << std::endl;
    return sts + ";" + data;
  }
};
