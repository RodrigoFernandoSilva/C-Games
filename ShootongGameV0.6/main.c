#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <time.h>

#define BulletSpeed 60     //40
#define PlayerSpeed 50     //30
#define EnemysSpeed 140    //100
#define TimeFuncation 15
#define TimeChangeSprite 400    //380
#define DificuldadeMais 5
#define TimeEnemyShootMaxRand 1500
#define TimeDestroyBullet 300
#define EnemyBulletY 3
#define SetaTimeMove 30
#define PlayerDeadTime 1500
#define TimeMenuSAT 40

//Variáveis do tipo Delta_Time são usadas para delimitar i tempo de um evento, elas tem cada uma seu propio '#define'.
//Estado do jogo: 0 = Menu | 1 = Start | 2 = High Score| 3 = Options | 4 = Credits | 5 = Exit | 6 = GameOver
int TimeEnemyShoot = 0, TimeDestroyBullet_DeltaTime=0, BlewUpSAT=0, EstadoJogo=0, CenariosIbdex[5]={0,558,1116,1674,2232},
    Seta=0, SetaIndex[5]={22, 122, 225, 325, 428}, SetaIsPress=0, PlayerDeadTime_DeltaTime=0, PlayerIsDead=0,
    PlayerImuni=0, PlayerLastSprite=192, PlayerLastSprite0=192, TimeMenuSAT_DeltaTime=0, DificuldadeJogo=20, PlayFastFaseSound=0;

//Variáveis usadas para criar a janela de renderização.
SDL_Window * janela;;
SDL_Renderer * render;

//Variáveis de textura, nela estão salvo todas as imagens que aparece no jogo.
SDL_Texture * SpritesPlayer;
SDL_Texture * cenario;
SDL_Texture * bullet;
SDL_Texture * enemys;
SDL_Texture * seta;
SDL_Texture * FimDeJogo;

//Variável usada para salvar os eventos do teclado do buffer de comandos de perifericos.
SDL_Event evento;

//Vari'`veis que são usadas para printar textos na tela.
TTF_Font *text_font;
SDL_Texture *text_Texto;
SDL_Surface * surText;
SDL_Color font_color = {240, 240, 240};

//Variáveis de som, nelas então salvo todos os efeitos sonoro do jogo.
Mix_Chunk * ShootPlayerSound = NULL;
Mix_Chunk * ShootEnemySound = NULL;
Mix_Chunk * FastFaseSound = NULL;
Mix_Chunk * ExplisionPlayerSound = NULL;
Mix_Chunk * ExplosionEnemyrSound = NULL;
Mix_Chunk * BulletCollisionSound = NULL;
Mix_Chunk * MenuPassSound = NULL;
Mix_Chunk * MenuSelectSound = NULL;

int WhoCanShoot[25], Score=0;

//WitchSprite 0 e o primeiro sprite, 1 é o segundo sprite.
//LifeIndex E a vida do jogador, ela subtrai em 64 em 64, para dar 3 vidas, é um SAT.
int Enemys_DeltaTime = 1, Bulet_DeltaTime=0, Player_DeltaTime=0, Function_DeltaTime=0, BuletEnemy_DeltaTime=0,
    TimeChangeSprite_DeltaTime=0, WitchSprite=0, InitialPint=0, InitialPint0=0, Dificuldade=10, LastDown=1,
    EnemysDeads=0, LifeIndex=192, BEIndex=0, TimeEnemyShoot_DeltaTime=0;

typedef struct{ //EStrutura do SDL_RectF --> usa ele para um rect float.
    float x, y;
    int w, h;
} SDL_RectF;

typedef struct{ //Estrutura do player.
    float x;
    float y;
    int w;
    int h;
    int DireMove;
    int IndexImage;
} Player;
Player Mathews={270, 430, 64, 44, 0, 192};
Player PlayerLife;

typedef struct{ //Estrutura das balas.
    SDL_RectF destino;
    int Ativo;
    int IndexImage;

} Bala;
Bala PlayerBullet = {{100, 100, 7, 14}, 0, 0};
Bala EnemyBullet[5];

typedef struct{ //Estrutura dos inimigos.
    SDL_RectF destino;
    int Ativo;
    int DirecMove;
    int Distance;
    int PointOrigem;
    int MatrizX; //Serve para dizer qual inimigo ja foi destruido para o outro debaixo dele poder atirar.
} Enemys;
Enemys MoveBase={{20,-30, 91, 55},1, 1, 0, 0, 0};
Enemys inimigos[15];

