#include <manual.h>
#include <rules.h>

std::string colorToString(Color color) {
  switch (color) {
  case White:
    return "white";
  case Blue:
    return "blue";
  case Red:
    return "red";
  case Purple:
    return "purple";
  case Brown:
    return "brown";
  }
  return "";
}

std::string positionToString(int position, int wires) {
  if (position == 0)
    return "first";
  else if (position == wires - 1)
    return "last";
  else if (position == 1)
    return "second";
  else if (position == 2)
    return "third";
  else if (position == 3)
    return "fourth";
  else if (position == 4)
    return "fifth";
  return "";
}

std::string conditionToString(Condition condition, int wires) {
  std::string condition_str = "";
  if (condition.type == ColorCount) {
    condition_str += "there";
    if (condition.colorCountOptions == None)
      condition_str += " are no";
    else if (condition.colorCountOptions == One)
      condition_str += " is exactly one";
    else
      condition_str += " is more than one";
    condition_str += " " + colorToString(condition.color) + " wire";
    if (condition.colorCountOptions == None)
      condition_str += "s";
  } else {
    condition_str += "the " + positionToString(condition.position, wires) +
                     " wire is " + colorToString(condition.color);
  }
  return condition_str;
}

std::string actionToString(Action action, int wires) {
  std::string action_str = "";
  if (action.type == ColorPosition) {
    if (action.colorPositionOptions == Only)
      action_str += "the " + colorToString(action.color) + " wire";
    else if (action.colorPositionOptions == First)
      action_str += "the first " + colorToString(action.color) + " wire";
    else if (action.colorPositionOptions == Last)
      action_str += "the last " + colorToString(action.color) + " wire";
  } else
    action_str += "the " + positionToString(action.position, wires) + " wire";
  return action_str;
}

int main(int argc, char **argv) {
  int code = atoi(argv[1]);
  std::map<int, Rules> allRules = generateRules(code);

  manual::json data = manual::init(
      "Wires", "Wires",
      "Wires are the lifeblood of electronics! Wait, no, electricity is the "
      "lifeblood. Wires are more like the arteries. The veins? No matter…",
      APP_VERSION);

  data["all_rules"] = manual::json::array();
  for (int i = MIN_WIRES; i <= MAX_WIRES; i++) {
    Rules rules = allRules[i];
    manual::json rules_json = manual::json::object();
    rules_json["wire_count"] = i;
    rules_json["rules"] = manual::json::array();
    rules_json["size"] = rules.size();
    for (auto rule : rules) {
      manual::json rule_json = manual::json::object();
      rule_json["conditions"] = manual::json::array();
      for (auto condition : rule.conditions) {
        rule_json["conditions"].push_back(conditionToString(condition, i));
      }
      rule_json["action"] = actionToString(rule.action, i);
      rules_json["rules"].push_back(rule_json);
    }
    data["all_rules"].push_back(rules_json);
  }

  manual::save(data, code);
}