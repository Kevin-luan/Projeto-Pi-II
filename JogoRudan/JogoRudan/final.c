#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "fase.h"

// Declaracoes do que esta definido no arquivo da fase2
#define MAX_INIMIGOS 30

enum TECLAS {

    CIMA, BAIXO, DIREITA, ESQUERDA, FOGO

};

enum DIRECAO {

    DIRECAO_BAIXO,

    DIRECAO_CIMA,

    DIRECAO_DIREITA,

    DIRECAO_ESQUERDA

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

extern inimigo inimigos[MAX_INIMIGOS];
extern int total_inimigos;
extern int onda_atual;

int colideComInimigo(float x, float y, float w, float h);
bool todosInimigosMortos(void);
void gerarOnda(int quantidade, int largura_t, int altura_t,
    float alvo_x, float alvo_y);
void liberarInimigo(void);
bool carregarInimigo(void);
void atualizarInimigo(float alvo_x, float alvo_y);
void desenharInimigo();

// Desenha um texto centralizado em "cx", ampliado pela "escala"
// (a fonte padrao do Allegro e pequena, entao ela e ampliada)
static void desenharTextoGrande(ALLEGRO_FONT* f, ALLEGRO_COLOR cor,
    float cx, float y, float escala, const char* txt)
{
    int w = al_get_text_width(f, txt);
    int h = al_get_font_line_height(f);

    if (w <= 0 || h <= 0) return;

    ALLEGRO_BITMAP* destino = al_get_target_bitmap();
    ALLEGRO_BITMAP* tmp = al_create_bitmap(w, h);

    if (!tmp) return;

    al_set_target_bitmap(tmp);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_text(f, cor, 0, 0, 0, txt);

    al_set_target_bitmap(destino);
    al_draw_scaled_bitmap(tmp, 0, 0, w, h,
        cx - (w * escala) / 2.0f, y, w * escala, h * escala, 0);

    al_destroy_bitmap(tmp);
}

// Escreve um paragrafo com quebra de linha automatica, centralizado em "cx".
// "largura_max" e a largura maxima de cada linha (ja ampliada).
// Use \n no texto para pular de linha (\n\n deixa uma linha em branco).
// Devolve o "y" onde o texto terminou.
static float desenharParagrafo(ALLEGRO_FONT* f, ALLEGRO_COLOR cor,
    float cx, float y, float escala, float largura_max,
    float espacamento, const char* texto)
{
    char linha[256] = "";
    char teste[320];
    char palavra[64];
    const char* p = texto;

    while (*p) {

        // Pula os espacos
        while (*p == ' ') p++;

        if (!*p) break;

        // Le uma palavra
        int n = 0;

        while (*p && *p != ' ' && *p != '\n' && n < 63) {
            palavra[n++] = *p++;
        }

        palavra[n] = '\0';

        // Testa se a palavra cabe na linha atual
        if (linha[0])
            snprintf(teste, sizeof(teste), "%s %s", linha, palavra);
        else
            snprintf(teste, sizeof(teste), "%s", palavra);

        if (linha[0] && al_get_text_width(f, teste) * escala > largura_max) {

            desenharTextoGrande(f, cor, cx, y, escala, linha);
            y += espacamento;

            snprintf(linha, sizeof(linha), "%s", palavra);
        }
        else {

            snprintf(linha, sizeof(linha), "%s", teste);
        }

        // Quebra de linha manual
        if (*p == '\n') {

            desenharTextoGrande(f, cor, cx, y, escala, linha);
            y += espacamento;

            linha[0] = '\0';
            p++;
        }
    }

    if (linha[0]) {

        desenharTextoGrande(f, cor, cx, y, escala, linha);
        y += espacamento;
    }

    return y;
}


void final(void)

{

    const int largura_t = 1920;

    const int altura_t = 1200;

    const int FPS = 60;

    const float velocidade = 5.0f;

    const float velocidade_tiro = 10.0f;

    // Configuracao das ondas (ajuste aqui a dificuldade)
    const int soldados_primeira_onda = 30;     // a primeira onda tem 2 soldados
    const int max_soldados_onda = 100;         // limite de soldados por onda
    const int intervalo_onda = 60;            // pausa entre ondas (60 frames = 1 segundo)
    const float vel_inimigo_base = 0.6f;      // velocidade inicial (bem lenta)
    const float vel_inimigo_por_onda = 0.06f; // quanto ficam mais rapidos a cada onda
    const float vel_inimigo_max = 1.8f;       // limite de velocidade dos soldados

    int pausa_onda = intervalo_onda;          // tempo ate a proxima onda comecar

    // Letreiro e musica da morte
    const char* texto_titulo = "FIM DE JOGO";                      // titulo do letreiro
    const char* texto_rodape = "APERTE ESC PARA SAIR";             // texto pequeno no fim
    const char* arquivo_musica = "assets/sprites/spritesRudan/musicaFinal.mp3"; // caminho da musica
    // ESCREVA SUA HISTORIA AQUI (quebra de linha automatica; use \n para pular linha)
    const char* texto_historia =
        "Mas um dia, o horizonte mudou.\n"
        "Grandes embarcacoes surgiram no mar. Homens desconhecidos chegaram trazendo armas, doencas e uma ambicao que jamais haviamos visto.\n"
        "No começo, pensamos que eram apenas visitantes. Nao imaginavamos que sua chegada mudaria nossas vidas para sempre.\n\n"

        "Eles nao vieram para conhecer nossa cultura. Vieram para tomar nossas terras.\n"
        "Nossas aldeias foram invadidas, nossas casas queimadas e nossos irmaos assassinados.\n"
        "Mulheres, criancas e idosos foram obrigados a fugir.\n"

        "Lutamos para proteger nossas familias e nossa cultura,\n"
        "mas muitos de nossos irmaos foram perdidos.\n"

        "Mesmo diante de tanta dor, fiz uma promessa:\n"
        "nossa historia nunca sera esquecida.\n"

        "NOS JA ESTAVAMOS AQUI.\n"
        "NOSSA HISTORIA CONTINUARA VIVA.";



    const float letreiro_larg = 1500.0f;
    const float letreiro_alt = 1000.0f;
    const float letreiro_vel = 6.0f;                          // velocidade da subida
    const float letreiro_y_final = (altura_t - letreiro_alt) / 2.0f;
    float letreiro_y = (float)altura_t;                       // comeca fora da tela (embaixo)

    // Posicao do personagem nesta fase (variaveis locais)
    float px = 900.0f;

    float py = 550.0f;

    bool terminou = false;

    bool morto = false;

    int contador_morte = 0;

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

    ALLEGRO_BITMAP* fundo = NULL;

    ALLEGRO_FONT* fonte = NULL;

    ALLEGRO_SAMPLE* musica = NULL;

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

    // Fonte do letreiro
    al_init_font_addon();

    fonte = al_create_builtin_font();

    // Inicialização do áudio
    if (!al_install_audio()) {
        printf("Erro ao inicializar o audio!\n");
    }

    if (!al_init_acodec_addon()) {
        printf("Erro ao inicializar os codecs de audio!\n");
    }

    if (!al_reserve_samples(2)) {
        printf("Erro ao reservar samples de audio!\n");
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

        if (fonte) al_destroy_font(fonte);

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

        if (fonte) al_destroy_font(fonte);

        al_destroy_event_queue(fila_eventos);

        al_destroy_timer(timer);

        al_destroy_display(display);

        return;

    }

    // fundo
    fundo = al_load_bitmap("assets/sprites/spritesRudan/planodeFundofase2.jpg");

    // Musica da morte (carregada agora para nao travar na hora de tocar)
    musica = al_load_sample(arquivo_musica);

    if (!musica) {
        printf("Erro ao carregar a musica: %s\n", arquivo_musica);
    }
    else {
        printf("Musica carregada com sucesso!\n");
    }

    // Comeca sem inimigos: a primeira onda e criada pelo loop abaixo
    onda_atual = 0;

    // Registra os eventos

    al_register_event_source(

        fila_eventos, al_get_keyboard_event_source());

    al_register_event_source(

        fila_eventos, al_get_display_event_source(display));

    al_register_event_source(

        fila_eventos, al_get_timer_event_source(timer));

    al_start_timer(timer);


    while (!terminou) {

        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);

        // Teclas pressionadas

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_ESCAPE:

                terminou = true;

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

                if (!fogo_ativo && !morto) {

                    fogo_ativo = true;

                    if (direcao_atual == DIRECAO_CIMA) {

                        fogo_x = px + 20;

                        fogo_y = py;

                        fogo_dx = 0;

                        fogo_dy = -velocidade_tiro;

                    }

                    else if (direcao_atual == DIRECAO_BAIXO) {

                        fogo_x = px + 20;

                        fogo_y = py + 40;

                        fogo_dx = 0;

                        fogo_dy = velocidade_tiro;

                    }

                    else if (direcao_atual == DIRECAO_DIREITA) {

                        fogo_x = px + 40;

                        fogo_y = py + 20;

                        fogo_dx = velocidade_tiro;

                        fogo_dy = 0;

                    }

                    else if (direcao_atual == DIRECAO_ESQUERDA) {

                        fogo_x = px;

                        fogo_y = py + 20;

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

            terminou = true;

        }

        // Atualizacao do jogo

        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            // Movimento do personagem (so enquanto estiver vivo)

            if (!morto) {

                if (teclas[CIMA]) py -= velocidade;

                if (teclas[BAIXO]) py += velocidade;

                if (teclas[ESQUERDA]) px -= velocidade;

                if (teclas[DIREITA]) px += velocidade;

            }

            // Limites da tela

            if (px < 0) px = 0;

            if (py < 0) py = 0;

            if (px + largura_personagem > largura_t)

                px = largura_t - largura_personagem;

            if (py + altura_personagem > altura_t)

                py = altura_t - altura_personagem;

            // Animacao

            bool andando =

                teclas[CIMA] || teclas[BAIXO] ||

                teclas[DIREITA] || teclas[ESQUERDA];

            if (andando && !morto) {

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

            // Os inimigos perseguem o personagem
            // (quando ele morre, eles param de perseguir)

            if (!morto) {

                atualizarInimigo(px + largura_personagem / 2.0f,
                    py + altura_personagem / 2.0f);

            }

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

            if (!morto && colideComInimigo(px, py,
                largura_personagem, altura_personagem) >= 0) {

                morto = true;

                fogo_ativo = false;

                // Toca a musica da morte (em loop, ate apertar ESC)
                if (musica) {

                    al_play_sample(musica, 1.0f, 0.0f, 1.0f,
                        ALLEGRO_PLAYMODE_LOOP, NULL);

                }

            }

            // Ondas infinitas: quando todos da onda morrem, vem a proxima
            // Onda 1 = 2 soldados, onda 2 = 3, ... ate o limite

            if (!morto && todosInimigosMortos()) {

                if (pausa_onda > 0) {

                    pausa_onda--;

                }

                else {

                    onda_atual++;

                    int quantidade = soldados_primeira_onda + (onda_atual - 1);

                    if (quantidade > max_soldados_onda)

                        quantidade = max_soldados_onda;

                    gerarOnda(quantidade, largura_t, altura_t,
                        px + largura_personagem / 2.0f,
                        py + altura_personagem / 2.0f);

                    // Soldados lentos, ficando um pouco mais rapidos a cada onda
                    for (int i = 0; i < total_inimigos; i++) {

                        float v = vel_inimigo_base
                            + (onda_atual - 1) * vel_inimigo_por_onda
                            + (rand() % 5) / 10.0f;

                        if (v > vel_inimigo_max) v = vel_inimigo_max;

                        inimigos[i].vel = v;

                    }

                    pausa_onda = intervalo_onda;

                }

            }

            // Depois de morrer: o letreiro sobe e fica na tela
            // (o jogo so fecha quando o jogador aperta ESC)

            if (morto) {

                contador_morte++;

                if (letreiro_y > letreiro_y_final) {

                    letreiro_y -= letreiro_vel;

                    if (letreiro_y < letreiro_y_final)

                        letreiro_y = letreiro_y_final;

                }

            }

            al_clear_to_color(al_map_rgb(20, 50, 70));

            // fundo
            if (fundo) {
                al_draw_scaled_bitmap(fundo,
                    0, 0,
                    al_get_bitmap_width(fundo), al_get_bitmap_height(fundo),
                    0, 0, largura_t, altura_t, 0);
            }

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

            // Desenha o personagem (some quando morre)

            if (!morto) {

                al_draw_bitmap(sprite_atual, px, py, 0);

            }

            // Desenha o tiro

            if (fogo_ativo) {

                al_draw_filled_circle(

                    fogo_x, fogo_y, fogo_raio,

                    al_map_rgb(255, 0, 0));

            }

            // Desenha os inimigos

            desenharInimigo();

            // Desenha o letreiro subindo (so depois que o personagem morre)

            if (morto) {

                float lx = (largura_t - letreiro_larg) / 2.0f;

                // Escurece a tela
                al_draw_filled_rectangle(0, 0, largura_t, altura_t,
                    al_map_rgba(0, 0, 0, 120));

                // Fundo e borda do letreiro
                al_draw_filled_rectangle(lx, letreiro_y,
                    lx + letreiro_larg, letreiro_y + letreiro_alt,
                    al_map_rgb(40, 25, 15));

                al_draw_rectangle(lx, letreiro_y,
                    lx + letreiro_larg, letreiro_y + letreiro_alt,
                    al_map_rgb(180, 30, 30), 6.0f);

                // Textos
                if (fonte) {

                    // Titulo
                    desenharTextoGrande(fonte, al_map_rgb(200, 30, 30),
                        largura_t / 2.0f, letreiro_y + 40.0f, 8.0f, texto_titulo);

                    // Historia (quebra de linha automatica)
                    desenharParagrafo(fonte, al_map_rgb(240, 230, 200),
                        largura_t / 2.0f, letreiro_y + 150.0f,
                        3.0f, letreiro_larg - 140.0f, 42.0f, texto_historia);

                    // Rodape
                    desenharTextoGrande(fonte, al_map_rgb(160, 140, 120),
                        largura_t / 2.0f, letreiro_y + letreiro_alt - 50.0f,
                        2.0f, texto_rodape);

                }

            }

            al_flip_display();

        }

    }

    // Para a musica e libera tudo

    al_stop_samples();

    if (musica) al_destroy_sample(musica);

    if (fonte) al_destroy_font(fonte);

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

    if (fundo) al_destroy_bitmap(fundo);

    al_destroy_event_queue(fila_eventos);

    al_destroy_timer(timer);

    al_destroy_display(display);

}
