#include "CBaralho.h"
/*
O construtor � respons�vel por iniciar as vari�veis logo que a 
classe � inst�nciada.
*/
CBaralho::CBaralho()
{
	this->iLargura = 80;
	this->iAltura = 100;
	this->imgCartao = NULL;
	this->imgCartao = create_bitmap(iLargura,iAltura);
	this->imgVerso = NULL;
	this->imgVerso = create_bitmap(iLargura,iAltura);
	this->Id = 0;
	this->iPosx = 0;
	this->iPosy = 0;
	this->bVirada = false;
	this->bAtivo = true;
}
/*
O destru�dor � respons�vel por desalocar as imagens
utilizadas pela classe.
*/
CBaralho::~CBaralho()
{
	this->imgCartao = NULL;
	delete this->imgCartao;

	this->imgVerso = NULL;
	delete this->imgVerso;
}

int CBaralho::Pegax()
{
	//Retorna a posi��o X do baralho
	return this->iPosx;
}

int CBaralho::Pegay()
{
	//Retorna a posi��o Y do baralho
	return this->iPosy;
}

void CBaralho::Setaxy( int vx, int vy )
{
	/*
	Seta a posi��o X eY do paralho com base no par�mentro 
	das vari�veis VX e VY.
	*/
	this->iPosx = vx;
	this->iPosy = vy;
}

void CBaralho::Atualizar(BITMAP *Buffer)
{
	/*
	Se a carta j� foi virada mostra a carta com o desenho virado para cima. 
	Sen�o, mostra a carta com o verso para cima.
	*/
	if (bVirada)
	{
		draw_sprite(Buffer,this->imgCartao,this->iPosx, this->iPosy);
	}
	else
	{
		draw_sprite(Buffer,this->imgVerso,this->iPosx, this->iPosy);
	}
}
bool CBaralho::Colisao(int obj2x,int obj2y, int obj2a, int obj2l)
{
	/*
		Verifica a colis�o entre o baralho e outro objetvo. Nesse caso, iremos
		informar como o "outro objeto" o mouse. Dessa forma vamos saber se o 
		mouse teve contato com o baralho ou n�o.
	*/
	if (this->iPosx+this->iLargura > obj2x &&
		this->iPosx < obj2x+obj2l &&
		this->iPosy+this->iAltura > obj2y &&
		this->iPosy < obj2y+obj2a)
		{
			return true;
		} 
		else
		return false;
}
