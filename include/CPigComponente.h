typedef enum {COMPONENTE_NORMAL,COMPONENTE_MOUSEOVER,COMPONENTE_ACIONADO,COMPONENTE_DESABILITADO,COMPONENTE_EDITANDO,COMPONENTE_INVISIVEL} PIG_EstadoComponente;
typedef enum {COMPONENTE_CIMA,COMPONENTE_CIMADIR,COMPONENTE_DIREITA,COMPONENTE_BAIXODIR,COMPONENTE_BAIXO,COMPONENTE_BAIXOESQ,COMPONENTE_ESQUERDA,COMPONENTE_CIMAESQ,COMPONENTE_POSICAO_PERSONALIZADA} PIG_PosicaoComponente;
class CPigComponente: public CVisual{

protected:

PIG_EstadoComponente estado;
char *hint;
int fonteHint;
int audio;
int antesOn,agoraOn;
int id;

virtual int OnMouseOn()=0;
virtual int OnMouseOff()=0;

void IniciaBase(int idComponente, int px, int py){
    id = idComponente;
    hint = NULL;
    fonteHint = 0;
    audio = -1;
    estado = COMPONENTE_NORMAL;
    x = px;
    y = py;
    agoraOn = antesOn = 0;
}

bool SomenteNumeros(char *frase){
    for (int i=0;i<strlen(frase);i++)
        if (frase[i]<'0'||frase[i]>'9')
            return false;
    return true;
}

public:

//construtor temporário (apenas para compilação funcionar neste momento) - remover depois


CPigComponente(int idComponente,int px,int py, int altura, int largura, int janela=0):CVisual(altura, largura, "",janela){
    IniciaBase(idComponente,px,py);
}

CPigComponente(int idComponente,int px,int py, int altura, int largura, char *nomeArq,int retiraFundo=1,int janela=0):CVisual(nomeArq,retiraFundo,NULL,janela){
    IniciaBase(idComponente,px,py);
    CVisual::SetDimensoes(altura,largura);
}

~CPigComponente(){
    if (hint)
        free(hint);
}

int GetId(){
    return id;
}

int MouseSobre(int mx, int my){
    if (estado==COMPONENTE_INVISIVEL)
        return 0;

    SDL_Point p={mx,my};
    SDL_Rect r={x,y,larg,alt};

    antesOn = agoraOn;
    agoraOn = SDL_PointInRect(&p,&r);
    if (agoraOn&&!antesOn){
        OnMouseOn();
        return 1;
    }else if(!agoraOn&&antesOn){
        OnMouseOff();
        return -1;
    }
    return 0;
}

PIG_EstadoComponente GetEstado(){
    return estado;
}

virtual int Desenha()=0;

virtual int TrataEvento(PIG_Evento evento)=0;

void DefineHint(char *mensagem){
    if (hint)
        free(hint);
    hint = (char*) malloc(strlen(mensagem));
    strcpy(hint,mensagem);
}

void DefineSom(int idAudio){
    audio = idAudio;
}

void DefineFonteHint(int fonte){
    fonteHint = fonte;
}

void EscreveHint(int mx, int my){
    if (agoraOn&&hint)
        EscreverEsquerda(hint,mx+16,my+5,fonteHint);
}

virtual void DefineEstado(PIG_EstadoComponente estadoComponente)=0;

};


typedef CPigComponente *PigComponente;
