#ifndef RULES_H
#define RULES_H

#include <map>
#include <vector>

const int COLORS = 5;
const int MIN_WIRES = 3, MAX_WIRES = 6;

enum Color { White, Black, Blue, Red, Yellow, Empty };

enum ConditionType { ColorCount, PositionColor };

enum ColorCountOptions { None, One, MoreThanOne };

struct Condition {
  ConditionType type;
  ColorCountOptions color_count_options;
  Color color;
  int position;
  bool operator==(const Condition& other) const {
    if (type != other.type || color != other.color) return false;
    if (type == ColorCount)
      return color_count_options == other.color_count_options;
    else
      return position == other.position;
  }
};

enum ActionType { ColorPosition, Position };

enum ColorPositionOptions { Only, First, Last };

struct Action {
  ActionType type;
  ColorPositionOptions color_position_options;
  Color color;
  int position;
};

struct Rule {
  std::vector<Condition> conditions;
  Action action;
};

using Wiring = std::vector<Color>;
using Rules = std::vector<Rule>;

std::map<int, Rules> generate_rules(int code);

#endif  // RULES_H