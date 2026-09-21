#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main() {

    if (!al_init()) {
        printf("Erro ao inicializar a Allegro.\n");
        return 1;
    }

    al_install_keyboard();
    al_init_image_addon();

    ALLEGRO_DISPLAY* tela = al_create_display(900, 600);

    if (tela == NULL) {
        printf("Erro ao criar a tela.\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();

    al_register_event_source(
        fila,
        al_get_display_event_source(tela)
    );

    al_register_event_source(
        fila,
        al_get_keyboard_event_source()
    );

    ALLEGRO_BITMAP* personagem = al_load_bitmap("assets/sprites/spritesRudan/rudanCima.png");

    if (personagem == NULL) {
        printf("Erro ao carregar personagem.png\n");
        return 1;
    }

    bool rodando = true;

    // Posição do personagem
    float x = 100;
    float y = 100;

    // Velocidade
    float velocidade = 5;

    // Teclas pressionadas
    bool teclas[ALLEGRO_KEY_MAX] = { false };

    while (rodando) {

        ALLEGRO_EVENT evento;

        al_wait_for_event(fila, &evento);

        // Fechar janela
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }

        // Tecla pressionada
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            teclas[evento.keyboard.keycode] = true;
        }

        // Tecla solta
        if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            teclas[evento.keyboard.keycode] = false;
        }

        // Movimento
        if (teclas[ALLEGRO_KEY_W]) {
            y -= velocidade;
        }

        if (teclas[ALLEGRO_KEY_S]) {
            y += velocidade;
        }

        if (teclas[ALLEGRO_KEY_A]) {
            x -= velocidade;
        }

        if (teclas[ALLEGRO_KEY_D]) {
            x += velocidade;
        }

        // Limpa a tela
        al_clear_to_color(al_map_rgb(30, 30, 30));

        // Desenha o personagem
        al_draw_bitmap(personagem, x, y, 0);

        // Atualiza a tela
        al_flip_display();
    }

    al_destroy_bitmap(personagem);
    al_destroy_event_queue(fila);
    al_destroy_display(tela);

    al_shutdown_image_addon();

    return 0;
}
