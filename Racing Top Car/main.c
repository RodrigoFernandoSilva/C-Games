#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

int CarSpeed_int=0;

#define SpawCarTime 9500
#define DivisibleSound 2
int SpawCarTime_int=0;

SDL_Window * janela;
SDL_Texture * BC;
SDL_Texture * EnemyCars;
SDL_Texture * PinkCar;
SDL_Texture * JogarNovamente;
SDL_Renderer * render;
SDL_Event evento;
TTF_Font *text_font;
SDL_Texture *textTexto;
SDL_Color font_color = {255, 25, 117};

Mix_Chunk * CrashCar = NULL;
Mix_Chunk * ClickSound = NULL;
Mix_Chunk * PinkCarSound = NULL;
Mix_Music * Musica = NULL;

/* As matrizes de 4 são a faixa e as de 7 é o carro. */
int jogadorPositionAtual=1, JogadorPositions[4]={2, 62, 110, 168}, KesIsPress=0,
    RecalcadaSpaw[4]={2, 62, 110, 168}, CarsModels[7]={0, 46, 92, 138, 184, 230, 276},
    ManyCars=0, NumberCars=3, i=0, FaixasUsadas[4]={0, 0, 0, 0}, CarSpeed=20, KeyIsPress=0;

typedef struct{
    int x;
    int y;
    int w;
    int h;
    int Image;
    int Alive;
    int Faixa;
} Cars;

Cars Jogador={0, 390, 46, 76, 0, 1, 0};
Cars Inimigos[3];

void CreateCars(){
    jogadorPositionAtual=1; KesIsPress=0; ManyCars=0;
    Jogador.x=JogadorPositions[jogadorPositionAtual];
    FaixasUsadas[0]=0; FaixasUsadas[1]=0;
    FaixasUsadas[2]=0; FaixasUsadas[3]=0;
    for(i=0;i<NumberCars;i++){
        Inimigos[i].Alive=0;
        Inimigos[i].Faixa=0;
        Inimigos[i].Image=0;
        Inimigos[i].y=-110;
        Inimigos[i].x=0;
        Inimigos[i].w=46;
        Inimigos[i].h=93;
    }
}

void LoadSounds(){
    int flags = MIX_INIT_OGG|MIX_INIT_MOD|MIX_INIT_MP3;
    int initted=Mix_Init(flags);
    if( initted&flags != flags) {
        printf("Mix_Init: Failed to init required ogg and mod support!\n");
        printf("Mix_Init: %s\n", Mix_GetError());
    }

    if(SDL_Init(SDL_INIT_AUDIO)==-1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1 ) {
        printf("Erro ao inicializar o sistema de som \n");
        return -1;
    }

    /* Load the music */
    CrashCar = Mix_LoadWAV  ("./Cars/CarCrash.wav");
    ClickSound = Mix_LoadWAV  ("./Cars/Click.mp3");

    Mix_VolumeChunk(CrashCar, MIX_MAX_VOLUME/DivisibleSound);

    Musica = Mix_LoadMUS("./Cars/Musica.mp3");
    Mix_PlayMusic( Musica, -1 );

    if (CrashCar == NULL || ClickSound == NULL){
        printf("Algum efeito sonoro nao foi encontrado no carregamento!!!");
    }

}
void LoadImages(){
    SDL_Surface * temp0 = IMG_Load("./Cars/Street.png");
    BC = SDL_CreateTextureFromSurface(render, temp0);

    SDL_Surface * temp1 = IMG_Load("./Cars/Cars.png");
    EnemyCars = SDL_CreateTextureFromSurface(render, temp1);

    SDL_Surface * temp2 = IMG_Load("./Cars/Ferrari.png");
    PinkCar = SDL_CreateTextureFromSurface(render, temp2);

    SDL_Surface * temp3 = IMG_Load("./Cars/Texto.png");
    JogarNovamente = SDL_CreateTextureFromSurface(render, temp3);

    SDL_FreeSurface(temp0); SDL_FreeSurface(temp1); SDL_FreeSurface(temp2); SDL_FreeSurface(temp3);

}

void PrintCars(){
    /* Da uma limpada na tela faz bem prasaude. */
    SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
    SDL_RenderClear(render);

    /* Chumba a BackGround no munitor */
    SDL_Rect BCOrigem = {0, 0, 214, 470};
    SDL_Rect BCDestino = {0, 0, 214, 470};
    SDL_RenderCopy(render, BC, &BCOrigem, &BCDestino);

    for(i=0;i<NumberCars;i++){
        if(Inimigos[i].Alive==1){ /* Mostra nudes do carro. */
            SDL_Rect mOrigem = {CarsModels[Inimigos[i].Image], 0, 46, 93};
            SDL_Rect mCDestino = {Inimigos[i].x, Inimigos[i].y, Inimigos[i].w, Inimigos[i].h};
            SDL_RenderCopy(render, EnemyCars, &mOrigem, &mCDestino);
        }
    }

    if(Jogador.Alive==1){ /* Mostra o jogador */
        SDL_Rect mOrigem = {0, 0, 46, 76};
        SDL_Rect mCDestino = {Jogador.x, Jogador.y, Jogador.w, Jogador.h};
        SDL_RenderCopy(render, PinkCar, &mOrigem, &mCDestino);
    }
}
void PrintFont(){
    char texto[9];
    itoa(CarSpeed, texto, 10);
    SDL_Surface * surText = TTF_RenderText_Solid(text_font, texto, font_color);
    textTexto = SDL_CreateTextureFromSurface(render, surText);
    int w=110, h=110, x;
    SDL_QueryTexture(textTexto, 0, NULL, &w, &h);
    x = (98*w)/w;
    SDL_Rect rOrigem = {0, 0, w, h};
	SDL_Rect rDestino = {x, 199, w, h};
    SDL_RenderCopy(render, textTexto, &rOrigem, &rDestino);

    SDL_FreeSurface(surText);
    free(surText);
    surText=NULL;
    SDL_DestroyTexture(textTexto);
    free(textTexto);
}

