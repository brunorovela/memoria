#include "Global.h"

/*
	Prot�tipo das fun��es usadas nesse arquivo.
*/
void Mostra_Abertura();
void Carrega_Arquivo_Dados();
void Controles_Teclado(CBaralho Baralho[], BITMAP *);
void Controles_Mouse(CBaralho Baralho[], BITMAP *);
void incrementa_segundos();
void ContaPontuacao();

int main()
{
	//Habilita acentua��o
	set_uformat(U_ASCII);

	//Inicia a biblioteca Allegro e seus perif�ricos.
	allegro_init();
	install_keyboard();
	set_color_depth(32);
	install_mouse();
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
	set_window_title( "Mem�ria" );

	Carrega_Arquivo_Dados();
	Mostra_Abertura();

	//Randomiza a cada execu��o do jogo
	srand( time( NULL ) );

	//Permite usar o tempo no Allegro.
	install_timer();
	install_int_ex( incrementa_segundos, BPS_TO_TIMER( 1 ) );
	install_int_ex( ContaPontuacao, BPS_TO_TIMER( 60 ) );

	// Inst�ncia a classe Baralho
	CBaralho Baralho[ iQtdeTiles ];

	Carrega_Tiles( Baralho );
	Embaralhar( Baralho );
	Reinicia_Variaveis();
	RecuperaArquivo();
	OrganizaRecordes();

	/*
		Abaixo segue a t�cnica usada para fazer double buffer que consiste
		em desenhar todos os objetos no buffer antes de mostrar para o jogador
		reduzindo dr�sticamente o pisca pisca de imagens na tela.
	*/
	BITMAP *Buffer = NULL;
	Buffer = create_bitmap(SCREEN_W, SCREEN_H);

	int i = 0;

	while (!Fim_de_Jogo)
	{
		clear_bitmap(Buffer);
		blit((BITMAP *)Dados[iFundos[indFundo]].dat,Buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

		//Imagem e Texto do B�nus
		draw_sprite(Buffer,(BITMAP *)Dados[12].dat,60, 30);  
		textprintf_centre_ex( Buffer, (FONT *)Fontes[5].dat, 112, 38, makecol(0,255,255), -1, "%dx%d", xBonus,iPontuacao );
		
		//Imagem e Texto do N�vel
		draw_sprite(Buffer,(BITMAP *)Dados[13].dat,162, 30);
		textprintf_centre_ex( Buffer, (FONT *)Fontes[5].dat, 187, 38, makecol(0,255,255), -1, "%d", indFundo+1 );

		//Imagem e Texto dos Pontos
		draw_sprite(Buffer,(BITMAP *)Dados[11].dat,60, 62);
		textprintf_centre_ex( Buffer, (FONT *)Fontes[5].dat, 111, 70, makecol(0,255,255), -1, "%.5d", iPontos );

		//Imagem e Texto das Chances
		draw_sprite(Buffer,(BITMAP *)Dados[14].dat,162, 62);
		textprintf_centre_ex( Buffer, (FONT *)Fontes[5].dat, 187, 70, makecol(0,255,255), -1, "%d", iMaxErros-iErros );

		//Imagem e texto do Rel�gio (Horas:Minutos:Segundos)
		draw_sprite(Buffer,(BITMAP *)Dados[10].dat,(SCREEN_W/2)-90, 30);
		textprintf_centre_ex(Buffer, (FONT *)Fontes[4].dat, 339, 34, makecol( 255, 255, 0 ), -1, "%.2d",iHoras);
		textprintf_centre_ex(Buffer, (FONT *)Fontes[4].dat, 399, 34, makecol( 255, 255, 0 ), -1, "%.2d",iMinutos);
		textprintf_centre_ex(Buffer, (FONT *)Fontes[4].dat, 459, 34, makecol( 255, 255, 0 ), -1, "%.2d",iSegundos);

		/*
			Sempre que ouver inicio de nova partida o estado do jogo ser�
			Mostrando. Desse modo o jogador tem o tempo para memorizar os
			desenhos das cartas.
		*/
		if (Status == MOSTRANDO)
		{
			textprintf_centre_ex(Buffer, (FONT *)Fontes[1].dat, SCREEN_W/2, 85, makecol( 255, 255, 0 ), -1, "Memorize as cartas!!!");
			Mostra_Baralho(Baralho, Buffer);
		}

		/*
			Sempre que o estado do jogo for JOGANDO o jogador poder� clicar
			e virar as cartas normalmente, e � nesse estado que � feito o 
			controle dos clicks, verifica��es de cartas iguais e pontua��es.
		*/
		if (Status == JOGANDO)
		{
			/*
				A rotina abaixo se certifica que se a estrutura Cartas_Viradas
				tiver 2 cartas elas devem estar com o verso para cima.
			*/
			if (Cartas_Viradas[0].iEcran > -1 && Cartas_Viradas[1].iEcran > -1)
			{
				Baralho[Cartas_Viradas[0].iEcran].bVirada = false;
				Baralho[Cartas_Viradas[1].iEcran].bVirada = false;
				Inicia_Estrutura();
			}

			textprintf_centre_ex(Buffer, (FONT *)Fontes[1].dat, SCREEN_W/2, 12, makecol( 255, 255, 0 ), -1, "TEMPO");
			
			/*
				Pois seh, essa rotina, assim como outras, com certeza pode 
				ser alterada mas se eu fiz dessa forma � pq tbm funciona.
				Note que eu seto o estado do jogo para RECORDES dentro do estado
				JOGANDO antes de verificar o fim do jogo. Somente ap�s setar o 
				estado que eu verifico se existe baralho ativo. Se existir o
				jogo continua, caso contr�rio, o jogo termina.
			*/
			Status = RECORDES;
			for (i=0; i<iQtdeTiles; i++)
			{
				Baralho[i].Atualizar( Buffer );
				//Se existir baralho ativo (com verso para cima) fa�a
				if (Baralho[i].bAtivo)
				{
					//Continua o jogo
					Status = JOGANDO;
				}
				//Se a quantidade de erros estourou fa�a
				if ( iQtdeNiveis && iErros == iMaxErros)
				{
					// Fim de jogo e seta o estado para RECORDES
					Status = RECORDES;
				}
			}

			/*
				Se ap�s a rotina acima o estado ainda for igual a RECORDES
				� necess�rio fazer mais algumas checagens abaixo.
			*/
			if (Status == RECORDES)
			{
				//Muda o fundo
				indFundo++;

				/*
					Se a imagem de fundo n�o for do ultimo n�vel e o 
					jogador ainda tem chances fa�a...
				*/
				if (indFundo < iQtdeNiveis && iErros < iMaxErros)
				{
					PassarNivel();
					Carrega_Tiles( Baralho );
					Embaralhar( Baralho );
					iEspera = iMaxEspera;
					Status = NOVAFASE;
					//Acrescenta um bonus de +2 chances
					iMaxErros += 2;
				}
				else
				{
					/*
						Se chegou ao final da partida ou do jogo contabiliza
						os restantes da pontua��o em seguida mostra a tabela
						de recordes.
					*/
					if (iPontosGanhos > 0)
					{
						iPontos += iPontosGanhos;
						iPontosGanhos = 0;
					}
					Status = RECORDES;
				}
			}
			/*
				Enquanto o estado do jogo for JOGANDO o mouse vai poder ser
				movimentado e acionado em qualquer momento.
			*/
			Controles_Mouse( Baralho, Buffer );
		}
		
		// A rotina abaixo mostra a mensagem de carta errada
		if (Status == ERRADO)
		{
			textprintf_centre_ex(Buffer, (FONT *)Fontes[1].dat, SCREEN_W/2, 12, makecol( 255, 255, 0 ), -1, "TEMPO");
			textprintf_centre_ex(Buffer, (FONT *)Fontes[2].dat, SCREEN_W/2, 85, makecol( 0, 255, 255 ), -1, "Carta errada!");
			textprintf_centre_ex(Buffer, (FONT *)Fontes[2].dat, SCREEN_W/2, 110, makecol( 0, 255, 255 ), -1, "Iniciando em %d", iEspera);
			
			//Desenha todas as cartas na tela.
			for (i=0; i<iQtdeTiles; i++)
			{
				Baralho[i].Atualizar( Buffer );
			}
			//Note que quando voc� erra a carta n�o tem movimenta��o do mouse
		}

		// Imprime o texto quando troca de fase
		if (Status == NOVAFASE)
		{
			textprintf_centre_ex(Buffer, (FONT *)Fontes[0].dat, SCREEN_W/2, 250, makecol( 0, 255, 200 ), -1, "NOVO N�VEL");
		}

		// Tabela de Recordes
		if (Status == RECORDES)
		{
			blit(Buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
			//Verifica a qualifica��o do jogador
			iLugar = VerficaPontuacao();
			Mostra_Recordes(Buffer, Baralho);
		}else	Controles_Teclado( Baralho, Buffer );
		
		// Estado ENCERRAR
		if (Status == ENCERRAR)
		{
			//Sai do jogo
			Fim_de_Jogo = true;
		}
		/*
			Desenha o ponteiro do mouse na tela. Lembre-se que no arquivo
			dados.dat o indece 4 � referente a imagem do ponteiro do mouse.
		*/
		draw_sprite(Buffer, (BITMAP *)Dados[4].dat, mouse_x, mouse_y);
		//Desenha o Buffer na tela.
		blit(Buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		vsync();
	}

	// Desaloca as imagens e oas arquivos de dados da mem�ria.
	destroy_bitmap( Buffer );
	unload_datafile( Dados );
	unload_datafile( Fontes );
	allegro_exit();

	return 0;
}

/********************************************************************
A fun��o abaixo � respons�vel por carregar a imagem 
de abertura durante 3 segundos.
********************************************************************/
void Mostra_Abertura()
{
	draw_sprite(screen, (BITMAP *)Dados[3].dat, (SCREEN_W/2)-150, (SCREEN_H/2)-150);
	
	// espera 3 segundos
	rest( 3000 );
	clear_keybuf();
}

/********************************************************************
A fun��o abaixo � respons�vel por carregar o arquivo de imagens
e o arquivo de fontes personalizadas. Caso n�o consiga encontrar
um ou outro o programa ser� finalizado imediatamente.

Ornaniza��o do Arquivo dados.dat

Index	Nome		Desci��o
0		fundo		Fundo do nivel 1
1		verso		Verso do baralho
2		tiles		Desenho das cartas do n�vel 1
3		abertura	Imagem da abertura
4		mouse1		Imagem do mouse
5		recorde		Imagem de fundo da tela de recordes
6		fundo2		Fundo do n�vel 2
7		fundo3		Fundo do n�vel 3
8		tiles2		Desenho das cartas di n�vel 2
9		tiles3		Desenho das cartas do n�vel 3
10		relogio		Imagem do Rel�gio
11		pontos		Imagem dos pontos
12		bonus		Imagem do bonus
13		nivel		Imagem do n�vel
14		chances		Imagem das chances

Organiza��o do Arquivo fontes.dat

Index	Nome		Descri��o
0		contador	Fonte do texto "NOVO N�VEL", troca de fase.
1		padrao		Fonte do texto "TEMPO" em cima do rel�gio
2		padrao1		Fonte do tempo de espera a cada erro
3		padrao2		Fonte de teste para mostrar o FPS. (N�o usada)
4		digital		Fonte do rel�gio
5		comics		Fonte do placar (Ponto, Chances, bonus e n�vel)
********************************************************************/
void Carrega_Arquivo_Dados()
{
	Dados = load_datafile("dados.dat");
	Fontes = load_datafile("fontes.dat");

	if (!Dados)
	{
	 	allegro_message("Problema ao carregar arquivo de dados.");
		unload_datafile( Dados );
		allegro_exit();
	}
	if (!Fontes)
	{
		allegro_message("Problema ao carregar arquivo de fontes.");
		unload_datafile( Fontes );
		allegro_exit();
	}
}

/********************************************************************
A fun��o abaixo controla o tempo para diversos tipos de estados
do jogo. Esses estados s�o:

JOGANDO, MOSTRANDO, ERRADO, RECORDES, ENCERRAR, NOVAFASE}

Esses estados foram criados para facilitar a manipula��o do tempo
em alguns momentos. Por exemplo, quando estivermos no estado 
MOSTRANDO os desenhos das cartas estar�o voltadas para cima e o 
tempo estar� decrescente. Assim que o tempo finalizar o status do 
jogo volta para JOGANDO, onde o tempo corre normalmente de forma
crescente. Assim ocorre para os demais estados listados acima.
********************************************************************/
void incrementa_segundos()
{
	if (Status == JOGANDO || Status == ERRADO)
	{
		iSegundos++;
		if (iSegundos == 60)
		{
			iMinutos ++;
			iSegundos = 0;
			if (iMinutos == 60)
			{
				iMinutos = 0;
				iHoras++;
				if (iHoras == 24)
				{
					iHoras = 0;
				}
			}
		}
	}
	
	if (Status == ERRADO)
	{
		iEspera--;
		if (iEspera <= 0)
		{
			Status = JOGANDO;
		}
	}
	
	if (Status == NOVAFASE)
	{
		iEspera--;
		if (iEspera <= 0)
		{
			Status = MOSTRANDO;
			iSegundos++;
		}
	}
	
	if (Status == MOSTRANDO)
	{
		iSegundos--;
		if (iSegundos == 0)
		{
			Status = JOGANDO;
		}
	}
}
/********************************************************************
A fun��o abaixo � respons�vel por controlar as teclas pressionadas.
********************************************************************/
void Controles_Teclado(CBaralho Baralho[], BITMAP *Buffer)
{
	int i = 0;

	//Buffer necess�rio para controlar o tempo de uma tecla e outra
	if (Buffer_Controles == 0)
	{
		// Se apertar a tecla F2 fa�a...
		if (key[KEY_F2])
		{
			/*
				A tecla F2 nesse jogo tem a fun��o de reiniciar o jogo.
				Dessa forma o jogador volta para a primeira faze com suas
				respextivas regras. Como se fosse a primeira vez de jogo.
				
				Muda estado do jogo para mostrando, Carrega as imagens das
				cartas, Embaralha o baralho, Organiza o espa�amento entre as
				cartas, seta o buffer de tempo entre uma tecla e outra e  
				reinica as vari�veis de acordo com as regras da primeira fase.
			*/
			Status = MOSTRANDO;
			Carrega_Tiles( Baralho );
			Embaralhar( Baralho );
			Buffer_Controles = 100;
			Reinicia_Variaveis();

		}else
		//Se apertar a tecla F1 fa�a...
		if (key[KEY_F1])
		{
			/*
				A tecla F1 tem a fun��o de mostrar a tela de recordes para
				o jogador. Esse procedimento obriga o jogador a come�ar um
				novo jogo apertando a tecla F2 ou ESC para sair do jogo.
			*/
			Mostra_Recordes(Buffer, Baralho);
			Buffer_Controles = 30;			
		}else
		//Se apertar a tecla ESC fa�a...
		if (key[KEY_ESC])
		{
			//Informa para o loop o fim do jogo
			Fim_de_Jogo = true;
		}
	} else Buffer_Controles--;
}

/********************************************************************
A fun��o abaixo � respons�vel por controlar os clicks do mouse e
a pontua��o de uma forma geral, Pontos, Bonus, Erros... etc
********************************************************************/
void Controles_Mouse(CBaralho Baralho[], BITMAP *Buffer)
{
	int i = 0;

	//Buffer necess�rio para a velocidade do click n�o ser t�o r�pido.
	if (Buffer_Controles == 0)
	{
		// Se clicou com o bot�o esquerdo do mouse fa�a
		if (mouse_b & 1) 
		{
			// Percorre o baralho
			for (i=0;i<iQtdeTiles;i++) 
			{
				/* 
					Verifica se onde o mouse foi clicaco existe alguma carta.
					Se existe continua...
				*/
				if (Baralho[i].Colisao(mouse_x,mouse_y,1,1)) 
				{
					/*
						Se a carta que o mouse clicou n�o estiver virada 
						continua...
					*/
					if (!Baralho[i].bVirada)
					{
						Baralho[i].bVirada = true; // Vira a Carta
						/*
							Cartas_Viradas � uma estrutura de dois indices que �
							respons�vel por guardar as cartas viradas. � por
							esse array que sabemos se as duas cartas viradas
							s�o iguais.
						*/
						Cartas_Viradas[iClick].iEcran = i;
						Cartas_Viradas[iClick].iId = Baralho[i].Id;
						iClick++; // Controle de click
						
						/*
							Se a quantidade de click que o mouse deu for igual
							a 2. Quer dizer que o jogador j� virou duas cartas.
							Dessa forma, continua...
						*/
						if (iClick == 2)
						{
							// Zera a quantidade de clicks
							iClick = 0; 
							
							/*
								Verifica pelo ID armazenado se a primeira carta
								virada � igual a segunda carta virada. Se for
								continua...
							*/
							if (Cartas_Viradas[0].iId == Cartas_Viradas[1].iId)
							{
								//Faz o c�lculo da pontua��o
								iPontosGanhos += (xBonus*iPontuacao);
								//Acrescenta bonus para a pr�xima pontua��o
								xBonus++;
								
								/*
									J� que o jogador encontrou o par de cartas
									devemos desativar essas cartas do jogo. Dessa
									forma, quando clicar com o mouse em cima das
									cartas n�o aver� rea��o nenhuma.
								*/
								Baralho[Cartas_Viradas[0].iEcran].bAtivo = false;
								Baralho[Cartas_Viradas[1].iEcran].bAtivo = false;
								
								/* 
									Limpa a estrutura Cartas_Viradas para um novo
									come�o de clicks
								*/
								Inicia_Estrutura();
							}
							else
							{
								/*
									Esse trecho acontece se a quantidade de 
									clicks que o jogador deu for igual a 2 
									mas as duas cartas n�o forem iguais.
									Zera o bonus, penaliza o jogador com 
									3 segundos setando o estado como ESPERA e 
									contabiliza erro.
								*/

								xBonus = iMinBonus;
								iEspera = iMaxEspera;
								iErros++;
								Status = ERRADO;
							}
						}
					}
				}
			}
			//Seta o tempo para o pr�ximo click em clicos.
			Buffer_Controles = 30;
		}
	} else Buffer_Controles--;
}

/********************************************************************
A fun��o abaixo � respons�vel por fazer "anima��o da pontua��o".
Toda vez que o jogador receber pontos ele vai para a vari�veil 
iPontosGanhos. Dessa forma, sempre que ouver pontos nessa vari�vel
o jogo vai fazer com que a pontua��o pule de 5 em 5 pontos at� que
a vari�vel iPontosGanhos seja = a 0. � s� para fazer o efeito da 
pontua��o correr a cada frame do jogo.
********************************************************************/
void ContaPontuacao()
{
	if (iPontosGanhos > 0)
	{
		iPontos += 5;
		iPontosGanhos -= 5;
	}
}
END_OF_MAIN();