//Zera as livda do jogador, pontuação e etc.
void ZerarFase(){
    LifeIndex=192; Score=0;
    PlayerLife.DireMove=0; PlayerLife.h=44; PlayerLife.w=192;
    PlayerLife.x=10;       PlayerLife.y=10; PlayerLife.IndexImage=0;
}
void InicializarJogo(){ //Zera a fase para cada vez q o jogador troca dela, porém comtinua apontuação e vida.
    MoveBase.DirecMove = 1;
    LastDown=1;
    PlayerImuni=0;
    PlayerIsDead=0;
    PlayerDeadTime_DeltaTime=0;
    PlayerLastSprite0=192;
    PlayerLastSprite=192;
    EnemysDeads=0;
    Dificuldade=0;
    //Inicializa a movimentação base.
    MoveBase.DirecMove=1;  MoveBase.destino.x=20;
    //Inicializa o player.
    Mathews.x=270; Mathews.DireMove=0, Mathews.IndexImage=192;

    PlayFastFaseSound=0;

    int i=0,i0, Distance, Coluna=100, PontoOrigem=0, IndexMatriz=0;
    //Inicializar matriz.
    for(i=0;i<15;i++){
        WhoCanShoot[i]=1;
    }
    for(i=15;i<25;i++){
        WhoCanShoot[i]=0;
    }

    //Inicializar as balas do inimigo.
    for(i=0;i<5;i++){
        EnemyBullet[i].destino.x=100;
        EnemyBullet[i].destino.y=100;
        EnemyBullet[i].destino.w=15;
        EnemyBullet[i].destino.h=26;
        EnemyBullet[i].Ativo=0;
        EnemyBullet[i].IndexImage=0;
    }

    //Inicializa os inimigos.
    for(i0=0; i0<15; i0=i0+5){
        Distance=0;
        for(i=0; i<5; i++){
            inimigos[i+i0].Ativo=1;
            inimigos[i+i0].DirecMove=0;
            inimigos[i+i0].destino.x=10+Distance;
            inimigos[i+i0].destino.y=Coluna;
            inimigos[i+i0].destino.h=50;
            inimigos[i+i0].destino.w=50;
            inimigos[i+i0].Distance=Distance;
            inimigos[i+i0].PointOrigem=PontoOrigem;
            inimigos[i+i0].MatrizX=IndexMatriz;
            Distance+=100;
            IndexMatriz+=1;
        }
        Coluna+=60;
        PontoOrigem+=116;
    }
}

