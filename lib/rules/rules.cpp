#include "rules.h"

#include <random>

const int MIN_RULES = 4, MAX_RULES = 5;
const float double_condition_chance = 0.1;

int rand_range(int min, int max, std::mt19937& rng) { return rng() % (max - min + 1) + min; }

Condition create_random_condition(int wires, std::mt19937& rng) {
  Condition condition;
  condition.type = (ConditionType)rand_range(0, 1, rng);
  condition.color = (Color)rand_range(0, COLORS - 1, rng);
  if (condition.type == PositionColor)
    condition.position = rand_range(0, wires - 1, rng);
  else
    condition.color_count_options = (ColorCountOptions)rand_range(0, 2, rng);
  return condition;
}

Action create_random_action(int wires, std::vector<Condition> conditions, std::mt19937& rng) {
  std::vector<Color> allowed_colors;
  std::vector<int> allowed_color_count;
  for (auto condition : conditions) {
    if (condition.type == ColorCount && condition.color_count_options != None) {
      allowed_colors.push_back(condition.color);
      allowed_color_count.push_back(condition.color_count_options == One ? 1 : 2);
    }
  }
  Action action;
  if (allowed_colors.size() == 0)
    action.type = Position;
  else
    action.type = (ActionType)rand_range(0, 1, rng);
  if (action.type == ColorPosition) {
    int colorIndex = rand_range(0, allowed_colors.size() - 1, rng);
    action.color = allowed_colors[colorIndex];
    if (allowed_color_count[colorIndex] == 1)
      action.color_position_options = Only;
    else
      action.color_position_options = (ColorPositionOptions)rand_range(1, 2, rng);
  } else
    action.position = rand_range(0, wires - 1, rng);
  return action;
}

Rule create_random_rule(int wires, bool lastRule, std::mt19937& rng) {
  Rule rule;
  if (!lastRule) {
    rule.conditions.push_back(create_random_condition(wires, rng));
    if (rng() % 100 < double_condition_chance * 100) rule.conditions.push_back(create_random_condition(wires, rng));
  }
  rule.action = create_random_action(wires, rule.conditions, rng);
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
  std::mt19937 rng(code);
  std::map<int, Rules> rules;
  for (int i = MIN_WIRES; i <= MAX_WIRES; i++) {
    int rule_count = rand_range(MIN_RULES, MAX_RULES, rng);
    while (rules[i].size() < rule_count) {
      Rule rule = create_random_rule(i, rules[i].size() == rule_count - 1, rng);
      if (!is_redundant(rules[i], rule)) rules[i].push_back(rule);
    }
  }
  return rules;
}