void KeyBoard(){
    int board = SDL_PollEvent(&evento);
    if(board > 0){
        if(evento.type==SDL_KEYDOWN && KesIsPress==0){
            if(evento.key.keysym.sym == SDLK_RIGHT && jogadorPositionAtual<3){
                KesIsPress==1;
                jogadorPositionAtual+=1;
            }
            else if(evento.key.keysym.sym == SDLK_LEFT && jogadorPositionAtual>0){
                KesIsPress==1;
                jogadorPositionAtual-=1;
            }
            Jogador.x=JogadorPositions[jogadorPositionAtual];
        }
        else if(evento.type==SDL_KEYUP && KesIsPress==1){
            if(evento.key.keysym.sym == SDLK_RIGHT || evento.key.keysym.sym == SDLK_LEFT){
                KesIsPress==0;
            }
        }
    }
}

void IA(){
    int Faixa;
    if(CarSpeed_int>CarSpeed){
        for(i=0;i<NumberCars;i++){ /* Movimentação dos carro seguida pela morte deles. */
            CarSpeed_int=0;
            Inimigos[i].y+=1;
        }

    }
    for(i=0;i<NumberCars;i++){
        if(Inimigos[i].y>480 && Inimigos[i].Alive==1){
            FaixasUsadas[Inimigos[i].Faixa]=0;
            Inimigos[i].Alive=0;
            ManyCars-=1;
        }
    }

    SpawCarTime_int+=1;
    if(SpawCarTime_int>SpawCarTime && ManyCars<NumberCars){
        for(i=0;i<NumberCars;i++){
            if(Inimigos[i].Alive==0){
                Inimigos[i].y=-110;
                while(1){
                    Faixa = rand()%4;
                    Inimigos[i].Faixa=Faixa;
                    Inimigos[i].x = RecalcadaSpaw[Faixa];
                    if(FaixasUsadas[Inimigos[i].Faixa]==0){
                        break;
                    }
                }
                FaixasUsadas[Faixa]=1;
                Inimigos[i].Image = rand()%7;
                Inimigos[i].Alive=1;
                ManyCars+=1;
                SpawCarTime_int=rand()&SpawCarTime+300;
                break;
            }
        }
    }
}

void Colisao(){
    int Resposta, Animation=0;
    for(i=0;i<NumberCars;i++){
        if(Inimigos[i].Alive==1){
            SDL_Rect obj1 = {Jogador.x, Jogador.y, Jogador.w, Jogador.h};
            SDL_Rect obj2 = {Inimigos[i].x, Inimigos[i].y, Inimigos[i].w, Inimigos[i].h};
            if(SDL_HasIntersection((&obj1), (&obj2))){
                Mix_FreeChunk(PinkCarSound);
                Mix_PlayChannel( -1, CrashCar, 0 );
                RestartFase();
            }
        }
    }
}
void RestartFase(){
    int board = SDL_PollEvent(&evento), PressSpace=0;

    do{
        PrintCars();
        PrintFont();
        SDL_Rect TOrigem = {0, 0, 128, 280};
        SDL_Rect TDestino = {7, 10, 198, 250};
        SDL_RenderCopy(render, JogarNovamente, &TOrigem, &TDestino);
        SDL_RenderPresent(render);
        board = SDL_PollEvent(&evento);
        if(board > 0){
            if(evento.type==SDL_KEYDOWN && KeyIsPress==0){
                if(evento.key.keysym.sym == SDLK_SPACE){
                    break;
                }
                if(evento.key.keysym.sym == SDLK_UP){
                    Mix_PlayChannel(-1, ClickSound, 0);
                    KeyIsPress=1;
                    CarSpeed+=1;
                }
                if(evento.key.keysym.sym == SDLK_DOWN){
                    Mix_PlayChannel(-1, ClickSound, 0);
                    KeyIsPress=1;
                    CarSpeed-=1;
                }
            }
            else if(evento.type == SDL_KEYUP){
                if(evento.key.keysym.sym == SDLK_UP || evento.key.keysym.sym == SDLK_DOWN){
                    KeyIsPress = 0;
                }
            }
        }
    }while(1);

    PinkCarSound = Mix_LoadWAV  ("./Cars/PinkCar.wav");
    Mix_VolumeChunk(PinkCarSound, MIX_MAX_VOLUME/DivisibleSound);
    Mix_PlayChannel(-1, PinkCarSound, -1 );
    CreateCars();
}

void GamePlay(){
    KeyBoard();
    PrintCars();
    SDL_RenderPresent(render);
    CarSpeed_int+=1;
    IA();
    Colisao();
}

int main(int c, char *arts[]){
    srand(time(NULL));
    SpawCarTime_int=rand()%SpawCarTime+300;
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("Racing Top Car", 200, 100, 214, 470, SDL_WINDOW_SHOWN);
    if(janela != NULL){
        if(TTF_Init() >=0){
            text_font = TTF_OpenFont("consola.ttf", 20);
            if(text_font == NULL){
                printf("Fonte nao incomtrada!!!!\n");
            }
            render = SDL_CreateRenderer(janela, -1, 0);
            LoadImages();
            LoadSounds();
            CreateCars();
            RestartFase();
            if(render != NULL){
                while(1){
                    GamePlay();
                }
            }
        }
    }
    Mix_Quit();
    Mix_CloseAudio();
}
