#include <Arduino.h>
#include <average_analog.h>
#include <puzzle_module.h>
#include <rules.h>
#include <vector>

const int RED_PIN = 26;
const int GREEN_PIN = 27;

const int WIRES = 6;
const int WIRE_PINS[WIRES] = {33, 32, 35, 34, 39, 36};
AverageAnalogRead wireReaders[WIRES];

const int BASE_RESISTANCE = 10000;
const float ANALOG_MAX = 4095;
const float ANALOG_TOLERANCE = 0.05;
const int RESISTANCES[COLORS] = {0, 1000, 3000, 6000, 10000};

std::map<int, Rules> allRules;
Wiring initialWiring, lastStableWiring, lastWiring;
int wireToCut;

int consecutiveStableReadings = 0;

bool compare(Wiring a, Wiring b) {
  for (int i = 0; i < WIRES; i++)
    if (a[i] != b[i])
      return false;
  return true;
}

bool testCondition(Wiring wiring, Condition condition) {
  std::map<Color, int> colorCount;
  Wiring filteredWiring;
  for (auto wire : wiring) {
    if (wire != Empty) {
      colorCount[wire]++;
      filteredWiring.push_back(wire);
    }
  }
  if (condition.type == PositionColor)
    return filteredWiring[condition.position] == condition.color;
  else {
    int count = colorCount[condition.color];
    if (condition.colorCountOptions == None)
      return count == 0;
    else if (condition.colorCountOptions == One)
      return count == 1;
    else
      return count > 1;
  }
}

int findWireToCut(Wiring wiring, Action action) {
  if (action.type == Position) {
    std::vector<int> positions;
    for (int i = 0; i < wiring.size(); i++)
      if (wiring[i] != Empty)
        positions.push_back(i);
    return positions[action.position];
  } else {
    std::vector<int> colorPositions;
    for (int i = 0; i < wiring.size(); i++)
      if (wiring[i] == action.color)
        colorPositions.push_back(i);
    if (action.colorPositionOptions == Only)
      return colorPositions[0];
    else if (action.colorPositionOptions == First)
      return colorPositions[0];
    else
      return colorPositions[colorPositions.size() - 1];
  }
}

int findWireToCut(Wiring wiring, Rules rules) {
  for (auto rule : rules) {
    bool conditionsApply = true;
    for (auto condition : rule.conditions)
      conditionsApply &= testCondition(wiring, condition);
    if (conditionsApply)
      return findWireToCut(wiring, rule.action);
  }
  return 0;
}

int wireCount(Wiring wiring) {
  int wires = 0;
  for (auto wire : wiring)
    if (wire != Empty)
      wires++;
  return wires;
}

Wiring wiring() {
  Wiring wiring(WIRES);
  for (int i = 0; i < WIRES; i++) {
    wiring[i] = Empty;
    float vOut = wireReaders[i].value() / ANALOG_MAX;
    for (int j = 0; j < COLORS; j++) {
      float vOutExpected =
          BASE_RESISTANCE / float(BASE_RESISTANCE + RESISTANCES[j]);
      if (abs(vOut - vOutExpected) < ANALOG_TOLERANCE) {
        wiring[i] = (Color)j;
        break;
      }
    }
  }
  return wiring;
}

void onStart() {
  lastWiring = lastStableWiring = initialWiring = wiring();
  int wires = wireCount(initialWiring);
  wireToCut = findWireToCut(initialWiring, allRules[wires]);
}

void onRestart() { consecutiveStableReadings = 0; }

void onManualCode(int code) { allRules = generateRules(code); }

void setup() {
  PuzzleModule::onStart = onStart;
  PuzzleModule::onRestart = onRestart;
  PuzzleModule::onManualCode = onManualCode;

  if (!PuzzleModule::setup(PuzzleModule::StatusLight(RED_PIN, GREEN_PIN)))
    ESP.restart();

  for (int i = 0; i < WIRES; i++) {
    pinMode(WIRE_PINS[i], INPUT);
    wireReaders[i] = AverageAnalogRead(WIRE_PINS[i]);
  }
}

void loop() {
  PuzzleModule::update();
  for (int i = 0; i < WIRES; i++)
    wireReaders[i].update();
  if (PuzzleModule::status() != PuzzleModule::ModuleStatus::Started)
    return;
  auto currentWiring = wiring();
  if (compare(currentWiring, lastWiring))
    consecutiveStableReadings++;
  else
    consecutiveStableReadings = 0;
  lastWiring = currentWiring;
  if (consecutiveStableReadings < SAMPLES)
    return;
  for (int i = 0; i < WIRES; i++) {
    if (currentWiring[i] == Empty && lastStableWiring[i] != Empty) {
      if (i == wireToCut)
        PuzzleModule::solve();
      else
        PuzzleModule::strike();
    }
  }
  lastStableWiring = currentWiring;
}