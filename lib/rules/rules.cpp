#include "rules.h"
#include <random>

std::mt19937 rng;

const int MIN_RULES = 4, MAX_RULES = 5;
const float doubleConditionChance = 0.1;

int randRange(int min, int max) { return rng() % (max - min + 1) + min; }

Condition createRandomCondition(int wires) {
  Condition condition;
  condition.type = (ConditionType)randRange(0, 1);
  condition.color = (Color)randRange(0, COLORS - 1);
  if (condition.type == PositionColor)
    condition.position = randRange(0, wires - 1);
  else
    condition.colorCountOptions = (ColorCountOptions)randRange(0, 2);
  return condition;
}

Action createRandomAction(int wires, std::vector<Condition> conditions) {
  std::vector<Color> allowedColors;
  std::vector<int> allowedColorCount;
  for (auto condition : conditions) {
    if (condition.type == ColorCount && condition.colorCountOptions != None) {
      allowedColors.push_back(condition.color);
      allowedColorCount.push_back(condition.colorCountOptions == One ? 1 : 2);
    }
  }
  Action action;
  if (allowedColors.size() == 0)
    action.type = Position;
  else
    action.type = (ActionType)randRange(0, 1);
  if (action.type == ColorPosition) {
    int colorIndex = randRange(0, allowedColors.size() - 1);
    action.color = allowedColors[colorIndex];
    if (allowedColorCount[colorIndex] == 1)
      action.colorPositionOptions = Only;
    else
      action.colorPositionOptions = (ColorPositionOptions)randRange(1, 2);
  } else
    action.position = randRange(0, wires - 1);
  return action;
}

Rule createRandomRule(int wires, bool lastRule = false) {
  Rule rule;
  if (!lastRule) {
    rule.conditions.push_back(createRandomCondition(wires));
    if (rng() % 100 < doubleConditionChance * 100)
      rule.conditions.push_back(createRandomCondition(wires));
  }
  rule.action = createRandomAction(wires, rule.conditions);
  return rule;
}

bool matches(Wiring &wiring, Rule rule) {
  bool match = true;
  for (int i = 0; i < rule.conditions.size(); i++) {
    Condition condition = rule.conditions[i];
    if (condition.type == PositionColor)
      match &= wiring[condition.position] == condition.color;
    else {
      int count = 0;
      for (int j = 0; j < wiring.size(); j++)
        if (wiring[j] == condition.color)
          count++;
      if (condition.colorCountOptions == None)
        match &= count == 0;
      else if (condition.colorCountOptions == One)
        match &= count == 1;
      else
        match &= count > 1;
    }
  }
  return match;
}

bool isRedundant(Rules &rules, Rule rule) {
  for (int i = 0; i < rules.size(); i++)
    for (int j = 0; j < rules[i].conditions.size(); j++)
      for (int k = 0; k < rule.conditions.size(); k++)
        if (rules[i].conditions[j] == rule.conditions[k])
          return true;
  return false;
}

std::map<int, Rules> generateRules(int code) {
  rng = std::mt19937(code);
  std::map<int, Rules> rules;
  for (int i = MIN_WIRES; i <= MAX_WIRES; i++) {
    int ruleCount = randRange(MIN_RULES, MAX_RULES);
    while (rules[i].size() < ruleCount) {
      Rule rule = createRandomRule(i, rules[i].size() == ruleCount - 1);
      if (!isRedundant(rules[i], rule))
        rules[i].push_back(rule);
    }
  }
  return rules;
}