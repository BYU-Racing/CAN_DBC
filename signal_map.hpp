#pragma once

#include <optional>
#include <string>
#include <unordered_map>

struct DecodedSignal {
    std::string name;
    double value;
    std::string unit;
};

class SignalMap {
  public:
    SignalMap(std::unordered_map<std::string, DecodedSignal> sig_map);

    // Gets DecodedSignal struct from signal name
    std::optional<DecodedSignal> getSignalByName(std::string sig_name);

    // Gets data held by signal from signal name
    std::optional<double> getSignalValue(std::string sig_name);

  private:
    std::unordered_map<std::string, DecodedSignal> sig_map;
};