void CapturarTeclado(){ //Esta função é responsavel por todos os comandos do teclado digitados pelo Jogador.
    TimeMenuSAT_DeltaTime+=1;
    int qtd = SDL_PollEvent(&evento); //Variavel que armazena a leitura do teclado.
    if (qtd > 0 && EstadoJogo==0 && TimeMenuSAT_DeltaTime>TimeMenuSAT){ //Menu do jogo.
        if(evento.type == SDL_KEYDOWN && SetaIsPress==0){
            if(evento.key.keysym.sym == SDLK_DOWN && Seta<4){
                Mix_PlayChannel( -1, MenuPassSound, 0 );
                SetaIsPress=1;
                Seta+=1;
            }
            else if(evento.key.keysym.sym == SDLK_UP && Seta>0){
                Mix_PlayChannel( -1, MenuPassSound, 0 );
                SetaIsPress=1;
                Seta-=1;
            }
            else if(evento.key.keysym.sym == SDLK_SPACE && Seta==0){ //Start
                //Mix_PlayChannel( -1, MenuSelectSound, 0 );
                EstadoJogo=1;
                Bulet_DeltaTime=0;
            }
            else if(evento.key.keysym.sym == SDLK_SPACE && Seta==1){ //Start
                Mix_PlayChannel( -1, MenuSelectSound, 0 );
                EstadoJogo=2;
                TimeMenuSAT_DeltaTime=0;
            }
            else if(evento.key.keysym.sym == SDLK_SPACE && Seta==2){ //Credits
                Mix_PlayChannel( -1, MenuSelectSound, 0 );
                EstadoJogo=3;
                TimeMenuSAT_DeltaTime=0;
            }
            else if(evento.key.keysym.sym == SDLK_SPACE && Seta==3){ //Opções
                Mix_PlayChannel( -1, MenuSelectSound, 0 );
                SetaIsPress = 0;
                EstadoJogo=4;
                TimeMenuSAT_DeltaTime=0;
            }
            else if(evento.key.keysym.sym == SDLK_SPACE && Seta==4){ //Exit
                Mix_Quit();
                Mix_CloseAudio();
                exit(0);
            }
        }
        else if(evento.type == SDL_KEYUP && SetaIsPress==1){
            if(evento.key.keysym.sym == SDLK_DOWN || evento.key.keysym.sym == SDLK_UP){
                SetaIsPress=0;
            }
        }
    }

    if (qtd > 0 && EstadoJogo==1 && PlayerIsDead==0) { //Start do menu.
        if(evento.type == SDL_KEYDOWN){ //Ve se o evendo do teclado foi de precionar tecla.
            if(evento.key.keysym.sym == SDLK_RIGHT){ //Ve se a tecla e a seta para a direita.
                Mathews.DireMove = 1; //Nº 1 direita, nº -1 esquerda, isso vale para 'move'.
            }
            if(evento.key.keysym.sym == SDLK_LEFT){ //Ve se a tecla é a seta para a esquerda.
                Mathews.DireMove = -1;
            }
            if(evento.key.keysym.sym == SDLK_SPACE && PlayerBullet.Ativo == 0 && Bulet_DeltaTime>BulletSpeed){ //Atira uma bala do player.
                Mix_PlayChannel( -1, ShootPlayerSound, 0 );
                PlayerBullet.destino.x = Mathews.x + 28;
                PlayerBullet.destino.y = Mathews.y;
                PlayerBullet.Ativo = 1;
            }
        }
        else if(evento.type == SDL_KEYUP){
            if(evento.key.keysym.sym == SDLK_RIGHT && Mathews.DireMove==1){
                Mathews.DireMove = 0;
            }
            if(evento.key.keysym.sym == SDLK_LEFT && Mathews.DireMove==-1){
                Mathews.DireMove = 0;
            }
        }
    }

    if (qtd > 0 && EstadoJogo==2) { //maior pontuação.
        if(evento.type == SDL_KEYDOWN){ //Ve se o evendo do teclado foi de precionar tecla.
            if(evento.key.keysym.sym == SDLK_SPACE && TimeMenuSAT_DeltaTime>TimeMenuSAT){ //Atira uma bala do player.
                EstadoJogo=0;
                TimeMenuSAT_DeltaTime=0;
            }
        }
    }

    if (qtd > 0 && EstadoJogo==3 && TimeMenuSAT_DeltaTime>TimeMenuSAT) { //Opções do menu.
            if(evento.type == SDL_KEYDOWN && SetaIsPress == 0){ //Ve se o evendo do teclado foi de precionar tecla.
                if(evento.key.keysym.sym == SDLK_RIGHT){ //Ve se a tecla e a seta para a direita.
                    SetaIsPress = 1;
                    DificuldadeJogo+=1;
                }
                if(evento.key.keysym.sym == SDLK_LEFT){ //Ve se a tecla é a seta para a esquerda.
                    SetaIsPress = 1;
                    DificuldadeJogo -= 1;
                }
                if(evento.key.keysym.sym == SDLK_SPACE){
                    EstadoJogo = 0;
                    TimeMenuSAT_DeltaTime = 0;
                }
            }
            if(evento.type == SDL_KEYUP && SetaIsPress == 1){
                if(evento.key.keysym.sym == SDLK_RIGHT || evento.key.keysym.sym == SDLK_LEFT){
                    SetaIsPress = 0;
                }
            }
        }

    if (qtd > 0 && EstadoJogo==4 && TimeMenuSAT_DeltaTime>TimeMenuSAT) { //Credits do menu.
        if(evento.type == SDL_KEYDOWN){ //Ve se o evendo do teclado foi de precionar tecla.
            if(evento.key.keysym.sym == SDLK_SPACE){ //Atira uma bala do player.
                EstadoJogo=0;
                TimeMenuSAT_DeltaTime=0;
            }
        }
    }
}

