
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>


int main() {
 

    //++++++++++++++ Variaveis do jogo

    const int largura_t = 1040;
    const int altura_t = 980;

    float pos_x =100;
    float pos_y = 100;


    bool fim = false; 
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;



    //__________________________

  

    //++++++++++++ Inicializacao do ellegro e display 
    ALLEGRO_DISPLAY* display = NULL;
    if (!al_init()) {
        al_show_native_message_box(NULL, "erro!", "Erro ao inicializar o allegro", NULL);
     }
    display = al_create_display(largura_t, altura_t);

    if (!display) {
        al_show_native_message_box(NULL, "erro!", "Erro ao inicializar o allegro", NULL);

    }
  

    //_____________________________________________

       //++++++++++++++++++++ addons e intalacao

    al_install_keyboard();
    al_init_image_addon();
    //___________________________________
    ALLEGRO_BITMAP* rudanBaixo = al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo.png");


    //++++++ fila e demais dispositivos 

    fila_eventos = al_create_event_queue();
    al_init_primitives_addon();
    //_________________________


    // +++++++++++++++++++ registros de sources

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    //_______________________

    //++++++++++++++++++ loop principal

    while (!fim)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

                //++++++ eventos e logoca do jogo 
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                fim = true;
            
            }

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                pos_y -= 10;
                break;
            case ALLEGRO_KEY_DOWN:
                    pos_y += 10;
                    break;
            case ALLEGRO_KEY_RIGHT:
                pos_x += 10;
                break; 
            case ALLEGRO_KEY_LEFT:
                pos_x -= 10;
                break;

            }
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            fim = true;
        }

        // Desenho do jogo
        al_draw_filled_rectangle(pos_x, pos_y, pos_x + 30, pos_y + 30, al_map_rgb(255, 255, 0));
        al_draw_bitmap(rudanBaixo, pos_x, pos_y, 0);
        al_flip_display();

        al_clear_to_color(al_map_rgb(0,0,0));
    }




    //+++++++++ finalizacao do programa 

    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

