#include "COffscreenRenderer.h"

class CMapaCaracteres{

private:

    int *alturaLetra;
    int *larguraLetra;
    int altTela;

    SDL_Renderer *render;
    SDL_Texture **glyphsT;
    TTF_Font *font;

    TTF_Font *IniciaFonte(char *nomeFonte,SDL_Renderer* renderer,int alturaTela, int tamanhoFonte,int estilo){
        strcpy(nome,nomeFonte);
        tamFonte = tamanhoFonte;
        render = renderer;
        altTela = alturaTela;

        alturaLetra = (int*)malloc(sizeof(int)*(ULTIMO_CAR-PRIMEIRO_CAR));
        larguraLetra = (int*)malloc(sizeof(int)*(ULTIMO_CAR-PRIMEIRO_CAR));

        glyphsT = (SDL_Texture**) malloc(sizeof(SDL_Texture*)*(ULTIMO_CAR-PRIMEIRO_CAR));

        TTF_Font *font = TTF_OpenFont( nome, tamanhoFonte );
        if (font==NULL)
            printf("Erro: arquivo de fonte (%s) nao localizado\n",nomeFonte);
        TTF_SetFontStyle(font,estilo);
        return font;
    }

    void IniciaBase(SDL_Surface *surfaceTemp[],OffscreenRenderer off, SDL_Rect &rect){
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            glyphsT[j-PRIMEIRO_CAR] = SDL_CreateTextureFromSurface(off->GetRenderer(),surfaceTemp[j-PRIMEIRO_CAR]);
            SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            rect.w=larguraLetra[j-PRIMEIRO_CAR];
            SDL_RenderCopy(off->GetRenderer(),glyphsT[j-PRIMEIRO_CAR],NULL,&rect);
            SDL_DestroyTexture(glyphsT[j-PRIMEIRO_CAR]);
            rect.x += rect.w;
        }
    }

    void FazGlyphs(SDL_Surface *surfaceTemp[],TTF_Font *font,int &altura,int &largura,PIG_Cor corFonte){

        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            surfaceTemp[j-PRIMEIRO_CAR] = TTF_RenderGlyph_Solid(font,(Uint16)j,corFonte);
            alturaLetra[j-PRIMEIRO_CAR] = surfaceTemp[j-PRIMEIRO_CAR]->h;
            if (surfaceTemp[j-PRIMEIRO_CAR]->h>altura)
                altura = surfaceTemp[j-PRIMEIRO_CAR]->h;
            larguraLetra[j-PRIMEIRO_CAR] = surfaceTemp[j-PRIMEIRO_CAR]->w;
            largura+=surfaceTemp[j-PRIMEIRO_CAR]->w;
        }

    }

    void CriaFinal(OffscreenRenderer off,TTF_Font *font,SDL_Rect &rect,SDL_Rect &dest,bool outline,PIG_Cor corOutline,PIG_Cor *corFonte=NULL){
        SDL_Surface *surfaceTemp[ULTIMO_CAR-PRIMEIRO_CAR];

        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            dest.h=alturaLetra[j-PRIMEIRO_CAR];
            rect.w=dest.w=larguraLetra[j-PRIMEIRO_CAR];
            surfaceTemp[j-PRIMEIRO_CAR] = SDL_CreateRGBSurface(0,larguraLetra[j-PRIMEIRO_CAR],alturaLetra[j-PRIMEIRO_CAR],24,0,0,0,0);

            SDL_BlitSurface(off->GetSurface(),&rect,surfaceTemp[j-PRIMEIRO_CAR],&dest);
            if (outline){
                SDL_Surface *out = TTF_RenderGlyph_Solid(font,(Uint16)j,corOutline);
                SDL_BlitSurface(out,NULL,surfaceTemp[j-PRIMEIRO_CAR],&dest);
                SDL_FreeSurface(out);
            }
            if (corFonte!=NULL&&CORESIGUAIS(*corFonte,BRANCO)){
                SDL_SetColorKey(surfaceTemp[j-PRIMEIRO_CAR],SDL_TRUE,SDL_MapRGBA(surfaceTemp[j-PRIMEIRO_CAR]->format, 0,0,0,255));//ROXO
            }else SDL_SetColorKey(surfaceTemp[j-PRIMEIRO_CAR],SDL_TRUE,SDL_MapRGBA(surfaceTemp[j-PRIMEIRO_CAR]->format, 255,255,255,255));//ROXO

            //SDL_SetColorKey(surfaceTemp[j-PRIMEIRO_CAR],SDL_TRUE,SDL_MapRGBA(surfaceTemp[j-PRIMEIRO_CAR]->format, 255,255,255,255));//ROXO
            glyphsT[j-PRIMEIRO_CAR] = SDL_CreateTextureFromSurface(render,surfaceTemp[j-PRIMEIRO_CAR]);
            SDL_FreeSurface(surfaceTemp[j-PRIMEIRO_CAR]);
            rect.x+=larguraLetra[j-PRIMEIRO_CAR];
        }

    }

    void AplicaStencil(OffscreenRenderer off, SDL_Rect &rect,char *fundoFonte){
        SDL_Surface* stencil = IMG_Load(NOME_ARQ_TEMP);
        SDL_SetColorKey(stencil,SDL_TRUE,SDL_MapRGBA(stencil->format, 0,0,0,255));
        SDL_Texture *textStencil = SDL_CreateTextureFromSurface(off->GetRenderer(),stencil);

        SDL_Surface *fundo = IMG_Load(fundoFonte);
        SDL_Texture *textFundo = SDL_CreateTextureFromSurface(off->GetRenderer(),fundo);

        rect.w = fundo->w;
        rect.x = 0;
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            rect.w=larguraLetra[j-PRIMEIRO_CAR];
            SDL_RenderCopy(off->GetRenderer(),textFundo,NULL,&rect);
            rect.x+=larguraLetra[j-PRIMEIRO_CAR];
        }
        SDL_RenderCopy(off->GetRenderer(),textStencil,NULL,NULL);

    }

    std::string TrataRetornoSeparaPalavraCaracterEspecial(int posAtual,int &posAnt,std::string txt,std::string Retorno){
        std::string aux;

        if(posAtual == posAnt){
            posAnt+=1;
            return Retorno;
        }

        aux.assign(txt,posAnt,posAtual - posAnt);
        posAnt = posAtual;
        return aux;

   }

    std::string SeparaPalavra(int &posAnt,std::string txt){

        if(posAnt == txt.size()) return "";

        for(int i = posAnt;i<txt.size();i++){
            if(txt[i] == '\n') return TrataRetornoSeparaPalavraCaracterEspecial(i,posAnt,txt,"\n");
            if(txt[i] == ' ') return TrataRetornoSeparaPalavraCaracterEspecial(i,posAnt,txt," ");
        }

        std::string aux;
        aux.assign(txt,posAnt,txt.size() - posAnt);
        posAnt = txt.size();
        return aux;

    }

