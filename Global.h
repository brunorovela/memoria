#ifndef GLOBAL_H
#define GLOBAL_H

#include <allegro.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include "CBaralho.h"

using namespace std;

/*
	Prot�tipo das fun��es usadas nesse arquivo.
*/
void Carrega_Tiles(CBaralho []);
void Mostra_Baralho(CBaralho [], BITMAP *);
void Embaralhar(CBaralho []);
void Inicia_Estrutura();
void Inicia_Jogador();
void RecuperaArquivo();
void GravaArquivo();
void Mostra_Recordes(BITMAP *, CBaralho []);
void Reinicia_Variaveis();
void Recorde_Padrao();
void PassarNivel();
void OrganizaRecordes();
int VerficaPontuacao();

static DATAFILE *Dados = NULL;
static DATAFILE *Fontes = NULL;

// Constantes que centralizam o conjunto de baralho na tela
const int Margem_Topo = 150;	// Empurra o conjunto de baralho para baixo
const int Margem_Esquerda = 60;	// Empurra o conjunto de baralho para direita

// S�o contantes bases, utilizadas para v�rias verfica��es.
const int iLargura_Tiles  = 80;	// Largura da carta
const int iAltura_Tiles = 100;	// Altura da carta
const int ixTiles  = 8;	// Quantidade de cartas por linha
const int iyTiles = 4;	// Quantidade de cartas por coluna
const int iQtdeTiles = (ixTiles * iyTiles);	// Total de cartas do jogo
const int iQtdeNiveis = 3;		// Quantidade de n�vel do jogo
const int iMaxEspera = 3;		// Tempo m�ximo de espera a cada erro
const int iQtdeRecordes = 10;	// Quantidade de posi��es no recorde

/*
As duas constantes abaixo s�o importantes.
iFundos = Guarda os indices de das imagens de fundo que iremos utilizar nos
3 n�veis.
iDesenhos = Guarda os indices das imagens das cartas que iremos utilizar nos
3 n�veis.

Volte para a tabela referente o arquivo dados.dar e cruze os valores abaixo
com os denhos guardados no arquivo de dados. � dessa forma que eu altero a fase
durante o jogo. Para n�o precisa fazer um arquivo s� de fases eu utilizo um 
indice. Dessa forma as imagens podem ficar espalhadas dentro do arquivo de 
dados.
Em um projeto maior o ideal � separar e fazer de maneira mais organizada.
*/

const int iFundos[iQtdeNiveis] = {0,6,7};
const int iDesenhos[iQtdeNiveis] = {2,8,9};

/*
Defini��o de todos os estados do Jogo.

JOGANDO   - Jogador jogando normalmente.
MOSTRANDO - Jogador apenas vizualizando as cartas para memoriza��o.
ERRADO    - Jogador errou a carta.
RECORDES  - Jogador visualiza a tabela de recordes.
ENCERRAR  - Jogador encerra o jogo.
NOVAFASE  - Jogador passa para o pr�ximo n�vel.
*/
enum stJogo {JOGANDO, MOSTRANDO, ERRADO, RECORDES, ENCERRAR, NOVAFASE};

//Vari�vel respons�vel por guardar o status atual do Jogo
stJogo Status = MOSTRANDO;

int iMaxErros = 15;	// Seta a quantidade m�xima de chances do usu�rio
int iMinBonus = 0;	// seta o bonus m�nimo
int iMaxSec = 15;	// Seta o m�ximo de segundos para mostrar as cartas
int iPontos = 0;	// Seta os pontos iniciais
int iEspera = 0;	// Seta a espera
int iSegundos = iMaxSec; // Seta o segundos para o m�ximo permitido
int iMinutos = 0;	// Seta o minuto
int iHoras = 0;		// Seta a hora
int Buffer_Controles = 0;	// Controle do tempo das teclas.
int xBonus = iMinBonus;		// Seta o bonus com o m�nimo permitido
int iClick = 0; 	// Seta o controle de Clicks
int iErros = 0;		// Se a quantidade de erros
int indFundo = 0;	// Seta o fundo inicial conforme tabela dados.dat
int indDesenho = 0;	// Seta o desenho inicial da carta conforme tabela dados.dat
bool Fim_de_Jogo = false; 	// Seta o fim do jogo
int iLugar = -1;			// Seta a qualifica��o
int iPontuacao = 100;		// Seta a pontua��o base
int iPontosGanhos = 0;		// Seta o buffer de pontos para anima��o dos pontos

