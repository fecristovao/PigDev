class CPigItemCheck: public CPigComponente{

private:

bool marcado;
char texto[200];

virtual int OnMouseOn()=0;
virtual int OnMouseOff()=0;

void Aciona(){
    if (estado!=COMPONENTE_DESABILITADO){
        marcado = !marcado;
        if (marcado){
            estado = COMPONENTE_ACIONADO;
        }else{
            estado = COMPONENTE_NORMAL;
        }
        if (audio>=0) PlayAudio(audio);
    }
}

public:

//construtor provisório
CPigItemCheck(int id, int px, int py, int alt, int larg):CPigComponente(id, px, py, alt, larg){

}

void SetMarcado(bool valor){
    marcado = valor;
}

bool GetMarcado(){
    return marcado;
}

void DefineEstado(PIG_EstadoComponente estadoComponente){
    estado = estadoComponente;

}

int Desenha(){
    SDL_RenderCopyEx(renderer, text, &frame,&dest,-angulo,&pivoRelativo,flip);
    if (strcmp(texto,"")){
        //escreve o texto
    }else{
        //desenha a figura
    }
}

int TrataEvento(PIG_Evento evento){
    if (evento.tipoEvento==EVENTO_MOUSE){

    }else if (evento.tipoEvento==EVENTO_TECLADO){

    }
}

};

typedef CPigItemCheck *PigItemCheck;

class CPigCheckBox:public CPigComponente{

private:

bool marcado[100];
bool habilitado[100];
bool exclusivo;
std::string nome[100];
int qtdItens;

public:



};

typedef CPigCheckBox *PigCheckBox;