void DescerFileira(){
    int i;
    for(i=0; i<15; i++){
        inimigos[i].destino.y += 15;
    }
}
void Mecanica(){
    int i;
    Player_DeltaTime+=1;
    if(Player_DeltaTime>PlayerSpeed-DificuldadeJogo){
        Player_DeltaTime=0;
        if(Mathews.DireMove == 1 && Mathews.x < 571){ //Movimenta a nave esquerda.
            Mathews.x += 5;
        }
        else if(Mathews.DireMove == -1 && Mathews.x > 5){ //Movimenta a nave direita.
            Mathews.x -= 5;
        }
    }

    Bulet_DeltaTime +=1;
    if(PlayerBullet.Ativo == 1 && Bulet_DeltaTime > BulletSpeed-DificuldadeJogo && BlewUpSAT==0){ //Movimentação da bala do player.
        PlayerBullet.destino.y -= 15;
        Bulet_DeltaTime = 0;
    }
    if(PlayerBullet.destino.y < -14 && PlayerBullet.Ativo == 1){
        PlayerBullet.Ativo = 0;
    }

    //Verificar se um inimigo ja passou da tela, para mudar a movimentação do retangulo base.
    Function_DeltaTime+=1;
    for(i=0; i<15; i++){
        if(inimigos[i].destino.x > 570 && inimigos[i].Ativo==1 && LastDown==1){
            MoveBase.DirecMove = -1;
            LastDown=0;
            if(Function_DeltaTime>TimeFuncation){
                DescerFileira();
                Function_DeltaTime=0;
            }
            break;
        }
        else if(inimigos[i].destino.x < 10 && inimigos[i].Ativo==1 && LastDown==0){
            MoveBase.DirecMove = 1;
            LastDown=1;
            if(Function_DeltaTime>TimeFuncation){
                DescerFileira();
                Function_DeltaTime=0;
            }
            break;
        }
    }

    //Movimentação do retangulo base dos inimigos.
    Enemys_DeltaTime+=1;
    if(Enemys_DeltaTime>EnemysSpeed-Dificuldade-(DificuldadeJogo*2)){
        Enemys_DeltaTime=0;
        if(MoveBase.DirecMove == 1){
            MoveBase.destino.x +=5;
        }
        if(MoveBase.DirecMove == -1){
            MoveBase.destino.x -=5;
        }
    }

    //Movimentação do inimigo.
    for(i=0; i<15; i++){
        inimigos[i].destino.x = MoveBase.destino.x + inimigos[i].Distance;
    }
    for(i=0; i<15; i++){
        if(inimigos[i].Ativo == 1 && inimigos[i].destino.y>380){
            Printar();
            TimeMenuSAT_DeltaTime = 0;
            GameOver();
        }
    }

}
void Collision(){
    int i, Index_PlayerBullet=0, Index_inimigo, Colidiu=0;
    if(PlayerBullet.Ativo==1 && BlewUpSAT==0){ //Se a bala estive rativa, ai sim pode calcular a colição
        for(i=0; i<15; i++){ //Inimigos
            SDL_Rect obj1 = {PlayerBullet.destino.x, PlayerBullet.destino.y, PlayerBullet.destino.w, PlayerBullet.destino.h};
            SDL_Rect obj2 = {inimigos[i].destino.x, inimigos[i].destino.y, inimigos[i].destino.w, inimigos[i].destino.h};
            if(SDL_HasIntersection((&obj1), (&obj2)) && inimigos[i].Ativo==1){
                inimigos[i].Ativo=0;
                PlayerBullet.IndexImage=254;
                PlayerBullet.destino.h=38;
                PlayerBullet.destino.w=57;
                PlayerBullet.destino.x=inimigos[i].destino.x;
                PlayerBullet.destino.y=inimigos[i].destino.y+10;
                TimeDestroyBullet_DeltaTime=0;
                Dificuldade += DificuldadeMais;
                EnemysDeads+=1;
                WhoCanShoot[i]=0;
                BlewUpSAT=1;
                Score+=10;
                Mix_PlayChannel( -1, ExplosionEnemyrSound, 0 );
            }
        }
    }

    for(i=0;i<5;i++){ //Testa colisão oom da bala do inimigo com o player.
        if(EnemyBullet[i].Ativo==1){
            SDL_Rect obj1 = {Mathews.x, Mathews.y, Mathews.w, Mathews.h};
            SDL_Rect obj2 = {EnemyBullet[i].destino.x, EnemyBullet[i].destino.y, EnemyBullet[i].destino.w, EnemyBullet[i].destino.h};
            if(SDL_HasIntersection((&obj1), (&obj2)) && PlayerIsDead==0 && PlayerImuni==0){
                EnemyBullet[i].Ativo=0;
                LifeIndex-=64;
                BEIndex-=1;
                PlayerIsDead=1;
                PlayerDeadTime_DeltaTime=0;
                PlayerLastSprite0=256;
                Mathews.DireMove = 0;
                PlayerLastSprite=256;
                Mix_PlayChannel( -1, ExplisionPlayerSound, 0 );
            }
            SDL_Rect obj3 = {PlayerBullet.destino.x, PlayerBullet.destino.y, PlayerBullet.destino.w, PlayerBullet.destino.h};
            if(SDL_HasIntersection((&obj3), (&obj2)) && BlewUpSAT==0 && PlayerBullet.IndexImage != 381){ //Colisão de bala com bala.
                PlayerBullet.IndexImage=381;
                PlayerBullet.destino.h=38;
                PlayerBullet.destino.w=57;
                PlayerBullet.destino.x=EnemyBullet[i].destino.x-15;
                PlayerBullet.destino.y=EnemyBullet[i].destino.y;
                EnemyBullet[i].Ativo=0;
                BEIndex-=1;
                BlewUpSAT=1;
                TimeDestroyBullet_DeltaTime=0;
                Score+=5;
                Mix_PlayChannel( -1, BulletCollisionSound, 0 );
            }
        }
    }


}
void GameOver(){
    SDL_Rect fOrigem = {0, 0, 523, 158};
    SDL_Rect fDestino = {50, 100, 523, 158};
    SDL_RenderCopy(render, FimDeJogo, &fOrigem, &fDestino);
    SDL_RenderPresent(render);
    int qtd = SDL_PollEvent(&evento); //Variavel que armazena a leitura do teclado.
    do{
        qtd = SDL_PollEvent(&evento); //Variavel que armazena a leitura do teclado.
        TimeMenuSAT_DeltaTime+=1;
        if (qtd > 0 && TimeMenuSAT_DeltaTime>TimeMenuSAT){ //So sai da tela de game over se o jogador apertar <espaço>.
            if(evento.type == SDL_KEYDOWN){
                if(evento.key.keysym.sym == SDLK_SPACE){
                    break;
                }
            }
        }
    }while(1);
    TimeMenuSAT_DeltaTime = 0;
    EstadoJogo=0;
    GravarScore();
    InicializarJogo();
    ZerarFase();
}
void ShootEnemys(){ //Responsavel também pela movimentação da bala do inimigo.
    int i;

    TimeEnemyShoot_DeltaTime+=1;
    if (BEIndex < 5 && TimeEnemyShoot_DeltaTime>TimeEnemyShoot){ //Vê se a quantidade de tiros ja atingio o limite.
        TimeEnemyShoot_DeltaTime=0;
        TimeEnemyShoot=rand()% TimeEnemyShootMaxRand; //Sorteia uma nave para atirar
        int i, EnemyShoot;
        do{
            EnemyShoot = rand()% 15;
        }while(WhoCanShoot[EnemyShoot]!=1); //while que só sai quando é sorteado uma nave que pode atirar. 'se a debaixo estiver sdo destroida'
        for(i=0; i<15; i++){
            if(inimigos[i].MatrizX==EnemyShoot && inimigos[i].Ativo==1 && EnemyBullet[BEIndex].Ativo==0 //Vê se o inimigo pode atirar.
               && WhoCanShoot[inimigos[i].MatrizX+5]==0 && WhoCanShoot[inimigos[i].MatrizX+10]==0){
                EnemyBullet[BEIndex].destino.x=inimigos[i].destino.x+15;
                EnemyBullet[BEIndex].destino.y=inimigos[i].destino.y+50;
                EnemyBullet[BEIndex].Ativo=1;
                BEIndex+=1;
                break;
            }
        }
    }


    BuletEnemy_DeltaTime+=1;
    if(BuletEnemy_DeltaTime>BulletSpeed-DificuldadeJogo){
        BuletEnemy_DeltaTime=0;
        for(i=0;i<5;i++){
            if (EnemyBullet[i].Ativo == 1){
                EnemyBullet[i].destino.y += EnemyBulletY; //3
            }
            if (EnemyBullet[i].destino.y>642 && EnemyBullet[i].Ativo == 1){
                EnemyBullet[i].Ativo = 0;
                BEIndex-=1;
            }
        }
    }


}


