// ═══════════════════════════════════════════════════════════════════════════
// Svitrix DIY Enclosure - Parametric Design
// Para matriz WS2812B 32×8 + ESP32-S3-DevKitC-1
//
// Autor: XE1E
// Versión: 1.0
// Fecha: 2026-06-05
//
// Uso:
//   1. Ajustar parámetros según tu panel LED
//   2. Renderizar (F6) y exportar STL
//   3. Imprimir base y tapa por separado
// ═══════════════════════════════════════════════════════════════════════════

// ─── PARÁMETROS DEL PANEL LED ───
// Cambiar estos valores según tu panel:
//   - Pitch 10mm (estándar): panel 320×80mm
//   - Pitch 8mm (rígido):    panel 256×64mm
//   - Pitch 5mm (mini):      panel 160×40mm

led_pitch      = 10;        // mm entre LEDs (5, 8, o 10)
led_cols       = 32;        // columnas
led_rows       = 8;         // filas
panel_thick    = 2;         // grosor del PCB del panel

// Dimensiones calculadas del panel
panel_width    = led_pitch * led_cols;  // 320mm para pitch 10
panel_height   = led_pitch * led_rows;  // 80mm para pitch 10

// ─── PARÁMETROS DEL GABINETE ───
wall           = 2;         // grosor de pared
clearance      = 1;         // holgura alrededor del panel
depth          = 25;        // profundidad total interna
corner_r       = 3;         // radio de esquinas redondeadas

// Dimensiones calculadas del gabinete
inner_w        = panel_width + clearance * 2;
inner_h        = panel_height + clearance * 2;
outer_w        = inner_w + wall * 2;
outer_h        = inner_h + wall * 2;
outer_d        = depth + wall;

// ─── PARÁMETROS DE COMPONENTES ───
// DevKitC-1
devkit_w       = 26;        // ancho DevKitC-1
devkit_l       = 70;        // largo DevKitC-1

// Puerto USB-C
usb_w          = 10;        // ancho apertura USB-C
usb_h          = 4;         // alto apertura USB-C

// Botones
btn_d          = 6.5;       // diámetro agujero botón (6mm + holgura)
btn_spacing    = 15;        // espacio entre centros de botones
btn_count      = 4;         // número de botones

// Sensores
ldr_d          = 5;         // diámetro agujero LDR

// Ventilación
vent_w         = 30;        // ancho total zona ventilación
vent_h         = 2;         // alto ranura ventilación
vent_count     = 5;         // número de ranuras

// ─── PARÁMETROS DE MONTAJE ───
screw_d        = 3.2;       // diámetro tornillo M3
screw_head_d   = 6;         // diámetro cabeza tornillo
screw_depth    = 8;         // profundidad del inserto/tuerca

// ═══════════════════════════════════════════════════════════════════════════
// MÓDULOS AUXILIARES
// ═══════════════════════════════════════════════════════════════════════════

module rounded_box(w, h, d, r) {
    // Caja con esquinas redondeadas
    hull() {
        for (x = [r, w - r])
            for (y = [r, h - r])
                translate([x, y, 0])
                    cylinder(r = r, h = d, $fn = 32);
    }
}

