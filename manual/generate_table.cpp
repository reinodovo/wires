#include <rules.h>
#include <string>

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

void print(Condition condition, int wires) {
  if (condition.type == ColorCount) {
    printf("there");
    if (condition.colorCountOptions == None)
      printf(" are no");
    else if (condition.colorCountOptions == One)
      printf(" is exactly one");
    else
      printf(" is more than one");
    printf(" %s wire", colorToString(condition.color).c_str());
    if (condition.colorCountOptions == None)
      printf("s");
  } else {
    printf("the %s wire is %s",
           positionToString(condition.position, wires).c_str(),
           colorToString(condition.color).c_str());
  }
}

void print(Action action, int wires) {
  if (action.type == ColorPosition) {
    if (action.colorPositionOptions == Only)
      printf("the %s wire", colorToString(action.color).c_str());
    else if (action.colorPositionOptions == First)
      printf("the first %s wire", colorToString(action.color).c_str());
    else if (action.colorPositionOptions == Last)
      printf("the last %s wire", colorToString(action.color).c_str());
  } else {
    printf("the %s wire", positionToString(action.position, wires).c_str());
  }
}

int main(int argc, char **argv) {
  freopen("./manual/table.html", "w", stdout);
  int seed = atoi(argv[1]);
  std::map<int, Rules> allRules = generateRules(seed);
  for (int i = MIN_WIRES; i <= MAX_WIRES; i++) {
    printf("<tr><td>");
    Rules rules = allRules[i];
    printf("<span style=\"font-weight: bold;font-style: "
           "normal;text-decoration: underline;\">%d Wires:</span>",
           i);
    for (int j = 0; j < rules.size(); j++) {
      printf("<br><span>");
      if (j == 0)
        printf("If ");
      else if (j == rules.size() - 1)
        printf("Otherwise");
      else
        printf("Otherwise, if ");

      Rule rule = rules[j];
      for (int k = 0; k < rule.conditions.size(); k++) {
        Condition condition = rule.conditions[k];
        if (k > 0)
          printf(" and ");
        print(condition, i);
      }
      printf(", cut ");
      print(rule.action, i);
      printf(".");
      printf("</span>");
    }
    printf("</td></tr>");
  }
  return 0;
}