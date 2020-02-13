class CPigForm{

private:

int id;
int x,y,alt,larg;


public:

CPigForm(int idForm, int xForm, int yForm, int altForm, int largForm){
    id = idForm;
    x = xForm;
    y = yForm;
    alt = altForm;
    larg = largForm;
}

~CPigForm(){
}

int TrataEvento(PIG_Evento evento){

}

int Desenha(){

}

};


typedef CPigForm *PigForm;