/*
Essa estrutura foi criada para guardar os IDs das cartas que o jogador
vira e desvira durante o jogo. � com essa estrutura que iremos fazer a 
verifica��o das cartas iguais.
*/
struct strCartas
{
	int iEcran;
	int iId;
} Cartas_Viradas[2];

/*
Essa estrutura guarda as informa��es do jogador para poder gravar na tabela
de recordes.
*/
struct strJogador
{
	int iHoras;
	char sNome[9];
	int iMinutos;
	int iSegundos;
	int iErros;
	int iPontos;
	int iNivel;
} Jogador;

/*
Essa estrutura � a pr�pria tabela de recordes com 9 posi��es. Assim que for
confirmado que o jogador conseguiu entrar para a tabela de recordes as 
informa��es da estrutura strJogador passa para a estrutura strRecordes.
*/
struct strRecordes
{
	int iId;
	char sNome[9];
	int iHoras;
	int iMinutos;
	int iSegundos;
	int iErros;
	int iPontos;
	int iNivel;
} Recordes[iQtdeRecordes];

/*
A estrutura abaixo foi criada para auxiliar na organiza��o da tabela
de recordes. Como o metodo de organiza��o � o BOLHA ent�o eu criei essa
estrutura para ser o meu TEMP enquanto fa�o a troca de posi��o entre os
recordes.
*/
struct strRecTemp
{
	char sNome[9];
	int iHoras;
	int iMinutos;
	int iSegundos;
	int iErros;
	int iPontos;
	int iNivel;
} RecTemp;

/*
A fun��o abaixo recebe como argumento o conjunto de baralho. Como dentro do arquivo
dados.dat n�s temos apenas 3 arquivos de desenhos e nesses 3 arquivos temos em 
cada um 32 imagens. Para que possamos colocar cada desenho em um baralho � 
necess�rio cortar o desenho para cada baralho. Para isso, precisamos da altura e
largura de cada tile.Para cada troca de fase essa fun��o � excutada para colocar
o desenho novo no baralho.
Junto com o corte e desenho do baralho eu aproveito para setar alguma 
propriedades do baralho e a posi��o deles na tela.
*/
void Carrega_Tiles(CBaralho Baralho[])
{
	int x = 0;
	int y = 0;
	int iControle = 0;
	int iId = 0;
	int iVertical = 0;
	int iHorizontal = 0;
	
	for (y = 0; y < iyTiles; y++)
	{
		for (x = 0; x < ixTiles; x++)
		{
			Baralho[iControle].Id = iId;

			blit((BITMAP *)Dados[iDesenhos[indDesenho]].dat, Baralho[iControle].imgCartao, x*iLargura_Tiles, y*iAltura_Tiles, 0, 0, iLargura_Tiles, iAltura_Tiles);

			Baralho[iControle].Setaxy((x*iLargura_Tiles)+Margem_Esquerda+iVertical, (y*iAltura_Tiles)+Margem_Topo+iHorizontal);
			draw_sprite(Baralho[iControle].imgVerso,(BITMAP *)Dados[1].dat,0,0);
		//	textprintf_centre_ex(Baralho[iControle].imgVerso, (FONT *)Fontes[1].dat, 80/2, 75, makecol( 255, 255, 0 ), -1, "%d",Baralho[iControle].Id);

			Baralho[iControle].bVirada = false;
			Baralho[iControle].bAtivo = true;

			/*
			Nessa parte especificamos um ID para cada baralho. Todos os desenhos
			que forem iguais ter�o IDs Iguais. No total teremos a metade 16 IDs
			dispon�veis.
			*/
			iControle++;
			if (iControle % 2 == 0)
			{
				iId++;
				Baralho[iControle].Id = iId;
			}
			iVertical += 5;
		}
		iVertical = 0;
		iHorizontal += 5;
	}
}

