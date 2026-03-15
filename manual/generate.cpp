#include <manual.h>
#include <rules.h>

std::string color_to_string(Color color) {
  switch (color) {
    case White:
      return "white";
    case Black:
      return "black";
    case Blue:
      return "blue";
    case Red:
      return "red";
    case Yellow:
      return "yellow";
  }
  return "";
}

std::string position_to_string(int position, int wires) {
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

std::string condition_to_string(Condition condition, int wires) {
  std::string condition_str = "";
  if (condition.type == color_count) {
    condition_str += "there";
    if (condition.color_count_options == None)
      condition_str += " are no";
    else if (condition.color_count_options == One)
      condition_str += " is exactly one";
    else
      condition_str += " is more than one";
    condition_str += " " + color_to_string(condition.color) + " wire";
    if (condition.color_count_options == None) condition_str += "s";
  } else {
    condition_str +=
        "the " + position_to_string(condition.position, wires) + " wire is " + color_to_string(condition.color);
  }
  return condition_str;
}

std::string action_to_string(Action action, int wires) {
  std::string action_str = "";
  if (action.type == ColorPosition) {
    if (action.color_position_options == Only)
      action_str += "the " + color_to_string(action.color) + " wire";
    else if (action.color_position_options == First)
      action_str += "the first " + color_to_string(action.color) + " wire";
    else if (action.color_position_options == Last)
      action_str += "the last " + color_to_string(action.color) + " wire";
  } else
    action_str += "the " + position_to_string(action.position, wires) + " wire";
  return action_str;
}

manual::json generate_json_for_code(uint16_t code) {
  std::map<int, Rules> all_rules = generate_rules(code);

  manual::json data = manual::init(MODULE_NAME, MODULE_NAME,
                                   "Wires are the lifeblood of electronics! Wait, no, electricity is the "
                                   "lifeblood. Wires are more like the arteries. The veins? No matter…",
                                   APP_VERSION);

  data["all_rules"] = manual::json::array();
  for (int i = MIN_WIRES; i <= MAX_WIRES; i++) {
    Rules rules = all_rules[i];
    manual::json rules_json = manual::json::object();
    rules_json["wire_count"] = i;
    rules_json["rules"] = manual::json::array();
    rules_json["size"] = rules.size();
    for (auto rule : rules) {
      manual::json rule_json = manual::json::object();
      rule_json["conditions"] = manual::json::array();
      for (auto condition : rule.conditions) {
        rule_json["conditions"].push_back(condition_to_string(condition, i));
      }
      rule_json["action"] = action_to_string(rule.action, i);
      rules_json["rules"].push_back(rule_json);
    }
    data["all_rules"].push_back(rules_json);
  }

  return data;
}

int main(int argc, char** argv) { manual::run(argc, argv, generate_json_for_code); }