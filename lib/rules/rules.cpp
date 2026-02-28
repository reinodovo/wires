#include "rules.h"

#include <random>

std::mt19937 rng;

const int MIN_RULES = 4, MAX_RULES = 5;
const float double_condition_chance = 0.1;

int rand_range(int min, int max) { return rng() % (max - min + 1) + min; }

Condition create_random_condition(int wires) {
  Condition condition;
  condition.type = (ConditionType)rand_range(0, 1);
  condition.color = (Color)rand_range(0, COLORS - 1);
  if (condition.type == PositionColor)
    condition.position = rand_range(0, wires - 1);
  else
    condition.color_count_options = (ColorCountOptions)rand_range(0, 2);
  return condition;
}

Action create_random_action(int wires, std::vector<Condition> conditions) {
  std::vector<Color> allowed_colors;
  std::vector<int> allowed_color_count;
  for (auto condition : conditions) {
    if (condition.type == color_count && condition.color_count_options != None) {
      allowed_colors.push_back(condition.color);
      allowed_color_count.push_back(condition.color_count_options == One ? 1 : 2);
    }
  }
  Action action;
  if (allowed_colors.size() == 0)
    action.type = Position;
  else
    action.type = (ActionType)rand_range(0, 1);
  if (action.type == ColorPosition) {
    int colorIndex = rand_range(0, allowed_colors.size() - 1);
    action.color = allowed_colors[colorIndex];
    if (allowed_color_count[colorIndex] == 1)
      action.color_position_options = Only;
    else
      action.color_position_options = (ColorPositionOptions)rand_range(1, 2);
  } else
    action.position = rand_range(0, wires - 1);
  return action;
}

Rule create_random_rule(int wires, bool lastRule = false) {
  Rule rule;
  if (!lastRule) {
    rule.conditions.push_back(create_random_condition(wires));
    if (rng() % 100 < double_condition_chance * 100) rule.conditions.push_back(create_random_condition(wires));
  }
  rule.action = create_random_action(wires, rule.conditions);
  return rule;
}

bool matches(Wiring& wiring, Rule rule) {
  bool match = true;
  for (int i = 0; i < rule.conditions.size(); i++) {
    Condition condition = rule.conditions[i];
    if (condition.type == PositionColor)
      match &= wiring[condition.position] == condition.color;
    else {
      int count = 0;
      for (int j = 0; j < wiring.size(); j++)
        if (wiring[j] == condition.color) count++;
      if (condition.color_count_options == None)
        match &= count == 0;
      else if (condition.color_count_options == One)
        match &= count == 1;
      else
        match &= count > 1;
    }
  }
  return match;
}

bool is_redundant(Rules& rules, Rule rule) {
  for (int i = 0; i < rules.size(); i++)
    for (int j = 0; j < rules[i].conditions.size(); j++)
      for (int k = 0; k < rule.conditions.size(); k++)
        if (rules[i].conditions[j] == rule.conditions[k]) return true;
  return false;
}

std::map<int, Rules> generate_rules(int code) {
  rng = std::mt19937(code);
  std::map<int, Rules> rules;
  for (int i = MIN_WIRES; i <= MAX_WIRES; i++) {
    int rule_count = rand_range(MIN_RULES, MAX_RULES);
    while (rules[i].size() < rule_count) {
      Rule rule = create_random_rule(i, rules[i].size() == rule_count - 1);
      if (!is_redundant(rules[i], rule)) rules[i].push_back(rule);
    }
  }
  return rules;
}