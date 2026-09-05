// ============================================================
//                 HATEU POD - FINAL V3 (Servo Edition)
// ============================================================

$fn = 48;

// ============================================================
// MAIN POD DIMENSIONS
// ============================================================

BOX_L = 90;
BOX_W = 70;
BOX_H = 50;

WALL = 3;
BOTTOM = 3;

CORNER_RADIUS = 7;

// ============================================================
// LID
// ============================================================

LID_TOP_THICKNESS = 3;
LID_LIP_HEIGHT = 5;
LID_LIP_THICKNESS = 2;
LID_CLEARANCE = 0.35;

// ============================================================
// TFT DISPLAY
// ============================================================

DISPLAY_L = 45;
DISPLAY_H = 30;
DISPLAY_BOTTOM = 11;

// ============================================================
// SERVO HOLE
// ============================================================
// Standard SG90 servo body is ~23 x 12.2 mm. 
// Added slight clearance.
SERVO_HOLE_L = 23.5;
SERVO_HOLE_W = 12.5;

// ============================================================
// USB CABLE HOLE
// ============================================================
USB_HOLE_W = 12;
USB_HOLE_H = 7;

// ============================================================
// CORNER MOUNTING BOSSES
// ============================================================

BOSS_RADIUS = 3.5;
BOSS_HEIGHT = 7;
BOSS_HOLE_RADIUS = 1.6;
BOSS_OFFSET = 8;

// ============================================================
// ROUNDED BOX
// ============================================================

module rounded_box(L, W, H, R) {
    // E is the edge rounding (fillet) for top/bottom edges. 
    // It makes the box look elegant without ruining wall thickness.
    E = min(2.5, H/2.001);
    
    // Ensure R is at least E to prevent scaling issues
    R_eff = max(R, E); 
    
    hull() {
        for(x = [R_eff, L-R_eff]) {
            for(y = [R_eff, W-R_eff]) {
                for(z = [E, H-E]) {
                    translate([x, y, z]) scale([R_eff/E, R_eff/E, 1]) sphere(r=E);
                }
            }
        }
    }
}

// ============================================================
// INTERNAL HOLLOW SPACE
// ============================================================

module inner_space() {
    translate([WALL, WALL, BOTTOM]) {
        rounded_box(BOX_L - 2*WALL, BOX_W - 2*WALL, BOX_H + 5, CORNER_RADIUS - WALL);
    }
}

// ============================================================
// TFT DISPLAY OPENING
// ============================================================

module display_opening() {
    translate([(BOX_L - DISPLAY_L)/2, -1, DISPLAY_BOTTOM]) {
        cube([DISPLAY_L, WALL + 2, DISPLAY_H]);
    }
}

// ============================================================
// SERVO SIDE HOLES
// ============================================================

module side_servo_holes() {
    // Cutout on Left and Right sides near the front display (Y=0) and bottom (Z=0)
    // Placed at Y=10 to clear the rounded corners
    
    // Left side hole
    translate([-1, 10, BOTTOM]) {
        cube([WALL + 2, SERVO_HOLE_L, SERVO_HOLE_W]);
    }
    
    // Right side hole
    translate([BOX_L - WALL - 1, 10, BOTTOM]) {
        cube([WALL + 2, SERVO_HOLE_L, SERVO_HOLE_W]);
    }
}

// ============================================================
// USB CABLE HOLE
// ============================================================

module usb_hole() {
    // Cutout on the back wall (Y = BOX_W) near the left corner (X=10) at bottom (Z=BOTTOM)
    translate([10, BOX_W - WALL - 1, BOTTOM]) {
        cube([USB_HOLE_W, WALL + 2, USB_HOLE_H]);
    }
}

// ============================================================
// MOUNTING BOSS
// ============================================================

module mounting_boss(x,y) {
    difference() {
        translate([x, y, BOTTOM]) cylinder(r=BOSS_RADIUS, h=BOSS_HEIGHT);
        translate([x, y, BOTTOM-1]) cylinder(r=BOSS_HOLE_RADIUS, h=BOSS_HEIGHT+2);
    }
}

// ============================================================
// FOUR CORNER BOSSES
// ============================================================

module mounting_bosses() {
    mounting_boss(BOSS_OFFSET, BOSS_OFFSET);
    mounting_boss(BOX_L-BOSS_OFFSET, BOSS_OFFSET);
    mounting_boss(BOSS_OFFSET, BOX_W-BOSS_OFFSET);
    mounting_boss(BOX_L-BOSS_OFFSET, BOX_W-BOSS_OFFSET);
}