//Função que carrega todas as imagens do jogo.
void CarregarImagens(){
    //Cenario.
    SDL_Surface * temp0 = IMG_Load("./Profile/Sprites/Cenarios.png");
    cenario = SDL_CreateTextureFromSurface(render, temp0);

    //Bullets.
    SDL_Surface * temp1 = IMG_Load("./Profile/Sprites/Bullets.png");
    bullet = SDL_CreateTextureFromSurface(render, temp1);

    //inimigos
    SDL_Surface * temp3 = IMG_Load("./Profile/Sprites/AllEnemys.png");
    enemys = SDL_CreateTextureFromSurface(render, temp3);

    //life sprite
    SDL_Surface * temp4 = IMG_Load("./Profile/Sprites/Life.png");
    SpritesPlayer = SDL_CreateTextureFromSurface(render, temp4);

    //seta sprite
    SDL_Surface * temp5 = IMG_Load("./Profile/Sprites/Seta.png");
    seta = SDL_CreateTextureFromSurface(render, temp5);

    //Fim de jogo.
    SDL_Surface * temp6 = IMG_Load("./Profile/Sprites/FimDeJogo.png");
    FimDeJogo = SDL_CreateTextureFromSurface(render, temp6);

    //Limpa as areas de memorias "temp's" criadas.
    SDL_FreeSurface(temp0); SDL_FreeSurface(temp1);
    SDL_FreeSurface(temp3); SDL_FreeSurface(temp4);
    SDL_FreeSurface(temp5); SDL_FreeSurface(temp6);
}
void CarregarSounds(){
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

    //Load the music
    ShootPlayerSound = Mix_LoadWAV  ( "./Profile/Sounds/ShootPlayer.wav" );
    ShootEnemySound = Mix_LoadWAV  ( "./Profile/Sounds/ShootEnemy.wav" );
    FastFaseSound = Mix_LoadWAV  ( "./Profile/Sounds/FaseFast.wav" );
    ExplisionPlayerSound = Mix_LoadWAV  ( "./Profile/Sounds/ExplisionPlayer.wav" );
    ExplosionEnemyrSound = Mix_LoadWAV  ( "./Profile/Sounds/ExplosionEnemy.wav" );
    BulletCollisionSound = Mix_LoadWAV  ( "./Profile/Sounds/BulletCollision.wav" );
    MenuPassSound = Mix_LoadWAV  ( "./Profile/Sounds/MenuPass.wav" );
    MenuSelectSound =  Mix_LoadWAV  ( "./Profile/Sounds/MenuSelect.wav" );

    Mix_VolumeChunk(ExplosionEnemyrSound, MIX_MAX_VOLUME / 1);

    if (ShootPlayerSound == NULL     || ShootEnemySound == NULL      || FastFaseSound == NULL ||
        ExplisionPlayerSound == NULL || ExplosionEnemyrSound == NULL  || BulletCollisionSound == NULL ||
        MenuPassSound == NULL        || MenuSelectSound == NULL){
        printf("Algums son nao foi localizada!!! \n");
    }
}

