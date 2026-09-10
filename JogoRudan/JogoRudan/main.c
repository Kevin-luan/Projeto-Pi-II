#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro.h>

int main() {
	al_init();
    al_init_image_addon();

	ALLEGRO_DISPLAY *tela = al_create_display(900, 600);

    if (tela == NULL)
    {
        printf("Erro ao criar a janela!\n");
        return 1;
    }


    // +++++ FILA DE ENVENTOS ++++
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(
        fila,
        al_get_display_event_source(tela)
    );

    ALLEGRO_BITMAP* personagem = al_load_bitmap("assets/sprites/spritesRudan/rudanCima.png");
    if (personagem == NULL) {
        printf("erro ao carregar o personagem");
        al_destroy_event_queue(fila);
        al_destroy_display(tela);
        return 1;

    }



    // ++++++++ loop do jogo +++++
    bool executando = true;
    while (executando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila, &evento);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_draw_bitmap_region(personagem, 0, 0, 113, 135, 100, 100, 0);

        al_flip_display();

        // Se clicar no X da janela
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            executando = false;
        }


    }


    // 6. Liberar memória
    al_destroy_event_queue(fila);
    al_destroy_display(tela);

    

}