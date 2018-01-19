/*
Cada quadrado tem 64 pixel;
O tabuleiro e de 512 por 512;
Se usa o mouse para jogar.
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

//SDLs para criar a janela do jogo.
SDL_Window *Janela;
SDL_Renderer *Render;

//SDL para ler o evento do teclado. É onde vai ser salvo um evento da lista de eventos do teclao.
SDL_Event Evento;
int int_Evento;

//SDLs de textura do jogo, onde vai ser salvo as imagens do jogo.
SDL_Texture *Textu_Tabuleiro;
SDL_Texture *Textu_Vermelho;

//Usadas para printar o texto na tela.
TTF_Font *Texto_Font;
SDL_Texture *Texto_Texture;
SDL_Surface *Texto_Surface;
SDL_Color Texto_ColorVermelho = {255, 0, 0};
SDL_Color Texto_ColorVerde = {0, 255, 0};


int i, i0, i1, MousePressionado = 0, *MouseX, *MouseY, Pegou_BoolPeca = 0, Pegou_PecaI, Pegou_PecaX, Pegou_PecaY,
    Matriz_Ocupada[8][8]; //1 - Vermelho    2 - Verde    0 - Casa branca não pode jogar    3 - Casa preta que pode jogar.
int QualJogador; //1 - vermelho,    2 - verde.
char QuemJoga = 'g'; //r = Vermelho/Red     g = Verde/Green.

//estruturas das peças do tabuleiro.
typedef struct{
    int x;
    int y;
    int Ativo;
    int FoiPega;
    int Dama;
} Pecas;

Pecas PecasVermelho[12];
Pecas PecasVerde[12];

void Func_CarregarImagens(){
    SDL_Surface *temp0 = IMG_Load("./Assest/Sprites/Tabuleiro.png"); //Cria uma textura na memoria.
    Textu_Tabuleiro = SDL_CreateTextureFromSurface(Render, temp0); //Tras a textura da memoria para a variável

    SDL_Surface *temp2 = IMG_Load("./Assest/Sprites/SpriteSheet.png"); //Cria uma textura na memoria.
    Textu_Vermelho = SDL_CreateTextureFromSurface(Render, temp2); //Tras a textura da memoria para a variável


    //Limpa as areas de memoria criadas.
    SDL_FreeSurface(temp0); SDL_FreeSurface(temp2);
}
void Func_ZerarPecas(){
     int DestinoX, DestinoY; //Variaveis usadas para ajudar nos calculas de colocar as peças verdes e vermelhas.
    //Inicializa a Matriz_Ocupada, dizendo o que é o que;
    for(i = 0; i < 8; i++){//Diz que todas as casas no tabuleiro é brancas, depoi são salvo as casas das peças.
        for(i0 = 0; i0 < 8; i0++){
            Matriz_Ocupada[i][i0] = 0;
        }
    }
    for(i = 0; i < 8; i++){//Diz qual lugar no tabuleiro são casas pretas.
        if(i%2 == 0){
            for(i0 = 1; i0 < 8; i0+=2){//Começa a salvar pulando uma linha e vai de dois em dois.
                Matriz_Ocupada[i][i0] = 3;
            }
        }
        else{
            for(i0 = 0; i0 < 8; i0+=2){//Começa a salvar sem pulando uma linha e vai de dois em dois.
                Matriz_Ocupada[i][i0] = 3;
            }
        }
    }
    for(i = 0; i < 3; i++){//Diz qual lugar é vermelho.
        if(i%2 == 0){
            for(i0 = 1; i0 < 8; i0+=2){//Começa a salvar pulando uma linha e vai de dois em dois.
                Matriz_Ocupada[i][i0] = 1;
            }
        }
        else{
            for(i0 = 0; i0 < 8; i0+=2){//Começa a salvar sem pulando uma linha e vai de dois em dois.
                Matriz_Ocupada[i][i0] = 1;
            }
        }
    }
    for(i = 5; i < 8; i++){//Diz qual lugar é verde.
        if(i%2 == 0){
            for(i0 = 1; i0 < 8; i0+=2){//Começa a salvar pulando uma linha e vai de dois em dois.
                Matriz_Ocupada[i][i0] = 2;
            }
        }
        else{
            for(i0 = 0; i0 < 8; i0+=2){//Começa a salvar sem pulando uma linha e vai de dois em dois.
                Matriz_Ocupada[i][i0] = 2;
            }
        }
    }

    //Carregas as variaveis das peças vermelhas.
    DestinoX = 64;
    DestinoY = 0;
    for(i = 0; i < 12; i++){
        PecasVermelho[i].x = DestinoX;
        PecasVermelho[i].y = DestinoY;
        PecasVermelho[i].Ativo = 1;
        PecasVermelho[i].FoiPega = 0;
        PecasVermelho[i].Dama = 0;
        DestinoX += 128;
        if(DestinoX >= 512){
            if(DestinoY == 0){
                DestinoX = 0;
                DestinoY += 64;
            }
            else{
                DestinoX = 64;
                DestinoY += 64;
            }
        }
    }
    //Carregas as variaveis das peças verdes.
    DestinoX = 0;
    DestinoY = 320;
    for(i = 0; i < 12; i++){
        PecasVerde[i].x = DestinoX;
        PecasVerde[i].y = DestinoY;
        PecasVerde[i].Ativo = 1;
        PecasVerde[i].FoiPega = 0;
        PecasVerde[i].Dama = 0;
        DestinoX += 128;
        if(DestinoX >= 512){
            if(DestinoY == 320){
                DestinoX = 64;
                DestinoY += 64;
            }
            else{
                DestinoX = 0;
                DestinoY += 64;
            }
        }
    }

}
void Func_DizerQualJogador(){
    Func_PrintarImagens();
    int x, w, h;
    //QualJogador = 2;
    if(QualJogador == 1){
        Texto_Surface = TTF_RenderText_Solid(Texto_Font, "Vermelho", Texto_ColorVermelho);
        x = 170;
    }
    else{
        Texto_Surface = TTF_RenderText_Solid(Texto_Font, "Verde", Texto_ColorVerde);
        x = 190;
    }
    Texto_Texture = SDL_CreateTextureFromSurface(Render, Texto_Surface);
    SDL_QueryTexture(Texto_Texture, 0, 0, &w, &h);
    SDL_Rect tOrigem = {0, 0, w, h};
    SDL_Rect tDestino = {x, 230, w, h};
    SDL_RenderCopy(Render, Texto_Texture, &tOrigem, &tDestino);

    SDL_FreeSurface(Texto_Surface);
    free(Texto_Surface);
    Texto_Surface=NULL;
    SDL_DestroyTexture(Texto_Texture);
    free(Texto_Texture);

    SDL_RenderPresent(Render);
    //Sleep(500);
}

void Func_LerPerifericos(){
    SDL_GetMouseState(&MouseX, &MouseY); //Salva as posições 'x' e 'y' do mouse.
    int_Evento = SDL_PollEvent(&Evento); //Salva o primeiro evento da lista de evento em uma variavel int;
    if(int_Evento>0){ //Ve se a variavel émaior q zero porque se for quer dizer q alguma tecla foi pressionada,
        if (Evento.type == SDL_QUIT) { //Se o jogador apertar no 'x' da janela,ojogofecha.
            exit(0);
        }
        else if(Evento.type == SDL_MOUSEBUTTONDOWN && MousePressionado == 0){ //Vê se o botão do mouse foi precionado.
            if(Evento.button.clicks == SDL_BUTTON_LEFT){
                MousePressionado = 1;
            }
        }
        else if(Evento.type == SDL_MOUSEBUTTONUP && MousePressionado == 1){ //Vê se o botão do mouse foi solto.
            if(Evento.button.clicks == SDL_BUTTON_LEFT){
                MousePressionado = 0;
                if(QualJogador == 1){
                    Func_DroparPecaVermelha();
                }
                else{
                    Func_DroparPecaVerde();
                }
            }
        }
    }
}
void Func_MoverPecasVermelha(){
    if(MousePressionado == 1){ //Só pode mover a peça se o mouse estiver precionado.
        SDL_Rect Obj1 = {MouseX, MouseY, 1, 1}; //É criado para ve se o mouse colidio com a peça.
        for(i = 0; i < 12; i++){ //Colidio com as peças vermelha.
            SDL_Rect Obj2 = {PecasVermelho[i].x, PecasVermelho[i].y, 64, 64};
            if(SDL_HasIntersection((&Obj1), (&Obj2)) && Pegou_BoolPeca == 0){ //if da colisão em si.
                Pegou_PecaI = i;
                Pegou_PecaX = PecasVermelho[i].x;
                Pegou_PecaY = PecasVermelho[i].y;
                Pegou_BoolPeca = 1;
                PecasVermelho[i].FoiPega = 1;
            }
        }
    }
}
void Func_MoverPecasVerde(){
    if(MousePressionado == 1){ //Só pode mover a peça se o mouse estiver precionado.
        SDL_Rect Obj1 = {MouseX, MouseY, 1, 1}; //É criado para ve se o mouse colidio com a peça.
        for(i = 0; i < 12; i++){ //Colidio com as peças vermelha.
            SDL_Rect Obj2 = {PecasVerde[i].x, PecasVerde[i].y, 64, 64};
            if(SDL_HasIntersection((&Obj1), (&Obj2)) && Pegou_BoolPeca == 0){ //if da colisão em si.
                Pegou_PecaI = i;
                Pegou_PecaX = PecasVerde[i].x;
                Pegou_PecaY = PecasVerde[i].y;
                Pegou_BoolPeca = 1;
                PecasVerde[i].FoiPega = 1;
            }
        }
    }
}
void Func_PrintarImagens(){//A orde de printagem interfere em quem vai ficar por cima de quem.
    //Limpa a tela.
    SDL_SetRenderDrawColor(Render, 0, 0, 0, 1);
    SDL_RenderClear(Render);

    //Printa o tabuleiro na tela.
    SDL_Rect tOrigem = {0, 0, 512, 512};
    SDL_Rect tDestino = {0, 0, 512,512};
    SDL_RenderCopy(Render, Textu_Tabuleiro, &tOrigem, &tDestino);

    //Printa as peças vermelhas na tela.
    for(i = 0; i < 12; i++){
        if(PecasVermelho[i].Ativo == 1 && PecasVermelho[i].FoiPega == 0){
            SDL_Rect vOrigem = {0, 0, 64, 64};
            SDL_Rect vDestino = {PecasVermelho[i].x, PecasVermelho[i].y, 64, 64};
            SDL_RenderCopy(Render, Textu_Vermelho, &vOrigem, &vDestino);
        }
    }

    //Printa as peças verdes na tela.
    for(i = 0; i < 12; i++){
        if(PecasVerde[i].Ativo == 1 && PecasVerde[i].FoiPega == 0){
            SDL_Rect vOrigem = {64, 0, 64, 64};
            SDL_Rect vDestino = {PecasVerde[i].x, PecasVerde[i].y, 64, 64};
            SDL_RenderCopy(Render, Textu_Vermelho, &vOrigem, &vDestino);
        }
    }

    if(Pegou_BoolPeca == 1){
        if(QualJogador == 1){
            SDL_Rect vOrigem = {0, 0, 64, 64};
            SDL_Rect vDestino = {MouseX - 8, MouseY - 8, 64, 64};
            SDL_RenderCopy(Render, Textu_Vermelho, &vOrigem, &vDestino);
        }
        else{
            SDL_Rect vOrigem = {64, 0, 64, 64};
            SDL_Rect vDestino = {MouseX - 8, MouseY - 8, 64, 64};
            SDL_RenderCopy(Render, Textu_Vermelho, &vOrigem, &vDestino);
        }
    }


    //Atualiza a tela, apra as texturas serem aplicadas;
    SDL_RenderPresent(Render);
}

void Func_DroparPecaVermelha(){
    int TempX, TempY;
    if(Pegou_BoolPeca == 1){
        TempX = MouseX;
        TempY = MouseY;

        TempX /= 64;
        TempY /= 64;

        Pegou_BoolPeca = 0;
        PecasVermelho[Pegou_PecaI].x = Pegou_PecaX;
        PecasVermelho[Pegou_PecaI].y = Pegou_PecaY;
        PecasVermelho[Pegou_PecaI].FoiPega = 0;
        if(Matriz_Ocupada[TempY][TempX] != 3){
            return 0;
        }
        if(TempX > (Pegou_PecaX/64)){
            if(TempX - 1 != (Pegou_PecaX/64)){
                if(TempX + 1 != (Pegou_PecaX/64)){
                    if(Matriz_Ocupada[TempY][TempX]){}
                    printf("oi");
                }
                //return 0;
            }
        }
        if(TempX < (Pegou_PecaX/64)){
            if(TempX + 1 != (Pegou_PecaX/64)){
                return 0;
            }
        }
        if(TempY != (Pegou_PecaY/64) + 1){
            return 0;
        }


        PecasVermelho[Pegou_PecaI].x = TempX * 64;
        PecasVermelho[Pegou_PecaI].y = TempY * 64;
        PecasVermelho[Pegou_PecaI].FoiPega = 0;
        Matriz_Ocupada[TempY][TempX] = 1;
        Matriz_Ocupada[Pegou_PecaY/64][Pegou_PecaX/64] = 3;
        QualJogador = 2;
        Func_DizerQualJogador();
    }
}
void Func_DroparPecaVerde(){
    int TempX, TempY;
    if(Pegou_BoolPeca == 1){
        TempX = MouseX;
        TempY = MouseY;

        TempX /= 64;
        TempY /= 64;

        Pegou_BoolPeca = 0;
        PecasVerde[Pegou_PecaI].x = Pegou_PecaX;
        PecasVerde[Pegou_PecaI].y = Pegou_PecaY;
        PecasVerde[Pegou_PecaI].FoiPega = 0;
        if(Matriz_Ocupada[TempY][TempX] != 3){
            return 0;
        }
        if(TempX > (Pegou_PecaX/64)){
            if(TempX - 1 != (Pegou_PecaX/64)){
                return 0;
            }
        }
        if(TempX < (Pegou_PecaX/64)){
            if(TempX + 1 != (Pegou_PecaX/64)){
                return 0;
            }
        }
        if(TempY != (Pegou_PecaY/64) - 1){
            return 0;
        }


        PecasVerde[Pegou_PecaI].x = TempX * 64;
        PecasVerde[Pegou_PecaI].y = TempY * 64;
        PecasVerde[Pegou_PecaI].FoiPega = 0;
        Matriz_Ocupada[TempY][TempX] = 1;
        Matriz_Ocupada[Pegou_PecaY/64][Pegou_PecaX/64] = 3;
        QualJogador = 1;
        Func_DizerQualJogador();
    }
}

void Func_Jogar(){
    Func_LerPerifericos();
    if(QualJogador == 1){
        Func_MoverPecasVermelha();
    }
    else{
        Func_MoverPecasVerde();
    }

    Func_PrintarImagens();
}

int main(int c, char *arts[]){
    srand(time(NULL));
    QualJogador = (rand()%2) + 1;
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    Janela = SDL_CreateWindow("ShootingGame", 100, 100, 512, 512, SDL_WINDOW_SHOWN);
    if(Janela !=NULL){
        Render = SDL_CreateRenderer(Janela, -1, 0);
        if(Render != NULL && TTF_Init() >=0){
            Texto_Font = TTF_OpenFont("./Assest/Fonts/arialbd.ttf", 40);
            Func_CarregarImagens();
            Func_ZerarPecas();
            Func_DizerQualJogador();
            while(1){
                Func_Jogar();
            }
        }
    }

    return 0;
}