/*
Com a execu��o da fun��o Carrega_Tiles os baralhos estar�o com os devidos desenhos
conforme a fase atual, na posi��o correta e com suas propriedades definidas.
A fun��o abaixo � respons�vel por mostrar esse conjunto de baralho na tela afim
de fazer o jogador memorizar todas.
*/
void Mostra_Baralho(CBaralho Baralho[], BITMAP *Buffer)
{
	int x = 0;
	int y = 0;
	int iControle = 0;

	for (y = 0; y < iyTiles; y++)
	{
		for (x = 0; x < ixTiles; x++)
		{
			Baralho[iControle].bVirada = false; // Carta com o desenho para cima
			Baralho[iControle].bAtivo = true;	// Carta que continua no jogo
			draw_sprite(Buffer,Baralho[iControle].imgCartao,Baralho[iControle].Pegax(), Baralho[iControle].Pegay());
			iControle++;
		}
	}
}
/*
A fun��o abaixo embaralha cada uma das cartas individualmente. Funciona da 
seguinte forma: 
-Randomiza entre 0 e 31 uma posi��o antiga (old_pos) e uma 
posi��o nova (new_pos)
-Se as duas posi��es geradas forem iguais ele randomiza at� que elas sejam
diferentes.
-Pega a carta que est� na posi��o Old e troca com a carta que est� na posi��o 
new.
-A fun��o faz isso com cada uma das cartas.
No final "todas" estar�o trocadas.
*/
void Embaralhar(CBaralho Baralho[])
{
	int iX = 0; // Backup de X
	int iY = 0; // Backup de Y
	int i = 0;
	int old_pos = 0;
	int new_pos = 0;

	for (i=0; i<iQtdeTiles; i++)
	{
		old_pos = rand() % iQtdeTiles;
		new_pos = rand() % iQtdeTiles;
		while (old_pos == new_pos)
		{
			old_pos = rand() % iQtdeTiles;
			new_pos = rand() % iQtdeTiles;
		}
		iX = Baralho[old_pos].Pegax();
		iY = Baralho[old_pos].Pegay();
		Baralho[old_pos].Setaxy(Baralho[new_pos].Pegax(), Baralho[new_pos].Pegay());
		Baralho[new_pos].Setaxy(iX,iY);
	}
	
}

//fun��o respons�vel por iniciar a estrutura que controla as cartas viradas.
void Inicia_Estrutura()
{
	Cartas_Viradas[0].iEcran = -1;
	Cartas_Viradas[0].iId = -1;
	Cartas_Viradas[1].iEcran = -1;
	Cartas_Viradas[1].iId = -1;
}

//Fun��o que inicia as informa��es do jogador
void Inicia_Jogador()
{
	strcpy(Jogador.sNome,"Jogador ");
	Jogador.iHoras = 0;
	Jogador.iMinutos = 0;
	Jogador.iSegundos = 0;
	Jogador.iErros = 0;
	Jogador.iPontos = 0;
	Jogador.iNivel = indFundo;
}
//Fun��o respons�vel por gravar a estrutura recordes em arquivo de dados.
void GravaArquivo()
{
	ofstream AbreArquivo("Jogo.dat", ios::out | ios::binary);	 

	AbreArquivo.write((char *) &Recordes, sizeof Recordes); 

	AbreArquivo.close(); 
}

//Func��o que recupera as informa��es de recordes gravadas no arquivo de dados
void RecuperaArquivo()
{
	ifstream LendoArquivo("Jogo.dat", ios::in | ios::binary);
	if (LendoArquivo)
	{
		LendoArquivo.read((char *) &Recordes, sizeof Recordes); 
	
		LendoArquivo.close();
	}
	else
	{
		Recorde_Padrao();
		GravaArquivo();
	}
}

