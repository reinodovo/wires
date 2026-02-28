#include <Arduino.h>
#include <average_analog.h>
#include <modules/puzzle_module.h>
#include <rules.h>

#include <vector>

const int RED_PIN = 26, GREEN_PIN = 27;
PuzzleModule module(StatusLight(RED_PIN, GREEN_PIN));

const int WIRES = 6;
const int WIRE_PINS[WIRES] = {33, 32, 35, 34, 39, 36};
AverageAnalogRead wire_readers[WIRES];

const int BASE_RESISTANCE = 10000;
const float ANALOG_MAX = 4095;
const float ANALOG_TOLERANCE = 0.07;
const int RESISTANCES[COLORS] = {0, 1000, 20000, 6000, 10000};

std::map<int, Rules> all_rules;
Wiring initial_wiring, last_stable_wiring, last_wiring;
int wire_to_cut;

int consecutive_stable_readings = 0;

bool compare(Wiring a, Wiring b) {
  for (int i = 0; i < WIRES; i++)
    if (a[i] != b[i]) return false;
  return true;
}

bool test_condition(Wiring wiring, Condition condition) {
  std::map<Color, int> color_count;
  Wiring filtered_wiring;
  for (auto wire : wiring) {
    if (wire != Empty) {
      color_count[wire]++;
      filtered_wiring.push_back(wire);
    }
  }
  if (condition.type == PositionColor)
    return filtered_wiring[condition.position] == condition.color;
  else {
    int count = color_count[condition.color];
    if (condition.color_count_options == None)
      return count == 0;
    else if (condition.color_count_options == One)
      return count == 1;
    else
      return count > 1;
  }
}

int find_wire_to_cut(Wiring wiring, Action action) {
  if (action.type == Position) {
    std::vector<int> positions;
    for (int i = 0; i < wiring.size(); i++)
      if (wiring[i] != Empty) positions.push_back(i);
    return positions[action.position];
  } else {
    std::vector<int> color_positions;
    for (int i = 0; i < wiring.size(); i++)
      if (wiring[i] == action.color) color_positions.push_back(i);
    if (action.color_position_options == Only)
      return color_positions[0];
    else if (action.color_position_options == First)
      return color_positions[0];
    else
      return color_positions[color_positions.size() - 1];
  }
}

int find_wire_to_cut(Wiring wiring, Rules rules) {
  for (auto rule : rules) {
    bool conditions_apply = true;
    for (auto condition : rule.conditions) conditions_apply &= test_condition(wiring, condition);
    if (conditions_apply) return find_wire_to_cut(wiring, rule.action);
  }
  return 0;
}

int wire_count(Wiring wiring) {
  int wires = 0;
  for (auto wire : wiring)
    if (wire != Empty) wires++;
  return wires;
}

Wiring wiring() {
  Wiring wiring(WIRES);
  for (int i = 0; i < WIRES; i++) {
    wiring[i] = Empty;
    float v_out = wire_readers[i].value() / ANALOG_MAX;
    for (int j = 0; j < COLORS; j++) {
      float v_out_expected = BASE_RESISTANCE / float(BASE_RESISTANCE + RESISTANCES[j]);
      if (abs(v_out - v_out_expected) < ANALOG_TOLERANCE) {
        wiring[i] = (Color)j;
        break;
      }
    }
  }
  return wiring;
}

void start() {
  last_wiring = last_stable_wiring = initial_wiring = wiring();
  int wires = wire_count(initial_wiring);
  wire_to_cut = find_wire_to_cut(initial_wiring, all_rules[wires]);
}

void restart() { consecutive_stable_readings = 0; }

void on_manual_code(int code) { all_rules = generate_rules(code); }

void setup() {
  module.on_start(start);
  module.on_reset(restart);
  module.on_manual_code(on_manual_code);

  if (!module.setup()) ESP.restart();

  for (int i = 0; i < WIRES; i++) {
    pinMode(WIRE_PINS[i], INPUT);
    wire_readers[i] = AverageAnalogRead(WIRE_PINS[i]);
  }
}

void loop() {
  module.update();
  for (int i = 0; i < WIRES; i++) wire_readers[i].update();
  if (module.get_state() != PuzzleModuleState::Started) return;
  auto currentWiring = wiring();
  if (compare(currentWiring, last_wiring))
    consecutive_stable_readings++;
  else
    consecutive_stable_readings = 0;
  last_wiring = currentWiring;
  if (consecutive_stable_readings < SAMPLES) return;
  for (int i = 0; i < WIRES; i++) {
    if (currentWiring[i] == Empty && last_stable_wiring[i] != Empty) {
      if (i == wire_to_cut)
        module.solve();
      else
        module.strike();
    }
  }
  last_stable_wiring = currentWiring;
}