#ifndef CBARALHO_H
#define CBARALHO_H

#include <allegro.h>
/*
Classe que guarda as caracteristicas do nosso baralho.
*/
class CBaralho 
{
	private:
		int iPosx;
		int iPosy;
	public:
		BITMAP *imgVerso;
		BITMAP *imgCartao;

		int iLargura;
		int iAltura;
		int Id;
		bool bVirada;
		bool bAtivo;

		CBaralho();
		~CBaralho();

		int Pegax();
		int Pegay();
		void Setaxy(int, int);
		void Atualizar(BITMAP *);
		bool Colisao(int, int, int, int);
};

#endif