//Fun��o respons�vel por mostrar e organizar a tela de recordes.
void Mostra_Recordes(BITMAP *Buffer, CBaralho Baralho[])
{
	BITMAP *imgRecorde = NULL;
	imgRecorde = create_bitmap(400,300);

	int i = 0;
	Status = ENCERRAR;

	while (!key[KEY_ESC])
	{
		blit((BITMAP *)Dados[5].dat,imgRecorde,0,0,0,0,400,300);

		textprintf_ex(imgRecorde, font, 7, 50, makecol( 255, 255, 0 ), -1, "N�  Nome       Tempo      Erros   Pontos   N�vel");
		
		//Percorre a tabela de recordes para imprimir na tela.
		for (i=0; i<(iQtdeRecordes-1); i++)
		{
			if (i == iLugar) 
			{
				textprintf_ex(imgRecorde, font, 7, 75+(i*15), makecol( 0, 255, 255 ), -1, "%d   %s   %.2d:%.2d:%.2d   %.2d      %.5d    %d", Recordes[i].iId, Recordes[i].sNome, Recordes[i].iHoras, Recordes[i].iMinutos, Recordes[i].iSegundos, Recordes[i].iErros, Recordes[i].iPontos,Recordes[i].iNivel);
			}
			else
			{
				textprintf_ex(imgRecorde, font, 7, 75+(i*15), makecol( 255, 255, 0 ), -1, "%d   %s   %.2d:%.2d:%.2d   %.2d      %.5d    %d", Recordes[i].iId, Recordes[i].sNome, Recordes[i].iHoras, Recordes[i].iMinutos, Recordes[i].iSegundos, Recordes[i].iErros, Recordes[i].iPontos, Recordes[i].iNivel);
			}
		}
		/*
		Se o iLugar for maior que -1 quer dizer que o jogador entrou para a tabela de 
		recordes. Dessa forma, escrevemos as informa��es do recorde do jogador em azul 
		claro.
		*/
		if (iLugar >-1)
		{
			textprintf_centre_ex(imgRecorde, (FONT *)Fontes[2].dat, 200, 215, makecol( 255, 255, 0 ), -1, "Parab�ns!");
		}
		else
		{
			textprintf_centre_ex(imgRecorde, (FONT *)Fontes[2].dat, 200, 215, makecol( 255, 255, 0 ), -1, "Sem novo Recorde.");
		}
		//Desenha a imagem de fundo do recorde na tela.
		draw_sprite(screen, imgRecorde, (SCREEN_W/2)-200, (SCREEN_H/2)-150);
		
		/*
		Se na tela de recorde o jogador apertar F2, come�a um jogo novo.
		Caso contr�rio � fim de jogo
		*/
		if (key[KEY_F2])
		{
			Status = MOSTRANDO;
			Reinicia_Variaveis();
			Carrega_Tiles( Baralho );
			Embaralhar( Baralho );
			Buffer_Controles = 100;
			break;
		}
	}

	//Desaloca imagem de recode na mem�ria.
	destroy_bitmap( imgRecorde );
}


//Reinicia todas as vari�veis do jogo para um novo come�o.
void Reinicia_Variaveis()
{
	Inicia_Estrutura();
	Inicia_Jogador();
	iPontos = 0;
	iEspera = 0;
	iMinutos = 0;
	iHoras = 0;
	Buffer_Controles = 0;
	iMinBonus = 1;
	xBonus = iMinBonus;
	iClick = 0; // Controle de Clicks
	iErros = 0;
	iMaxSec = 15;
	iSegundos = iMaxSec;
	indFundo = 0;
	indDesenho = 0;
	Fim_de_Jogo = false;
	iLugar = -1;
	iPontuacao = 100;
	iPontosGanhos = 0;
	iMaxErros = 15;
}

/*
A fun��o abaixo � respons�vel por percorrer a tabela de recordes e verificar
se a pontua��o do jogador merece entrar para a tabela de recordes. Se merecer
retorna o lugar em que o jogar ficou.
*/
int VerficaPontuacao()
{
	int i = 0;
	int iLugar = -1;

	// Atribui alguns informa��es do jogo para o jogador.
	Jogador.iHoras = iHoras;
	Jogador.iMinutos = iMinutos;
	Jogador.iSegundos = iSegundos;
	Jogador.iErros = iErros;
	Jogador.iPontos = iPontos;
	Jogador.iNivel = indFundo+1;

	//Percorre a tabela de recordes
	for (i=0; i<(iQtdeRecordes-1); i++)
	{
		// Se o nivel for maior ou igual ao da tabela continua...
		if (Jogador.iNivel >= Recordes[i].iNivel)
		{
			// Se os pontos do jogador for maior que o da tabela continua...
			if (Jogador.iPontos >= Recordes[i].iPontos)
			{
				//Atribui as informa��es do jogador na posi��o dp recorde quebrado
				strcpy(Recordes[(iQtdeRecordes-1)].sNome,Jogador.sNome);
				Recordes[(iQtdeRecordes-1)].iHoras = Jogador.iHoras;
				Recordes[(iQtdeRecordes-1)].iMinutos = Jogador.iMinutos;
				Recordes[(iQtdeRecordes-1)].iSegundos = Jogador.iSegundos;
				Recordes[(iQtdeRecordes-1)].iErros = Jogador.iErros;
				Recordes[(iQtdeRecordes-1)].iPontos = Jogador.iPontos;
				Recordes[(iQtdeRecordes-1)].iNivel = Jogador.iNivel-1;
				//Informa a qualidfica��o com +1 porque a contagem come�a do 0
				iLugar = i;
				break;
			}
		}
	}
	OrganizaRecordes();
	GravaArquivo();
	return iLugar;
}

