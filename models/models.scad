include <../../ktane-3d-models/variables.scad>;
use <../../ktane-3d-models/bomb_base.scad>;

$fn = $preview ? 10 : 50;

tolerance = 0.3;

wires = 6;
wire_radius = 4.65 / 2;
wire_distance_bottom = 15;
wire_distance_side = 15;
wire_gap_left = (size - 2 * wire_distance_side) / (wires - 1);
wire_gap_right = 9.5;
wire_support_thickness = 9;
wire_support_height = 7.5;

module top() {
    difference() {
        union() {
            bomb_module_top(height_above_pcb = 14, module_type = 1);
            translate([wire_distance_side, size / 2, -wire_support_height / 2 - wall_thickness]) cube([wire_support_thickness, 6 * wire_gap_left, wire_support_height], center = true);
            translate([size - wire_distance_side, wire_distance_bottom + 5 / 2 * wire_gap_right, -wire_support_height / 2 - wall_thickness]) cube([wire_support_thickness, 6 * wire_gap_right, wire_support_height], center = true);
        }
        for (i = [0:wires - 1]) {
            translate([wire_distance_side, wire_distance_bottom + i * wire_gap_left])
                cylinder(h = 100, r1 = wire_radius + tolerance / 2, r2 = wire_radius + tolerance / 2, center = true);
            translate([size - wire_distance_side, wire_distance_bottom + i * wire_gap_right])
                cylinder(h = 100, r1 = wire_radius + tolerance / 2, r2 = wire_radius + tolerance / 2, center = true);
        }
    }
}

module bottom() {
    bomb_module_bottom(height_above_pcb = 14);
}

color("darkgray") top();
color("darkgray") bottom();