module screw_post(d, h, screw_d) {
    // Poste para tornillo
    difference() {
        cylinder(d = d, h = h, $fn = 24);
        translate([0, 0, -0.1])
            cylinder(d = screw_d, h = h + 0.2, $fn = 16);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// BASE DEL GABINETE
// ═══════════════════════════════════════════════════════════════════════════

module enclosure_bottom() {
    difference() {
        // Caja exterior con esquinas redondeadas
        rounded_box(outer_w, outer_h, outer_d, corner_r);

        // ─── Cavidad interior ───
        translate([wall, wall, wall])
            cube([inner_w, inner_h, depth + 1]);

        // ─── Ventana para LEDs (frente) ───
        translate([wall + clearance, wall + clearance, -1])
            cube([panel_width, panel_height, wall + 2]);

        // ─── Puerto USB-C (lateral derecho) ───
        translate([outer_w - wall - 1, outer_h/2 - usb_w/2, depth - usb_h - 5])
            cube([wall + 2, usb_w, usb_h + 6]);

        // ─── Agujeros para botones (trasero) ───
        for (i = [0 : btn_count - 1]) {
            translate([wall + 20 + i * btn_spacing, -1, depth/2])
                rotate([-90, 0, 0])
                    cylinder(d = btn_d, h = wall + 2, $fn = 24);
        }

        // ─── Agujero LDR (superior, cerca del frente) ───
        translate([wall + 15, outer_h/2, outer_d - wall - 1])
            cylinder(d = ldr_d, h = wall + 2, $fn = 24);

        // ─── Ranuras de ventilación (trasero) ───
        for (i = [0 : vent_count - 1]) {
            translate([outer_w - 60 + i * 12, -1, depth/2 - vent_h/2])
                cube([vent_w / vent_count * 0.6, wall + 2, vent_h]);
        }

        // ─── Ranuras de ventilación (inferior) ───
        vent_bottom_count = floor((outer_w - 80) / 45);
        for (i = [0 : vent_bottom_count]) {
            translate([40 + i * 45, wall + 10, -1])
                cube([30, 3, wall + 2]);
        }

        // ─── Agujeros para tornillos de tapa (esquinas) ───
        for (x = [wall + 8, outer_w - wall - 8])
            for (y = [wall + 8, outer_h - wall - 8])
                translate([x, y, outer_d - screw_depth])
                    cylinder(d = screw_d, h = screw_depth + 1, $fn = 16);
    }

    // ─── Soportes para panel LED (4 esquinas) ───
    support_h = 5;
    for (x = [wall + 5, outer_w - wall - 8])
        for (y = [wall + 5, outer_h - wall - 8])
            translate([x, y, wall])
                cube([3, 3, support_h]);

    // ─── Soporte/carril para DevKit ───
    devkit_x = outer_w/2 - devkit_w/2 - 2;
    devkit_y = wall + panel_height/2 - devkit_l/2;
    translate([devkit_x, devkit_y, wall])
        difference() {
            cube([devkit_w + 4, devkit_l + 4, 8]);
            translate([2, 2, 2])
                cube([devkit_w, devkit_l, 10]);
        }

    // ─── Postes para tornillos ───
    post_d = 8;
    post_h = depth - 3;
    for (x = [wall + 8, outer_w - wall - 8])
        for (y = [wall + 8, outer_h - wall - 8])
            translate([x, y, wall])
                screw_post(post_d, post_h, screw_d);
}

// ═══════════════════════════════════════════════════════════════════════════
// TAPA DEL GABINETE
// ═══════════════════════════════════════════════════════════════════════════

module enclosure_top() {
    lip_h = 3;      // altura del labio de encaje
    lip_clearance = 0.3;

    difference() {
        union() {
            // Tapa principal
            rounded_box(outer_w, outer_h, wall, corner_r);

            // Labio de encaje (va dentro de la base)
            translate([wall + lip_clearance, wall + lip_clearance, wall])
                cube([inner_w - lip_clearance*2, inner_h - lip_clearance*2, lip_h]);
        }

        // ─── Ventana para difusor ───
        // Dejamos marco de 2mm para soporte del difusor
        translate([wall + clearance + 2, wall + clearance + 2, -1])
            cube([panel_width - 4, panel_height - 4, wall + 2]);

        // ─── Rebaje para difusor (opcional) ───
        // translate([wall + clearance, wall + clearance, wall - 1.5])
        //     cube([panel_width, panel_height, 1.5]);

        // ─── Agujeros para tornillos ───
        for (x = [wall + 8, outer_w - wall - 8])
            for (y = [wall + 8, outer_h - wall - 8])
                translate([x, y, -1]) {
                    // Agujero pasante
                    cylinder(d = screw_d, h = wall + lip_h + 2, $fn = 16);
                    // Avellanado para cabeza
                    cylinder(d = screw_head_d, h = 2, $fn = 24);
                }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// DIFUSOR (OPCIONAL)
// ═══════════════════════════════════════════════════════════════════════════

module diffuser() {
    // Lámina difusora que encaja en la tapa
    // Imprimir en PLA blanco o usar acrílico opalino
    diffuser_thick = 1.5;

    translate([wall + clearance, wall + clearance, 0])
        cube([panel_width, panel_height, diffuser_thick]);
}

// ═══════════════════════════════════════════════════════════════════════════
// RENDER - DESCOMENTAR LA PIEZA DESEADA
// ═══════════════════════════════════════════════════════════════════════════

// ─── Opción 1: Ver base sola ───
enclosure_bottom();

// ─── Opción 2: Ver tapa sola (para exportar STL) ───
// enclosure_top();

// ─── Opción 3: Ver tapa desplazada (para visualizar conjunto) ───
// translate([0, outer_h + 20, 0])
//     enclosure_top();

// ─── Opción 4: Ver ensamblaje completo ───
// enclosure_bottom();
// translate([0, 0, outer_d + 5])
//     enclosure_top();

// ─── Opción 5: Difusor solo ───
// diffuser();

// ═══════════════════════════════════════════════════════════════════════════
// INFORMACIÓN DE IMPRESIÓN
// ═══════════════════════════════════════════════════════════════════════════
//
// CONFIGURACIÓN RECOMENDADA:
// - Material: PLA o PETG
// - Altura de capa: 0.2mm
// - Perímetros: 3
// - Relleno: 20% (Grid o Gyroid)
// - Soportes: No necesarios
// - Brim: 5mm recomendado para piezas grandes
//
// TEMPERATURAS:
// - PLA: Hotend 200-210°C, Cama 60°C
// - PETG: Hotend 230-245°C, Cama 80°C
//
// ORIENTACIÓN:
// - Base: Imprimir con la ventana de LEDs hacia arriba
// - Tapa: Imprimir con la cara exterior hacia abajo (superficie lisa)
//
// TIEMPO ESTIMADO (200mm/s):
// - Base: 4-6 horas
// - Tapa: 1-2 horas
//
// FILAMENTO ESTIMADO:
// - Base: 60-80g
// - Tapa: 20-30g
// - Total: ~100g
//
// POST-PROCESADO:
// - Limpiar rebabas de los agujeros
// - Verificar que los tornillos M3 entren suavemente
// - Opcional: Lijar cara exterior de tapa para mejor acabado
//
// ═══════════════════════════════════════════════════════════════════════════