void Pontuacao(){ //Printa pontuação durante a jgatina.
    int w, h;
    char buf[10];
    sprintf(buf, "%i", Score);
    surText = TTF_RenderText_Solid(text_font, buf, font_color);
    text_Texto = SDL_CreateTextureFromSurface(render, surText);
    SDL_QueryTexture(text_Texto, NULL, NULL, &w, &h);
    SDL_Rect rOrigem = {0, 0, w, h};
	SDL_Rect rDestino = {375, 22, w, h};
    SDL_RenderCopy(render, text_Texto, &rOrigem, &rDestino);
    SDL_FreeSurface(surText);
    SDL_DestroyTexture(text_Texto);
    free(text_Texto);
}
void PrintHighScore(){
    FILE * arq;
    int w, h;
    char char_arq[9];
    arq = fopen("./Profile/Data/config.ShootGame","r");
    if (arq == NULL){
        surText = TTF_RenderText_Solid(text_font, "0", font_color);
    }
    else{
        fscanf(arq,"%s",char_arq);
        surText = TTF_RenderText_Solid(text_font, char_arq, font_color);
    }
    fclose(arq);
    text_Texto = SDL_CreateTextureFromSurface(render, surText);
    SDL_QueryTexture(text_Texto, NULL, NULL, &w, &h);
    SDL_Rect rOrigem = {0, 0, w, h};
	SDL_Rect rDestino = {52, 170, w, h};
    SDL_RenderCopy(render, text_Texto, &rOrigem, &rDestino);
    SDL_FreeSurface(surText);
    SDL_DestroyTexture(text_Texto);
    free(text_Texto);
}
void GravarScore(){
    FILE *arq;
    char char_arq[9];
    int int_arq;
    arq = fopen("./Profile/Data/config.ShootGame","r");
    if(arq == NULL){
        fclose(arq);
        arq = fopen("./Profile/Data/config.ShootGame","w");
        fprintf(arq,"%d",Score);
    }
    else{
        fscanf(arq, "%s", char_arq);
        int_arq = atoi(char_arq);
        if(int_arq < Score){
            fclose(arq);
            arq = fopen("./Profile/Data/config.ShootGame","w");
            fprintf(arq,"%d",Score);
        }
    }

    fclose(arq);
}
void DificuldadeFunction(){ //Do menu de dificuldadde.
    int w, h, SAT=0;
    char buf[10];
    sprintf(buf, "%i", DificuldadeJogo);
    surText = TTF_RenderText_Solid(text_font, buf, font_color);
    text_Texto = SDL_CreateTextureFromSurface(render, surText);
    SDL_QueryTexture(text_Texto, NULL, NULL, &w, &h);
    SDL_Rect rOrigem = {0, 0, w, h};
    if(DificuldadeJogo>9){
        SAT = -10;
    }
    SDL_Rect rDestino = {310+SAT, 236, w, h};
    SDL_RenderCopy(render, text_Texto, &rOrigem, &rDestino);
    SDL_FreeSurface(surText);
    SDL_DestroyTexture(text_Texto);
    free(text_Texto);
}

