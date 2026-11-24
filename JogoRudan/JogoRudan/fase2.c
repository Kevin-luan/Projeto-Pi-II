#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "fase.h"

bool fim = false;


// Posicao do personagem
float pos_x = 500.0f;
float pos_y = 800.0f;

// ini migo 

#define FOOTMAN_FRAMES 8     // frames de cada sprite sheet (lado a lado)
#define INI_ESCALA 0.2f      // tamanho na tela (o frame original tem 288x312)
#define INI_VEL_ANIM 8       // quanto maior, mais lenta a animacao

#define MAX_INIMIGOS 30      // maximo de inimigos ao mesmo tempo

#define INI_PASTA "assets/sprites/spritesRudan/royal_legion_lite_free_footman/animations/kng_footman/"

enum DIR_INIMIGO { INI_BAIXO, INI_CIMA, INI_DIREITA, INI_ESQUERDA };

// Mesma ordem do enum acima
static const char* ini_arquivos[4] = {
    INI_PASTA "down/kng_footman_down_idle_sheet.png",
    INI_PASTA "up/kng_footman_up_idle_sheet.png",
    INI_PASTA "right/kng_footman_right_idle_sheet.png",
    INI_PASTA "left/kng_footman_left_idle_sheet.png"
};

typedef struct {
    float x;
    float y;
    float vel;
    bool vivo;
    int dir;
    int frame;
    int contador;
} inimigo;

inimigo inimigos[MAX_INIMIGOS];

int total_inimigos = 0;   // quantos inimigos tem na onda atual
int onda_atual = 0;       // numero da onda atual

ALLEGRO_BITMAP* ini_sheet[4] = { NULL, NULL, NULL, NULL };

// Tamanho da caixa de colisao do inimigo (ajuste se precisar)
#define INI_HIT_LARG 36.0f
#define INI_HIT_ALT 52.0f

// Retorna o indice do primeiro inimigo vivo que encosta no
// retangulo (x, y, w, h), ou -1 se nenhum encostar
int colideComInimigo(float x, float y, float w, float h)
{
    for (int i = 0; i < total_inimigos; i++) {

        if (!inimigos[i].vivo) continue;

        if (x < inimigos[i].x + INI_HIT_LARG / 2.0f &&
            x + w > inimigos[i].x - INI_HIT_LARG / 2.0f &&
            y < inimigos[i].y + INI_HIT_ALT / 2.0f &&
            y + h > inimigos[i].y - INI_HIT_ALT / 2.0f) {

            return i;
        }
    }

    return -1;
}

// Retorna true se todos os inimigos da onda atual morreram
bool todosInimigosMortos(void)
{
    for (int i = 0; i < total_inimigos; i++) {
        if (inimigos[i].vivo) return false;
    }

    return true;
}

// Cria uma nova onda com "quantidade" inimigos em lugares aleatorios,
// sem nascer muito perto do jogador
void gerarOnda(int quantidade, int largura_t, int altura_t,
    float alvo_x, float alvo_y)
{
    if (quantidade > MAX_INIMIGOS) quantidade = MAX_INIMIGOS;

    total_inimigos = quantidade;

    for (int i = 0; i < quantidade; i++) {

        float x, y, dx, dy;
        int tentativas = 0;

        do {
            x = 30.0f + (float)(rand() % (largura_t - 60));
            y = 30.0f + (float)(rand() % (altura_t - 60));

            dx = x - alvo_x;
            dy = y - alvo_y;

            tentativas++;

        } while (sqrtf(dx * dx + dy * dy) < 250.0f && tentativas < 50);

        inimigos[i].x = x;
        inimigos[i].y = y;
        inimigos[i].vel = 1.5f + (rand() % 11) / 10.0f;   // entre 1.5 e 2.5
        inimigos[i].vivo = true;
        inimigos[i].dir = INI_BAIXO;
        inimigos[i].frame = rand() % FOOTMAN_FRAMES;
        inimigos[i].contador = 0;
    }
}

void liberarInimigo(void)
{
    for (int i = 0; i < 4; i++) {
        if (ini_sheet[i]) {
            al_destroy_bitmap(ini_sheet[i]);
            ini_sheet[i] = NULL;
        }
    }
}