/*
Se o arquivo de recordes n�o existir essa fun��o vai criar o arquivo
recordes com valores padr�es.
*/
void Recorde_Padrao()
{
	int i = 0;
	
	for (i = 0; i<iQtdeRecordes; i++)
	{
		strcpy(Recordes[i].sNome,"CPU     ");
		Recordes[i].iId = i+1;
		Recordes[i].iHoras = 23;
		Recordes[i].iMinutos = 59;
		Recordes[i].iSegundos = 59;
		Recordes[i].iErros = 0;
		Recordes[i].iPontos = 0;
		Recordes[i].iNivel = 0;
	}
}

/*
Assim que o jogador passar de n�vel a fun��o abaixo vai ajustar as vari�veis 
do jogo para  o pr�ximo n�vel.
*/
void PassarNivel()
{
	iMinBonus++;	// Sobe o bonus
	iMaxSec -= 5;	// Diminui o tempo de memoriza��o
	xBonus = iMinBonus;	// A base do bonus recebe o novo limite minimo
	iPontuacao += 50;	// A base de calculo da pontua��o recebe +50 pontos
	indDesenho++;		// Troca o desenho das cartas conforme o arquivo de dados
	iHoras = 0;			// Seta a hora
	iMinutos = 0;		// Seta os minutos
	iSegundos = iMaxSec;	// Seta o rel�gio para o tempo de memoriza��o novo
	iPontos += iPontosGanhos;	// Soma os pontos anteriores com o que vai ganhar agora
	iPontosGanhos = 0;	// Seta os pontos no novo n�vel
	Status = MOSTRANDO;	// Seta o estado da nova fase.
}

/*
A fun��o abaixo � respons�vel por organizar a tabela de recordes com o m�todo 
bolha na forma decrescente. O maior pontua��o em cima e o menor embaixo.
*/
void OrganizaRecordes()
{
	int iAtual = 0;
	int iProximo = 0;
	int iAnterior = 0;
	int i = 0;
	bool bTroca = true;

	while (bTroca)
	{
		bTroca = false;
		
		for (i = 0; i<(iQtdeRecordes-1); i++)
		{
			if (Recordes[i].iNivel <= Recordes[i+1].iNivel)
			{
				if (Recordes[i].iPontos < Recordes[i+1].iPontos)
				{
					bTroca = true;

					strcpy(RecTemp.sNome,Recordes[i].sNome);
					RecTemp.iHoras = Recordes[i].iHoras;
					RecTemp.iMinutos = Recordes[i].iMinutos;
					RecTemp.iSegundos = Recordes[i].iSegundos;
					RecTemp.iErros =	Recordes[i].iErros;
					RecTemp.iPontos = Recordes[i].iPontos;
					RecTemp.iNivel = Recordes[i].iNivel;

					strcpy(Recordes[i].sNome,Recordes[i+1].sNome);
					Recordes[i].iHoras = Recordes[i+1].iHoras;
					Recordes[i].iMinutos = Recordes[i+1].iMinutos;
					Recordes[i].iSegundos = Recordes[i+1].iSegundos;
					Recordes[i].iErros =	Recordes[i+1].iErros;
					Recordes[i].iPontos = Recordes[i+1].iPontos;
					Recordes[i].iNivel = Recordes[i+1].iNivel;

					strcpy(Recordes[i+1].sNome,RecTemp.sNome);
					Recordes[i+1].iHoras = RecTemp.iHoras;
					Recordes[i+1].iMinutos = RecTemp.iMinutos;
					Recordes[i+1].iSegundos = RecTemp.iSegundos;
					Recordes[i+1].iErros =	RecTemp.iErros;
					Recordes[i+1].iPontos = RecTemp.iPontos;
					Recordes[i+1].iNivel = RecTemp.iNivel;
				}
			}
		}
	}
}
#endif