void Printar(){
    int i;
    // Limpar a tela
    SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
    SDL_RenderClear(render);

    //Printa o cenario
    SDL_Rect cOrigem = {CenariosIbdex[EstadoJogo], 0, 558, 465};
    SDL_Rect cDestino = {0, 0, 640, 480};
    SDL_RenderCopy(render, cenario, &cOrigem, &cDestino);

    if(EstadoJogo==0){
        //Printa o seta do menu
        SDL_Rect sOrigem = {0, 0, 64, 49};
        SDL_Rect sDestino = {7, SetaIndex[Seta],  32, 27};
        SDL_RenderCopy(render, seta, &sOrigem, &sDestino);
    }

    if(EstadoJogo==1){
        //Printar as vida do jogador.
        SDL_Rect plOrigem = {0, 0, LifeIndex, 44};
        SDL_Rect plDestino = {PlayerLife.x, PlayerLife.y, LifeIndex, PlayerLife.h};
        SDL_RenderCopy(render, SpritesPlayer, &plOrigem, &plDestino);

        //Printar a bala do inimigo na tela.
        for(i=0;i<5;i++){
            if(EnemyBullet[i].Ativo == 1){
                SDL_Rect ibOrigem = {127, 0, 127, 78};
                SDL_Rect ibDestino = {EnemyBullet[i].destino.x, EnemyBullet[i].destino.y,
                EnemyBullet[i].destino.w, EnemyBullet[i].destino.h}; //Continuação da linha acima.
                SDL_RenderCopy(render, bullet, &ibOrigem, &ibDestino);
            }

        }

        //Printa a bala do player na tela se ela estiver ativada.
        TimeDestroyBullet_DeltaTime+=1;
        if(TimeDestroyBullet_DeltaTime>TimeDestroyBullet && BlewUpSAT==1){
            BlewUpSAT=0;
            PlayerBullet.Ativo=0;
            PlayerBullet.IndexImage=0;
            PlayerBullet.destino.h=14;
            PlayerBullet.destino.w=7;
        }
        if(PlayerBullet.Ativo == 1){
            SDL_Rect pbOrigem = {PlayerBullet.IndexImage, 0, 127, 78};
            SDL_Rect pbDestino = {PlayerBullet.destino.x, PlayerBullet.destino.y,
            PlayerBullet.destino.w, PlayerBullet.destino.h}; //Continuação da linha acima.
            SDL_RenderCopy(render, bullet, &pbOrigem, &pbDestino);
        }

        //Printar o playere
        PlayerDeadTime_DeltaTime+=1;
        if(PlayerIsDead==1 && PlayerDeadTime_DeltaTime>PlayerDeadTime && PlayerImuni==0){
            PlayerLastSprite0=192;
            Mathews.IndexImage=192;
            PlayerIsDead=0;
            Mathews.x=270; Mathews.y=430;
            PlayerImuni=1;
            PlayerLastSprite=320;
            PlayerDeadTime_DeltaTime=0;

        }
        if(PlayerDeadTime_DeltaTime>PlayerDeadTime*2 && PlayerImuni==1){
            PlayerLastSprite=192;
            PlayerImuni=0;
        }
        SDL_Rect pOrigem = {Mathews.IndexImage, 0, 64, 44};
        SDL_Rect pDestino = {Mathews.x, Mathews.y,
                        Mathews.w, Mathews.h}; //Continuação da linha acima.
        SDL_RenderCopy(render, SpritesPlayer, &pOrigem, &pDestino);



        //Printar a movimentação base na tela.
        //SDL_Rect mbOrigem = {0, 0, 54, 55};
        //SDL_Rect mbDestino = {MoveBase.destino.x, MoveBase.destino.y, MoveBase.destino.w, MoveBase.destino.h};
        //SDL_RenderCopy(render, SpritesPlayer, &mbOrigem, &mbDestino);

        TimeChangeSprite_DeltaTime+=1;
        if(TimeChangeSprite_DeltaTime>TimeChangeSprite-(Dificuldade*3)-DificuldadeJogo){
            if (PlayFastFaseSound==1){
                PlayFastFaseSound=0;
                Mix_PlayChannel( -1, FastFaseSound, 0 );
            }
            else{
                PlayFastFaseSound=1;
            }

            TimeChangeSprite_DeltaTime=0;
            if(WitchSprite==0){
                WitchSprite=1;
                InitialPint=58;
                InitialPint0=59;
                Mathews.IndexImage=PlayerLastSprite0;
            }
            else{
                WitchSprite=0;
                InitialPint=0;
                InitialPint0=0;
                Mathews.IndexImage=PlayerLastSprite;
            }
        }

        //Printar inimigos.
        for(i=0; i<15; i++){
            if(inimigos[i].Ativo == 1){
                SDL_Rect inOrigem = {InitialPint+inimigos[i].PointOrigem, 0, 58, 58};
                SDL_Rect inDestino = {inimigos[i].destino.x, inimigos[i].destino.y,
                inimigos[i].destino.w, inimigos[i].destino.h}; //Continuação da linha acima.
                SDL_RenderCopy(render, enemys, &inOrigem, &inDestino);
            }
        }
        Pontuacao();
    }

    if(EstadoJogo==2){
        //Printa o seta do menu
        SDL_Rect sOrigem = {0, 0, 64, 49};
        SDL_Rect sDestino = {7, 410,  32, 27};
        SDL_RenderCopy(render, seta, &sOrigem, &sDestino);
        PrintHighScore();
    }

    if(EstadoJogo==3){
        DificuldadeFunction();
    }

    if(EstadoJogo==4){
        //Printa o seta do menu
        SDL_Rect sOrigem = {0, 0, 64, 49};
        SDL_Rect sDestino = {7, 410,  32, 27};
        SDL_RenderCopy(render, seta, &sOrigem, &sDestino);
    }

    SDL_RenderPresent(render);
}

