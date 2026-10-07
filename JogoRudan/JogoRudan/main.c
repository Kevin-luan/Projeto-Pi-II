#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

enum TECLAS { CIMA, BAIXO, DIREITA, ESQUERDA, FOGO };

int main() {

    // Variáveis do jogo
    const int largura_t = 1040;
    const int altura_t = 980;

    float pos_x = 100;
    float pos_y = 100;

    // Variáveis do tiro
    float fogo_x = 0;
    float fogo_y = 0;
    bool fogo_ativo = false;

    bool fim = false;
    int FPS = 60;
    
    

    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

    bool teclas[] = { false, false, false, false, false };


    // Inicialização do Allegro
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_TIMER* timer = NULL;


    if (!al_init()) {
        al_show_native_message_box(
            NULL,
            "erro!",
            "Erro ao inicializar o allegro",
            NULL
        );
        return 1;
    }

    display = al_create_display(largura_t, altura_t);

    if (!display) {
        al_show_native_message_box(
            NULL,
            "erro!",
            "Erro ao criar o display",
            NULL
        );
        return 1;
    }


    // Addons e dispositivos
    al_install_keyboard();
    al_init_image_addon();
    al_init_primitives_addon();
    timer = al_create_timer(1.0 / FPS);
   
   


    // Sprites
    ALLEGRO_BITMAP* rudanBaixo1 =
        al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo1.png");

    ALLEGRO_BITMAP* rudanBaixo2 =
        al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo2.png");

    ALLEGRO_BITMAP* rudanCima1 =
        al_load_bitmap("assets/sprites/spritesRudan/rudanCima1.png");

    ALLEGRO_BITMAP* rudanCima2 =
        al_load_bitmap("assets/sprites/spritesRudan/rudanCima2.png");




    // Fila de eventos
    fila_eventos = al_create_event_queue();

    al_register_event_source(fila_eventos,al_get_keyboard_event_source());

    al_register_event_source(fila_eventos,al_get_display_event_source(display));

    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    al_start_timer(timer);
    // Loop principal
    while (!fim) {

        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);


        // Tecla pressionada
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                fim = true;
            }

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_UP:
                teclas[CIMA] = true;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = true;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = true;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = true;
                break;

            case ALLEGRO_KEY_D:

                teclas[FOGO] = true;

                // Só cria um novo tiro se não houver
                // outro tiro ativo
                if (!fogo_ativo) {

                    fogo_x = pos_x;
                    fogo_y = pos_y;

                    fogo_ativo = true;
                }

                break;
            }
        }


        // Janela fechada
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

            fim = true;
        }


        // Tecla solta
        if (ev.type == ALLEGRO_EVENT_KEY_UP) {

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_UP:
                teclas[CIMA] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = false;
                break;

            case ALLEGRO_KEY_D:
                teclas[FOGO] = false;
                break;
            }
        }


        // Movimento do jogador
        pos_y -= teclas[CIMA] * 5;
        pos_y += teclas[BAIXO] * 5;

        pos_x -= teclas[ESQUERDA] * 5;
        pos_x += teclas[DIREITA] * 5;


        // Movimento do tiro
        if (fogo_ativo) {

            fogo_x += 10;


            // Se sair da tela, desativa o tiro
            if (fogo_x > largura_t) {
                fogo_ativo = false;
            }
        }

        
            // Limpa a tela
            al_clear_to_color(al_map_rgb(0, 0, 0));


            // Desenha o jogador
            al_draw_bitmap(
                rudanBaixo1,
                pos_x,
                pos_y,
                0
            );


            // Desenha o tiro
            if (fogo_ativo) {

                al_draw_filled_circle(fogo_x,fogo_y,5,
                al_map_rgb(255, 0, 0)
                );
            }



        

     

        // Mostra tudo na tela
        al_flip_display();
    }


    // Finalização
    al_destroy_bitmap(rudanBaixo1);
    al_destroy_bitmap(rudanBaixo2);
    al_destroy_bitmap(rudanCima1);
    al_destroy_bitmap(rudanCima2);

    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);

    return 0;
}