bool carregarInimigo(void)
{
    srand((unsigned int)time(NULL));

    onda_atual = 0;
    total_inimigos = 0;

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i].vivo = false;
    }

    for (int i = 0; i < 4; i++) {
        ini_sheet[i] = al_load_bitmap(ini_arquivos[i]);
    }

    // Se nao existir a pasta "up", usa o sprite de baixo
    if (!ini_sheet[INI_CIMA] && ini_sheet[INI_BAIXO]) {
        ini_sheet[INI_CIMA] = al_clone_bitmap(ini_sheet[INI_BAIXO]);
    }

    for (int i = 0; i < 4; i++) {
        if (!ini_sheet[i]) {
            printf("Erro ao carregar: %s\n", ini_arquivos[i]);
            liberarInimigo();
            return false;
        }
    }

    return true;
}

void atualizarInimigo(float alvo_x, float alvo_y)

{

    for (int i = 0; i < total_inimigos; i++) {

        if (!inimigos[i].vivo) continue;

        float dx = alvo_x - inimigos[i].x;
        float dy = alvo_y - inimigos[i].y;
        float distancia = sqrtf(dx * dx + dy * dy);
        // Os inimigos perseguem o jogador
        if (distancia > 5.0f)
        {
            inimigos[i].x += (dx / distancia) * inimigos[i].vel;
            inimigos[i].y += (dy / distancia) * inimigos[i].vel;

            // Vira para a direcao em que esta andando
            if (fabsf(dx) > fabsf(dy))
                inimigos[i].dir = (dx > 0) ? INI_DIREITA : INI_ESQUERDA;
            else
                inimigos[i].dir = (dy > 0) ? INI_BAIXO : INI_CIMA;

        }

        // Animacao dos frames
        inimigos[i].contador++;

        if (inimigos[i].contador >= INI_VEL_ANIM) {
            inimigos[i].contador = 0;
            inimigos[i].frame = (inimigos[i].frame + 1) % FOOTMAN_FRAMES;
        }

    }

}

// Desenha os inimigos

void desenharInimigo()
{

    for (int i = 0; i < total_inimigos; i++) {

        if (!inimigos[i].vivo) continue;

        ALLEGRO_BITMAP* sheet = ini_sheet[inimigos[i].dir];

        if (!sheet) continue;

        float fw = (float)al_get_bitmap_width(sheet) / FOOTMAN_FRAMES;
        float fh = (float)al_get_bitmap_height(sheet);

        float dw = fw * INI_ESCALA;
        float dh = fh * INI_ESCALA;

        al_draw_scaled_bitmap(
            sheet,
            inimigos[i].frame * fw, 0, fw, fh,
            inimigos[i].x - dw / 2.0f, inimigos[i].y - dh / 2.0f, dw, dh,
            0
        );

    }

}

enum TECLAS {

    CIMA, BAIXO, DIREITA, ESQUERDA, FOGO

};

enum DIRECAO {

    DIRECAO_BAIXO,

    DIRECAO_CIMA,

    DIRECAO_DIREITA,

    DIRECAO_ESQUERDA

};

void fase2(void)

