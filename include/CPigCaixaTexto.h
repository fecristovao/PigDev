class CPigCaixaTexto: public CPigComponente{

private:

int OnMouseOn(){} //nao é necessário fazer nada extra quando o mouse fica sobre o componente
int OnMouseOff(){}//nao é necessário fazer nada extra quando o mouse sai de cima do componente

char texto[300], label[200];
int fonteTexto,fonteLabel;
PIG_PosicaoComponente posLabel;
int posCursor;
int xBase,xCursor;
int labelX,labelY;
int margemVert,margemHor;
bool cursorExibido;
int maxCaracteres;
char mascara;
bool textoSenha;
bool somenteNumeros;
PIG_Cor corCursor;
Timer timer;

void AjustaAlinhamento(){
    char aux[300]="";
    std::string textoBase(texto);
    if (textoSenha){
        textoBase = GetTextoMask();
    }
    xBase = x+margemHor;//a base é a esquerda do componente
    strncpy(aux,(char*)textoBase.c_str(),posCursor);
    int largParcial = CalculaLarguraPixels(aux,fonteTexto);
    xCursor = xBase + largParcial;//calcula quantos pixels a frente da base ficará o cursor

    while (xCursor>x+larg-margemHor){//se o cursor ficar "depois do fim" do componente, desloca a base para esquerda
        xBase-=5;
        xCursor = xBase + largParcial;
    }

    while (xCursor<x+margemHor){//se o cursor ficar "antes do início" do componente, desloca a base para direita
        xBase+=5;
        xCursor = xBase + largParcial;
    }
}

int AvancaCursor(){
    if (posCursor>=strlen(texto)) return 0;
    posCursor++;
    AjustaAlinhamento();
    return 1;
}

int VoltaCursor(){
    if (posCursor==0) return 0;
    posCursor--;
    AjustaAlinhamento();
    return 1;
}

int RetiraTextoBackSpace(){
    if (strlen(texto)==0||posCursor==0) return 0;

    for (int i=posCursor-1;i<strlen(texto);i++)
        texto[i]=texto[i+1];

    VoltaCursor();
    if (audio>=0) PlayAudio(audio);
    return 1;
}

int RetiraTextoDelete(){
    if (strlen(texto)==0||posCursor>=strlen(texto)) return 0;

    for (int i=posCursor;i<strlen(texto);i++)
        texto[i]=texto[i+1];

    if (audio>=0) PlayAudio(audio);
    return 1;
}

std::string GetTextoMask(){
    std::string resp;
    for (int i=0;i<strlen(texto);i++)
        resp += mascara;

    return resp;
}

void EscreveTexto(){
    if (textoSenha){
        EscreverEsquerda((char*)GetTextoMask().c_str(),xBase,y+margemVert,fonteTexto);
    }else EscreverEsquerda(texto,xBase,y+margemVert,fonteTexto);
}

int AdicionaTexto(char *frase){
    if (strlen(texto)+strlen(frase)>maxCaracteres) return 0;
    if (somenteNumeros&&!SomenteNumeros(frase)) return 0;

    char fim[300]="";
    strcpy(fim,&texto[posCursor]);
    texto[posCursor]='\0';
    strcat(texto,frase);
    strcat(texto,fim);

    AvancaCursor();
    return 1;
}

std::string ConverteString(char *str){
    std::string resp(str);
    if (strlen(str)>1){
        if ((uint8_t)str[0]==195){
            resp = str[1]+64;
        }else if ((uint8_t)str[0]==194){
            resp = str[1];
        }

    }
    return resp;
}

int TrataEventoMouse(PIG_Evento evento){
    if (evento.mouse.acao!=MOUSE_PRESSIONADO) return 0;
    SDL_Point p;
    CMouse::PegaXY(p.x,p.y);

    SDL_Rect r = {x+margemHor,y,larg-2*margemHor,alt};
    if (SDL_PointInRect(&p,&r)){
        //printf("dentro\n");
        int delta = p.x-xBase;
        //printf("delta %d\n",delta);
        int largParcial = 0;
        char aux[2]="";
        for (int i=0;i<=strlen(texto);i++){
            //char aux[300]="";
            //strncpy(aux,texto,i);
            largParcial += CalculaLarguraPixels(aux,fonteTexto);
            //printf("Pos %d (larg %d %s)\n",i,largParcial,aux);
            if (delta<largParcial){
                posCursor = i-1;
                AjustaAlinhamento();
                return 1;
            }
            aux[0] = texto[i];
        }
        posCursor = strlen(texto);
        AjustaAlinhamento();
        //printf("Achei pos %d (larg %d %s)\n",i,largParcial,aux);
        if (estado==COMPONENTE_NORMAL)
            DefineEstado(COMPONENTE_EDITANDO);

        return 1;
    }
    return 0;
}

int TrataEventoTeclado(PIG_Evento evento){
    if (evento.teclado.acao==TECLA_EDICAO){
        return 1;
    }else if (evento.teclado.acao==TECLA_INPUT){
        if (AdicionaTexto((char*)ConverteString(evento.teclado.texto).c_str())){
            if (audio>=0) PlayAudio(audio);
            return 1;
        }else return 0;
    }else if (evento.teclado.acao==TECLA_PRESSIONADA){
        switch (evento.teclado.tecla){
        case TECLA_BACKSPACE:
            RetiraTextoBackSpace();break;
        case TECLA_DELETE:
            RetiraTextoDelete();break;
        case TECLA_DIREITA:
            AvancaCursor();break;
        case TECLA_ESQUERDA:
            VoltaCursor();break;
        }
        return 1;
    }
    return 0;
}

public:

CPigCaixaTexto(int idComponente,int px, int py, int alt,int larg,char *nomeArq, int maxCars, bool apenasNumeros=false, int retiraFundo=1,int janela=0):CPigComponente(idComponente,px,py,alt,larg,nomeArq,retiraFundo,janela){
    strcpy(texto,"");//sem texto definido
    strcpy(label,"");//sem label definido
    posLabel = COMPONENTE_ESQUERDA;//posição padrão do label
    fonteLabel = fonteTexto = 0;//fonte padrão
    posCursor = 0;//cursor no início do texto
    cursorExibido = true;
    timer = NULL;//o timer só será criado quando estiver editando
    estado = COMPONENTE_NORMAL;
    xBase = x+5;
    xCursor = xBase;
    maxCaracteres = maxCars;
    mascara = '*';
    somenteNumeros = apenasNumeros;
    textoSenha = false;
    margemHor = 5;
    margemVert = 10;
    corCursor = PRETO;
}



~CPigCaixaTexto(){
    if (timer) delete timer;
}

int SetTexto(char *frase){
    strcpy(texto,"");
    posCursor=0;
    AdicionaTexto(frase);
    return 1;
}

int SetLabel(char *frase){
    strcpy(label,frase);
    return 1;
}

void GetTexto(char *buffer){
    strcpy(buffer,texto);
}

void SetCorCursor(PIG_Cor cor){
    corCursor = cor;
}

void SetCampoSenha(bool campoSenha, char mask='*'){
    textoSenha = campoSenha;
    mascara = mask;
}

void SetPosicaoPadraoLabel(PIG_PosicaoComponente pos){
    posLabel = pos;
}

void SetPosicaoPersonalizadaLabel(int rx, int ry){
    labelX = rx;
    labelY = ry;
}

void SetMargens(int vertical, int horizontal){
    margemVert = vertical;
    margemHor = horizontal;
    AjustaAlinhamento();
}

int Desenha(){
    //imagem de fundo
    SDL_RenderCopyEx(renderer, text, &frame,&dest,-angulo,&pivoRelativo,flip);

    //bloqueia a area fora do componente
    SDL_Rect r={x+margemHor-2,altJanela-y-alt,larg-2*(margemHor-2),alt};
    SDL_RenderSetClipRect(renderer,&r);

    EscreveTexto();

    //trata o desenho do cursor
    if (estado==COMPONENTE_EDITANDO){
        if (cursorExibido){
            DesenhaLinhaSimples(xCursor,y+margemHor,xCursor,y+alt-margemHor,corCursor,idJanela);
        }
        if (timer&&timer->GetTempoDecorrido()>1){
            cursorExibido = !cursorExibido;
            timer->Reinicia(false);
        }
    }

    //desbloqueia o desenho fora da area do componente
    SDL_RenderSetClipRect(renderer,NULL);

    //desenha o label
    if (strcmp(label,""))
        int px,py;
        switch(posLabel){
        case COMPONENTE_CIMA:
            EscreverCentralizada(label,x+larg/2,y+alt+5,fonteLabel);
            break;
        case COMPONENTE_CIMADIR:
            EscreverDireita(label,x+larg,y+alt+5,fonteLabel);
            break;
        case COMPONENTE_CIMAESQ:
            EscreverEsquerda(label,x,y+alt+5,fonteLabel);
            break;
        case COMPONENTE_BAIXO:
            EscreverCentralizada(label,x+larg/2,y-30,fonteLabel);
            break;
        case COMPONENTE_BAIXODIR:
            EscreverDireita(label,x+larg,y-30,fonteLabel);
            break;
        case COMPONENTE_BAIXOESQ:
            EscreverEsquerda(label,x,y-30,fonteLabel);
            break;
        case COMPONENTE_ESQUERDA:
            EscreverDireita(label,x-5,y+margemVert,fonteLabel);
            break;
        case COMPONENTE_DIREITA:
            EscreverEsquerda(label,x+larg+5,y+margemVert,fonteLabel);
            break;
        case COMPONENTE_POSICAO_PERSONALIZADA:
            EscreverEsquerda(label,x+labelX,y+labelY,fonteLabel);
        }
}

int TrataEvento(PIG_Evento evento){
    if (evento.tipoEvento==EVENTO_MOUSE){
        return TrataEventoMouse(evento);
    }else if (evento.tipoEvento==EVENTO_TECLADO){
        return TrataEventoTeclado(evento);
    }
}

void DefineEstado(PIG_EstadoComponente estadoComponente){
    estado = estadoComponente;
    if (estado==COMPONENTE_EDITANDO){
        timer = new CTimer(false);
        SDL_StartTextInput();
    }else if (estado==COMPONENTE_NORMAL){
        SDL_StopTextInput();
        if (timer) delete timer;
    }
}

void Move(int nx, int ny){
    CPigComponente::Move(nx,ny);
    AjustaAlinhamento();
}

};

typedef CPigCaixaTexto *PigCaixaTexto;
