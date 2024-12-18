#include "gf2d_menu.h"

int gf2d_draw_menu(int *menuType, GFC_Vector2D mousePos, int mousePress) {
    GFC_Rect resume;
    GFC_Rect exit;

    //Background
    gf2d_draw_rect_filled(gfc_rect(0, 0, 1600, 800), gfc_color8(90, 90, 90, 200));

    //Menu Box
    gf2d_draw_rect_filled(gfc_rect(350, 40, 600, 650), gfc_color8(45, 45, 45, 255));
    gf2d_draw_rect(gfc_rect(350, 40, 600, 650), GFC_COLOR_RED);

    //Menu Title
    gf2d_draw_rect_filled(gfc_rect(352, 75, 200, 50), gfc_color8(200, 0, 0, 255));
    gf2d_font_draw_text_wrap_tag("Menu", FT_Large, GFC_COLOR_WHITE, gfc_rect(350, 75, 200, 200));

    //Resume Box
    resume = gfc_rect(352, 175, 200, 50);
    if (gfc_point_in_rect(mousePos, resume)) {
        gf2d_draw_rect_filled(gfc_rect(352, 175, 200, 50), GFC_COLOR_ORANGE);
        if (mousePress == 1) {
            menuType = -1;
            slog("resume game");
            return -1;
        }
    }
    else {
        gf2d_draw_rect_filled(gfc_rect(352, 175, 200, 50), gfc_color8(200, 0, 0, 255));
    }
    gf2d_font_draw_text_wrap_tag("Resume", FT_Large, GFC_COLOR_WHITE, gfc_rect(350, 175, 200, 200));
    

    //Beastiary
    gf2d_draw_rect_filled(gfc_rect(352, 275, 200, 50), gfc_color8(200, 0, 0, 255));
    gf2d_font_draw_text_wrap_tag("Beastiary", FT_Large, GFC_COLOR_WHITE, gfc_rect(350, 275, 200, 200));

    //Exit game
    exit = gfc_rect(352, 375, 200, 50);
    if (gfc_point_in_rect(mousePos, exit)) {
        gf2d_draw_rect_filled(exit, GFC_COLOR_ORANGE);
        if (mousePress == 1) {
            menuType = -2;
            slog("Exit game");
            return -2;
        }
    }
    else {
        gf2d_draw_rect_filled(exit, gfc_color8(200, 0, 0, 255));
    }
    gf2d_font_draw_text_wrap_tag("Exit", FT_Large, GFC_COLOR_WHITE, gfc_rect(350, 375, 200, 200));
    return 0;
}