// ============================================================
// FLAT TOP / BOTTOM ROUNDED BOXES
// ============================================================

module flat_top_rounded_box(L, W, H, R) {
    E = min(2.5, H/2.001);
    R_eff = max(R, E); 
    hull() {
        for(x = [R_eff, L-R_eff]) {
            for(y = [R_eff, W-R_eff]) {
                translate([x, y, E]) scale([R_eff/E, R_eff/E, 1]) sphere(r=E);
                translate([x, y, H - 0.01]) cylinder(r=R_eff, h=0.01);
            }
        }
    }
}

module flat_bottom_rounded_box(L, W, H, R) {
    E = min(2.5, H/2.001);
    R_eff = max(R, E); 
    hull() {
        for(x = [R_eff, L-R_eff]) {
            for(y = [R_eff, W-R_eff]) {
                translate([x, y, 0]) cylinder(r=R_eff, h=0.01);
                translate([x, y, H-E]) scale([R_eff/E, R_eff/E, 1]) sphere(r=E);
            }
        }
    }
}

// ============================================================
// MAIN POD BODY
// ============================================================

module main_pod() {
    difference() {
        // OUTER BODY (Flat top for flush lid fit)
        flat_top_rounded_box(BOX_L, BOX_W, BOX_H, CORNER_RADIUS);

        // OPEN INTERNAL AREA
        inner_space();

        // TFT
        display_opening();

        // SERVO HOLES
        side_servo_holes();

        // USB HOLE
        usb_hole();
    }
}

// ============================================================
// SEPARATE LID
// ============================================================

module rounded_prism(L, W, H, R) {
    R_eff = max(R, 0.001);
    linear_extrude(height = H) {
        hull() {
            translate([R_eff, R_eff]) circle(r=R_eff);
            translate([L-R_eff, R_eff]) circle(r=R_eff);
            translate([R_eff, W-R_eff]) circle(r=R_eff);
            translate([L-R_eff, W-R_eff]) circle(r=R_eff);
        }
    }
}

module lid() {
    // The top cover has a flush-fitting lip underneath.
    // The lip outer dimension exactly matches the inner space of the box minus clearance.
    difference() {
        union() {
            // TOP (Flat bottom for flush lip attachment and fit)
            flat_bottom_rounded_box(BOX_L, BOX_W, LID_TOP_THICKNESS, CORNER_RADIUS);

            // INNER LIP
            translate([WALL + LID_CLEARANCE, WALL + LID_CLEARANCE, -LID_LIP_HEIGHT]) {
                difference() {
                    // Outer lip boundary (fits into the hole)
                    rounded_prism(
                        BOX_L - 2*(WALL+LID_CLEARANCE),
                        BOX_W - 2*(WALL+LID_CLEARANCE),
                        LID_LIP_HEIGHT,
                        max(0, CORNER_RADIUS - WALL - LID_CLEARANCE)
                    );

                    // Inner lip hollow
                    translate([LID_LIP_THICKNESS, LID_LIP_THICKNESS, -1]) {
                        rounded_prism(
                            BOX_L - 2*(WALL+LID_CLEARANCE) - 2*LID_LIP_THICKNESS,
                            BOX_W - 2*(WALL+LID_CLEARANCE) - 2*LID_LIP_THICKNESS,
                            LID_LIP_HEIGHT+2,
                            max(0, CORNER_RADIUS - WALL - LID_CLEARANCE - LID_LIP_THICKNESS)
                        );
                    }
                }
            }
        }

        // 35mm x 13mm TOP MIDDLE HOLE
        translate([(BOX_L - 35)/2, (BOX_W - 13)/2, -1]) {
            cube([35, 13, LID_TOP_THICKNESS + 2]);
        }
    }
}

// ============================================================
// PRINT MODE
// ============================================================
// 1 = BODY ONLY
// 2 = LID ONLY
// 3 = BODY + LID
// ============================================================

PRINT_MODE = 3;

// ============================================================
// OUTPUT
// ============================================================

if(PRINT_MODE == 1) {
    main_pod();
}

if(PRINT_MODE == 2) {
    // Print flipped so flat top is on bed, lip points up
    translate([0, BOX_W, LID_TOP_THICKNESS]) rotate([180, 0, 0]) lid();
}

if(PRINT_MODE == 3) {
    main_pod();
    // Print flipped for support-free printing
    translate([BOX_L + 15, BOX_W, LID_TOP_THICKNESS]) rotate([180, 0, 0]) lid();
}

// ============================================================
//                       END
// ============================================================