void FistTela(){
    char Tecla;
    // Limpar a tela
    SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
    SDL_RenderClear(render);

    //Printa o cenario
    SDL_Rect cOrigem = {2790, 0, 558, 465};
    SDL_Rect cDestino = {0, 0, 640, 480};
    SDL_RenderCopy(render, cenario, &cOrigem, &cDestino);

    SDL_RenderPresent(render);

    do{ //Do while true para que so sai quando o jogador precionar <espaço>
        int qtd = SDL_PollEvent(&evento); //Variavel que armazena a leitura do teclado.
        if (qtd > 0 ){
            if(evento.type == SDL_KEYDOWN){
                if(evento.key.keysym.sym == SDLK_SPACE){
                    TimeMenuSAT_DeltaTime=0;
                    break;
                }
            }
        }
    }while(1);
}
void Menu(){
    Printar();
    CapturarTeclado();
}
void Start(){
    CapturarTeclado();
    Mecanica();
    ShootEnemys();
    Collision(); //Ve se ouve algum tipo de colisão.
    Printar(); //Printa na tela todas as imagens carregadas e trada ela juntamento com o texto da pontuação.
}
void HighScore(){
    CapturarTeclado();
    Printar();
}
void Options(){
    CapturarTeclado();
    Printar();
}
void Credits(){
    Printar();
    CapturarTeclado();
}

int main(int c, char *arts[]){
    srand(time(NULL));
    TimeEnemyShoot=rand()% TimeEnemyShootMaxRand-DificuldadeJogo;
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("ShootingGame", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if(janela != NULL){
        render = SDL_CreateRenderer(janela, -1, 0);
        if(render != NULL && TTF_Init() >=0){
            text_font = TTF_OpenFont("./Profile/Fonts/consola.ttf", 32);
            if(text_font == NULL){
                printf("ERRO, font `consola.ttf` inexistente!!!! \n\n *_* \n\n");
            }
            CarregarImagens();
            CarregarSounds();
            InicializarJogo();
            ZerarFase();
            FistTela();
            while(1){
                if(EstadoJogo==0){
                    Menu();
                }
                else if(EstadoJogo==1){
                    Start();
                    if(EnemysDeads>=15){ //if para reiniciar a fase após matar todos inimigos
                        InicializarJogo();
                        EnemysDeads=0;
                        Dificuldade=0;
                        LastDown=1;
                        BEIndex=0;
                    }
                    if(LifeIndex<0){ //Caso o jogador morra perda todas as vidas, volta para o menu.
                        TimeMenuSAT_DeltaTime = 0;
                        GameOver();
                    }
                }
                else if(EstadoJogo==2){
                    HighScore();
                }
                else if(EstadoJogo==3){
                    Options();
                }
                else if(EstadoJogo==4){
                    Credits();
                }
           }
        }
    }
    Mix_Quit();
    Mix_CloseAudio();
}