{

    const int largura_t = 1920;


    const int altura_t = 1200;

    const int FPS = 60;

    const float velocidade = 5.0f;

    const float velocidade_tiro = 10.0f;

    bool teclas[5] = { false, false, false, false, false };

    enum DIRECAO direcao_atual = DIRECAO_BAIXO;

    int frame_atual = 0;

    int contador_animacao = 0;

    const int velocidade_animacao = 10;

    float fogo_x = 0.0f;

    float fogo_y = 0.0f;

    float fogo_dx = 0.0f;

    float fogo_dy = 0.0f;

    const float fogo_raio = 5.0f;

    bool fogo_ativo = false;

    ALLEGRO_DISPLAY* display = NULL;

    ALLEGRO_TIMER* timer = NULL;

    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

    if (!al_init())

        return;

    if (!al_install_keyboard())

        return;

    if (!al_init_image_addon())

        return;

    if (!al_init_primitives_addon())

        return;

    display = al_create_display(largura_t, altura_t);

    if (!display)

        return;

    timer = al_create_timer(1.0 / FPS);

    if (!timer) {

        al_destroy_display(display);

        return;

    }

    fila_eventos = al_create_event_queue();

    if (!fila_eventos) {

        al_destroy_timer(timer);

        al_destroy_display(display);

        return;

    }

    // Sprites do personagem

    ALLEGRO_BITMAP* rudanBaixo1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo1.png");

    ALLEGRO_BITMAP* rudanBaixo2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo2.png");

    ALLEGRO_BITMAP* rudanCima1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanCima1.png");

    ALLEGRO_BITMAP* rudanCima2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanCima2.png");

    ALLEGRO_BITMAP* rudanDireita1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanDireita1.png");

    ALLEGRO_BITMAP* rudanDireita2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanDireita2.png");

    ALLEGRO_BITMAP* rudanEsquerda1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda1.png");

    ALLEGRO_BITMAP* rudanEsquerda2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda2.png");

    // Verifica se todos os sprites foram carregados

    if (!rudanBaixo1 || !rudanBaixo2 ||

        !rudanCima1 || !rudanCima2 ||

        !rudanDireita1 || !rudanDireita2 ||

        !rudanEsquerda1 || !rudanEsquerda2) {

        if (rudanBaixo1) al_destroy_bitmap(rudanBaixo1);

        if (rudanBaixo2) al_destroy_bitmap(rudanBaixo2);

        if (rudanCima1) al_destroy_bitmap(rudanCima1);

        if (rudanCima2) al_destroy_bitmap(rudanCima2);

        if (rudanDireita1) al_destroy_bitmap(rudanDireita1);

        if (rudanDireita2) al_destroy_bitmap(rudanDireita2);

        if (rudanEsquerda1) al_destroy_bitmap(rudanEsquerda1);

        if (rudanEsquerda2) al_destroy_bitmap(rudanEsquerda2);

        al_destroy_event_queue(fila_eventos);

        al_destroy_timer(timer);

        al_destroy_display(display);

        return;

    }

    int largura_personagem = al_get_bitmap_width(rudanBaixo1);

    int altura_personagem = al_get_bitmap_height(rudanBaixo1);

    // Carrega os sprites do inimigo

    if (!carregarInimigo()) {

        al_destroy_bitmap(rudanBaixo1);

        al_destroy_bitmap(rudanBaixo2);

        al_destroy_bitmap(rudanCima1);

        al_destroy_bitmap(rudanCima2);

        al_destroy_bitmap(rudanDireita1);

        al_destroy_bitmap(rudanDireita2);

        al_destroy_bitmap(rudanEsquerda1);

        al_destroy_bitmap(rudanEsquerda2);

        al_destroy_event_queue(fila_eventos);

        al_destroy_timer(timer);

        al_destroy_display(display);

        return;

    }

    // Registra os eventos

    al_register_event_source(

        fila_eventos, al_get_keyboard_event_source());

    al_register_event_source(

        fila_eventos, al_get_display_event_source(display));

    al_register_event_source(

        fila_eventos, al_get_timer_event_source(timer));

    al_start_timer(timer);


    while (!fim) {

        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);

        // Teclas pressionadas

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_ESCAPE:

                fim = true;

                break;

            case ALLEGRO_KEY_UP:

                teclas[CIMA] = true;

                direcao_atual = DIRECAO_CIMA;

                break;

            case ALLEGRO_KEY_DOWN:

                teclas[BAIXO] = true;

                direcao_atual = DIRECAO_BAIXO;

                break;

            case ALLEGRO_KEY_RIGHT:

                teclas[DIREITA] = true;

                direcao_atual = DIRECAO_DIREITA;

                break;

            case ALLEGRO_KEY_LEFT:

                teclas[ESQUERDA] = true;

                direcao_atual = DIRECAO_ESQUERDA;

                break;

            case ALLEGRO_KEY_D:

                teclas[FOGO] = true;

                if (!fogo_ativo) {

                    fogo_ativo = true;

                    if (direcao_atual == DIRECAO_CIMA) {

                        fogo_x = pos_x + 20;

                        fogo_y = pos_y;

                        fogo_dx = 0;

                        fogo_dy = -velocidade_tiro;

                    }

                    else if (direcao_atual == DIRECAO_BAIXO) {

                        fogo_x = pos_x + 20;

                        fogo_y = pos_y + 40;

                        fogo_dx = 0;

                        fogo_dy = velocidade_tiro;

                    }

                    else if (direcao_atual == DIRECAO_DIREITA) {

                        fogo_x = pos_x + 40;

                        fogo_y = pos_y + 20;

                        fogo_dx = velocidade_tiro;

                        fogo_dy = 0;

                    }

                    else if (direcao_atual == DIRECAO_ESQUERDA) {

                        fogo_x = pos_x;

                        fogo_y = pos_y + 20;

                        fogo_dx = -velocidade_tiro;

                        fogo_dy = 0;

                    }

                }

                break;

            }

        }

        // Teclas soltas

        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {

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

        // Fechar janela

        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

            fim = true;

        }

        // Atualizacao do jogo

        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            // Movimento do personagem

            if (teclas[CIMA]) pos_y -= velocidade;

            if (teclas[BAIXO]) pos_y += velocidade;

            if (teclas[ESQUERDA]) pos_x -= velocidade;

            if (teclas[DIREITA]) pos_x += velocidade;

            // Limites da tela

            if (pos_x < 0) pos_x = 0;

            if (pos_y < 0) pos_y = 0;

            if (pos_x + largura_personagem > largura_t)

                pos_x = largura_t - largura_personagem;

            if (pos_y + altura_personagem > altura_t)

                pos_y = altura_t - altura_personagem;

            // Animacao

            bool andando =

                teclas[CIMA] || teclas[BAIXO] ||

                teclas[DIREITA] || teclas[ESQUERDA];

            if (andando) {

                contador_animacao++;

                if (contador_animacao >= velocidade_animacao) {

                    contador_animacao = 0;

                    frame_atual = (frame_atual + 1) % 2;

                }

            }

            else {

                frame_atual = 0;

                contador_animacao = 0;

            }

            // Movimento do tiro

            if (fogo_ativo) {

                fogo_x += fogo_dx;

                fogo_y += fogo_dy;

                if (fogo_x + fogo_raio < 0 ||

                    fogo_x - fogo_raio > largura_t ||

                    fogo_y + fogo_raio < 0 ||

                    fogo_y - fogo_raio > altura_t) {

                    fogo_ativo = false;

                }

            }

            // Atualiza a posicao dos inimigos a cada timer

            atualizarInimigo(pos_x + largura_personagem / 2.0f,
                pos_y + altura_personagem / 2.0f);

            // Colisao do tiro com os inimigos

            if (fogo_ativo) {

                int atingido = colideComInimigo(
                    fogo_x - fogo_raio, fogo_y - fogo_raio,
                    fogo_raio * 2, fogo_raio * 2);

                if (atingido >= 0) {

                    inimigos[atingido].vivo = false;

                    fogo_ativo = false;

                }

            }

            // Colisao dos inimigos com o personagem

            if (colideComInimigo(pos_x, pos_y,
                largura_personagem, altura_personagem) >= 0) {

                fim = true;

            }

            // Quando todos morrem, vem a proxima onda (1, 2, 3, ...)

            if (todosInimigosMortos()) {

                onda_atual++;

                gerarOnda(onda_atual, largura_t, altura_t,
                    pos_x + largura_personagem / 2.0f,
                    pos_y + altura_personagem / 2.0f);

            }



            al_clear_to_color(al_map_rgb(20, 50, 70));

            ALLEGRO_BITMAP* sprite_atual = NULL;

            if (direcao_atual == DIRECAO_BAIXO)

                sprite_atual = frame_atual == 0

                ? rudanBaixo1 : rudanBaixo2;

            else if (direcao_atual == DIRECAO_CIMA)

                sprite_atual = frame_atual == 0

                ? rudanCima1 : rudanCima2;

            else if (direcao_atual == DIRECAO_DIREITA)

                sprite_atual = frame_atual == 0

                ? rudanDireita1 : rudanDireita2;

            else if (direcao_atual == DIRECAO_ESQUERDA)

                sprite_atual = frame_atual == 0

                ? rudanEsquerda1 : rudanEsquerda2;

            // Desenha o personagem

            al_draw_bitmap(sprite_atual, pos_x, pos_y, 0);

            // Desenha o tiro

            if (fogo_ativo) {

                al_draw_filled_circle(

                    fogo_x, fogo_y, fogo_raio,

                    al_map_rgb(255, 0, 0));

            }

            // Desenha os inimigos na posicao atualizada

            desenharInimigo();

            desenharInimigo();

            al_flip_display();

        }

    }

    // Libera os sprites

    al_destroy_bitmap(rudanBaixo1);

    al_destroy_bitmap(rudanBaixo2);

    al_destroy_bitmap(rudanCima1);

    al_destroy_bitmap(rudanCima2);

    al_destroy_bitmap(rudanDireita1);

    al_destroy_bitmap(rudanDireita2);

    al_destroy_bitmap(rudanEsquerda1);

    al_destroy_bitmap(rudanEsquerda2);

    liberarInimigo();

    al_destroy_event_queue(fila_eventos);

    al_destroy_timer(timer);

    al_destroy_display(display);

}