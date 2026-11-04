
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "fase.h"



void menu(void) {
    const int largura_t = 1920;
    const int altura_t = 1200;
    const int FPS = 60;

    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
    ALLEGRO_BITMAP* img_de_fundo = NULL;

    // Inicialização
    if (!al_init()) {
        return;
    }

    // Inicializa imagens
    al_init_image_addon();
    al_init_primitives_addon();

    // Teclado
    if (!al_install_keyboard()) {
        return;
    }

    // Display
    display = al_create_display(largura_t, altura_t);

    if (!display) {
        return;
    }

    timer = al_create_timer(1.0 / FPS);

    if (!timer) {
        al_destroy_display(display);
        return;
    }

    // Carrega imagem de fundo
    img_de_fundo = al_load_bitmap(
        "assets/sprites/spritesRudan/Plano_de_fundo_menu.png"
    );

    if (!img_de_fundo) {
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    // Cria fila de eventos
    fila_eventos = al_create_event_queue();

    if (!fila_eventos) {
        al_destroy_bitmap(img_de_fundo);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    // Registra eventos
    al_register_event_source(
        fila_eventos, al_get_keyboard_event_source()
    );

    al_register_event_source(
        fila_eventos, al_get_display_event_source(display)
    );

    al_register_event_source(
        fila_eventos, al_get_timer_event_source(timer)
    );

    al_start_timer(timer);

    ALLEGRO_FONT* fonte = al_create_builtin_font();


    bool fim = false;

    while (!fim) {
        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fim = true;
        }

        al_draw_scaled_bitmap(img_de_fundo,0, 0,al_get_bitmap_width(img_de_fundo),al_get_bitmap_height(img_de_fundo),0, 0,largura_t,altura_t,0);
        al_draw_filled_rectangle(0,1100,largura_t,altura_t, al_map_rgba(150, 2, 0,0));
        al_draw_textf(fonte,al_map_rgb(255, 255, 255),100, 1150,0,"APERTE L PARA COMEÇAR");

       
       

        if (ev.keyboard.keycode == ALLEGRO_KEY_L) {
            fim = true;
        }
        al_flip_display();
    }

    // Libera recursos
    al_destroy_bitmap(img_de_fundo);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_display(display);
    fase1();
}