public:

    char nome[100];
    int tamFonte;

    CMapaCaracteres(char *nomeFonte,SDL_Renderer* renderer,int alturaTela,int tamanhoFonte,int estilo, PIG_Cor corFonte,int outline,PIG_Cor corOutline){
        font = IniciaFonte(nomeFonte,renderer,alturaTela,tamanhoFonte,estilo);

        SDL_Surface *surfaceTemp[ULTIMO_CAR-PRIMEIRO_CAR];
        int altRend=0,largRend=0;

        FazGlyphs(surfaceTemp,font,altRend,largRend,corFonte);

        SDL_Rect rect;
        rect.x=0;
        rect.y=0;
        rect.h=altRend;

        OffscreenRenderer off = new COffscreenRenderer(altRend,largRend);
        if (CORESIGUAIS(corFonte,BRANCO)){
            off->PintarFundo(PRETO);
        }else off->PintarFundo(BRANCO);
        IniciaBase(surfaceTemp,off,rect);

        off->SalvarImagemBMP(NOME_ARQ_TEMP);

        rect.x=0;
        rect.y=0;
        rect.h=altRend;
        SDL_Rect dest;
        dest.x=dest.y=0;

        TTF_SetFontOutline(font,outline);

        CriaFinal(off,font,rect,dest,outline,corOutline,&corFonte);

        //TTF_CloseFont(font);
        delete off;
        remove(NOME_ARQ_TEMP);
    }

    CMapaCaracteres(char *nomeFonte,SDL_Renderer* renderer,int alturaTela,int tamanhoFonte,int estilo, char *fundoFonte,int outline,PIG_Cor corOutline){
        font = IniciaFonte(nomeFonte,renderer,alturaTela,tamanhoFonte,estilo);

        SDL_Surface *surfaceTemp[ULTIMO_CAR-PRIMEIRO_CAR];
        int altOffScreen=0,largOffScreen=0;

        FazGlyphs(surfaceTemp,font,altOffScreen,largOffScreen,PRETO);

        SDL_Rect rect;
        rect.x =0;
        rect.y =0;
        rect.h = altOffScreen;

        OffscreenRenderer off = new COffscreenRenderer(altOffScreen,largOffScreen);
        off->PintarFundo(BRANCO);
        IniciaBase(surfaceTemp,off,rect);

        rect.w = 1;
        rect.h = altOffScreen;
        rect.x = 0;
        rect.y = 0;
        SDL_FillRect(off->GetSurface(),&rect,SDL_MapRGB(off->GetSurface()->format,0,0,0));

        //off->EncerrarDesenho();
        off->SalvarImagemBMP(NOME_ARQ_TEMP);
        off->PintarFundo(BRANCO);

        AplicaStencil(off,rect,fundoFonte);

        off->SalvarImagemBMP(NOME_ARQ_TEMP);

        rect.x = 0;
        rect.y = 0;
        rect.h = altOffScreen;
        SDL_Rect dest;
        dest.x = dest.y = 0;

        TTF_SetFontOutline(font,outline);
        CriaFinal(off,font,rect,dest,outline,corOutline);

        //TTF_CloseFont(font);
        delete off;

        remove(NOME_ARQ_TEMP);
    }

    ~CMapaCaracteres(){
        TTF_CloseFont(font);
        for (Uint16 j=PRIMEIRO_CAR;j<ULTIMO_CAR;j++){
            SDL_DestroyTexture(glyphsT[j-PRIMEIRO_CAR]);
        }
        free(glyphsT);
        free(larguraLetra);
        free(alturaLetra);
    }

    std::vector<std::string> ExtraiLinhasString(std::string texto,int largMax){
        std::vector<std::string> linhas;
        std::string linhaAtual("");
        std::string linhaMaior("");
        std::string palavra = "";
        int posAnt=0;

        const std::string QUEBRALINHA = "\n";

        palavra = SeparaPalavra(posAnt,texto);

        while (palavra != ""){

            linhaMaior = linhaAtual + palavra;

            if(palavra == QUEBRALINHA){
                linhas.push_back(linhaAtual);
                linhaAtual = QUEBRALINHA;
            }else{

                if (GetLarguraPixelsString((char*)linhaMaior.c_str()) >largMax){

                    if(linhaAtual == QUEBRALINHA){
                        linhas.push_back(linhaMaior);
                        linhaAtual ="";
                    }else{
                        if(linhaAtual!=""){linhas.push_back(linhaAtual);}
                        linhaAtual = palavra;
                    }

                }else{
                    linhaAtual = linhaMaior;
                }
            }
            palavra = SeparaPalavra(posAnt,texto);
        }
        linhas.push_back(linhaAtual);
        return linhas;
    }

    int GetLarguraPixelsString(char *str){
        int resp=0;
        Uint16 aux;

        for (int i=0;i<strlen(str);i++){
            aux = str[i];
            aux = aux % 256;
            if(str[i] != '\n')resp += larguraLetra[aux-PRIMEIRO_CAR];
        }

        return resp;
    }

    void EscreveStringCentralizado(char *str,int x,int y){
        int larguraPixels = GetLarguraPixelsString(str);
        EscreveStringEsquerda(str,x-larguraPixels/2,y);
    }

    void EscreveStringEsquerda(char *str,int x,int y){
        SDL_Rect rectDestino;
        rectDestino.x = x;
        rectDestino.y = altTela-y-tamFonte;

        Uint16 aux;
        for (int i=0;i<strlen(str);i++){
            aux = str[i];
            aux = aux % 256;//UTF16 string, retirando só o byte que interessa

            rectDestino.w = larguraLetra[aux-PRIMEIRO_CAR];
            rectDestino.h = alturaLetra[aux-PRIMEIRO_CAR];
            //rectDestino.y = altTela-y-rectDestino.h;

            SDL_RenderCopy(render,glyphsT[aux-PRIMEIRO_CAR],NULL,&rectDestino);
            //desenhar um retangulo em volta da letra???
            //SDL_RenderDrawRect(render,&rectDestino);

            //mudar de cor individualmente
            //SDL_SetTextureColorMod(glyphsT[aux-PRIMEIRO_CAR],0,0,0);

            rectDestino.x += rectDestino.w;
        }
    }

    void EscreveStringDireita(char *str,int x,int y){
        int larguraPixels = GetLarguraPixelsString(str);
        EscreveStringEsquerda(str,x-larguraPixels,y);
    }

    void EscreveStringLongaCentralizado(std::string texto,int x,int y,int largMax,int espacoEntreLinhas){
        std::vector<std::string> linhas = ExtraiLinhasString(texto,largMax);
        int yTotal=y;
        for (int k=0;k<linhas.size();k++){
            int larguraPixels = GetLarguraPixelsString((char*)linhas[k].c_str());
            EscreveStringEsquerda((char*)linhas[k].c_str(),x-larguraPixels/2,yTotal);
            yTotal -= espacoEntreLinhas;
        }
    }

    int GetAlturaLetra(char letra){

        Uint16 aux = letra;
        aux = aux % 256;
        return alturaLetra[aux-PRIMEIRO_CAR];

    }

    int GetLarguraLetra(char letra){

        Uint16 aux = letra;
        aux = aux % 256;
        return larguraLetra[aux-PRIMEIRO_CAR];

    }

    int GetTamanhoFonte(){

        return tamFonte;

    }

    void EscreveStringLongaEsquerda(std::string texto,int x,int y,int largMax,int espacoEntreLinhas){
        SDL_Rect rectDestino;
        int linhaAtual =0;
        std::vector<std::string> linhas = ExtraiLinhasString(texto,largMax);

        int altOriginal = altTela-y-tamFonte;

        for (int k=0;k<linhas.size();k++){

            rectDestino.x = x;
            rectDestino.y = altOriginal;

            Uint16 aux;

            for (int i=0;i<strlen(linhas[k].c_str());i++){

                if(linhas[k][i] != '\n'){

                    aux = linhas[k][i];
                    aux = aux % 256;

                    rectDestino.y = altOriginal + (espacoEntreLinhas*linhaAtual);

                    rectDestino.w = larguraLetra[aux-PRIMEIRO_CAR];
                    rectDestino.h = alturaLetra[aux-PRIMEIRO_CAR];

                    SDL_RenderCopy(render,glyphsT[aux-PRIMEIRO_CAR],NULL,&rectDestino);

                    rectDestino.x += rectDestino.w;

                }

            }

            linhaAtual++;
        }
    }

    void EscreveStringLongaDireita(std::string texto,int x,int y,int largMax,int espacoEntreLinhas){
        std::vector<std::string> linhas = ExtraiLinhasString(texto,largMax);
        int yTotal=y;
        for (int k=0;k<linhas.size();k++){
            int larguraPixels = GetLarguraPixelsString((char*)linhas[k].c_str());
            EscreveStringEsquerda((char*)linhas[k].c_str(),x-larguraPixels,y);
            yTotal -= espacoEntreLinhas;
        }
    }

    SDL_Surface *GetGlyph(char *emoji){
        Uint16* text;
        text = (Uint16*)u"Hello World! \u2602";
        return TTF_RenderUTF8_Solid(font,emoji,{255,0,0,255});
    }
};

typedef CMapaCaracteres* MapaCaracteres;
