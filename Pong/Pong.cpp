///////////////////////////////////////////////////
///				MUSTAFA BERK BELGE				///
///					1151602083					///
///		BÝLGÝSAYAR GRAFÝKLERÝ DERSÝ PROJESÝ		///
///					PONG OYUNU					///
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///					  TUÞLAR					///
///		Q : Sol Bar Yukarý	A : Sol Bar Aþaðý	///
///		T : Sað Bar Yukarý	G : Sað Bar Aþaðý	///
///				Space : Top At					///
///		U : Kamera Yukarý	J : Kamera Aþaðý	///
///		H : Kamera Sol		K : Kamera Sað		///
///		N : Kamera Yaklaþ	M : Kamera Uzaklaþ	///
///		Z : Iþýk Aç			X : Iþýk Kapat		///
///////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <time.h>

int PencereGenislik = 800;
int PencereYukseklik = 600;
int OrthoGenislik = 800;
int OrthoYukseklik = 600;
int PencereXPozisyonu = 100;
int PencereYPozisyonu = 100;
int TepkiSuresi = 10;
float BarKalinlik = 10;
float OyunAlanBoyutuX = 600;
float OyunAlanBoyutuY = 400;
float KenarCizgiKalinlik = 10;
int SkorA = 0;
int SkorB = 0;
float YaziPozisyonuX = 0;
float YaziPozisyonuY = OyunAlanBoyutuY + 20;
float TopHizX = 3;
float BarHizY = 6;
float KameraX = 0;
float KameraY = 0;
float KameraZ = 600;

float isik_renk[] = { 0.5, 0.2, 0.7 };
float isik_yer[] = { 0,0,600,1 };

float kirmizi[] = { 1, 0, 0 };
float mavi[] = { 0, 0, 1 };

unsigned char header[54]; // BMP dosyalarý 54-byte header içerir
unsigned int dataPos;     // Piksel verilerinin baþlangýç adresi
unsigned int imageWidth, imageHeight, imageSize;
unsigned char *data;
static GLuint texName;

GLuint loadBMP(const char * imagepath) {
	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file) {
		printf("Image could not be opened\n");
		return 0;
	}
	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	imageWidth = *(int*)&(header[0x12]);
	imageHeight = *(int*)&(header[0x16]);
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = imageWidth*imageHeight * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

										 // Create a buffer
	data = new unsigned char[imageSize];
	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);
	//Everything is in memory now, the file can be closed
	fclose(file);
}

class Top
{
public:
	//Topun Koordinatlarý
	float x, y;
	//Topun Yönü
	float vx, vy;
	//Sýnýfa Ait Fonksiyonlar
	void Hareket();
	void TopSekmesi();
	void Cizim();
};

class Bar
{
public:
	float x, y;
	float vy;
	float Uzunluk;
	bool Up, Down, TopTut;
	Bar()
	{
		vy = 0;
		y = 0;
		Up = false;
		Down = false;
		TopTut = false;
	}
	void Cizim();
	void Hareket();
	void TopTutBarHareket();
};

Bar BarSol;
Bar BarSag;
Top Top;

//Bar ve Toplarýn Boyutlarýný Ayarlayan Fonksiyon
void BaslangicAyarlari()
{
	BarSol.Uzunluk = 200;
	BarSag.Uzunluk = 200;
	BarSol.x = -510;
	BarSag.x = 510;
	while (Top.vx == 0)Top.vx = (rand() % 3 - 1)*TopHizX;
	Top.vy = 0;
	Top.x = 0;
	Top.y = 0;
}

//Skor Ayarlama Fonksiyonu
void Kazanma()
{
	if ((SkorA == 10) || SkorB == 10)
	{
		glutTimerFunc(5000, exit, 0);
	}
	if (Top.x < BarSol.x)
	{
		BarSag.TopTut = true;
		SkorB++;
	}
	if (Top.x > BarSag.x)
	{
		BarSol.TopTut = true;
		SkorA++;
	}
}

//Topun Bar ile Hareket Etmesi için Fonksiyon
void Bar::TopTutBarHareket()
{
	if (TopTut)
	{
		Top.vx = 0;
		if (x < 0) Top.x = x + 2 * BarKalinlik;
		if (x > 0) Top.x = x - 2 * BarKalinlik;
		Top.vy = vy;
		Top.y = y;
	}
}

//Bar Hareketi Ýçin Fonksiyon
void Bar::Hareket()
{
	y += vy;
	if (y < -OyunAlanBoyutuY + Uzunluk / 2)
	{
		y = -OyunAlanBoyutuY + Uzunluk / 2;
		vy = 0;
	}
	if (y > OyunAlanBoyutuY - Uzunluk / 2)
	{
		y = OyunAlanBoyutuY - Uzunluk / 2;
		vy = 0;
	}
}

//Bar Çizimi Ýçin Fonksiyon
void Bar::Cizim()
{
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glColor3f(0, 0, 0);
	glScalef(1, 20, 1);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mavi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kirmizi);
	glBindTexture(GL_TEXTURE_2D, texName);
	glutSolidCube(10);
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

//Topun Duvarlar ve Barlardan Sekmesi Ýçin Fonksiyon
void Top::TopSekmesi()
{
	//Top Duvara Çarpýyorsa Geldiði Açýda Seker
	if ((y <= -OyunAlanBoyutuY) || (y >= OyunAlanBoyutuY))
	{
		vy = -vy;
	}
	//Top Bara Çarpýyorsa Geldiði Açýya Barýn Hýzý Eklenir ve Seker
	if ((x <= BarSol.x + BarKalinlik) && ((y<BarSol.y + BarSol.Uzunluk / 2) && (y>BarSol.y - BarSol.Uzunluk / 2)))
	{
		vx = -vx;
		vy += BarSol.vy;
	}
	//Top Bara Çarpýyorsa Geldiði Açýya Barýn Hýzý Eklenir ve Seker
	if ((x >= BarSag.x - BarKalinlik) && ((y<BarSag.y + BarSag.Uzunluk / 2) && (y>BarSag.y - BarSag.Uzunluk / 2)))
	{
		vx = -vx;
		vy += BarSag.vy;
	}
}

//Topun Çizimi Ýçin Fonksiyon
void Top::Cizim()
{
	glColor3f(0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(1, 0, 0);
	glTranslatef(x, y, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, kirmizi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mavi);
	glutSolidSphere(10, 50, 50);
	glPopMatrix();
}

//Topun Hareketi Ýçin Fonksiyon
void Top::Hareket()
{
	x += vx;
	y += vy;
}

//Oyun Alaný Çizimi
void AlanCizimi()
{
	//Üst Sýnýr
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslatef(0, OyunAlanBoyutuY, 0);
	glColor3f(0, 0, 0);
	glScalef(120, 1, 1);
	glutSolidCube(10);
	glPopMatrix();

	//Alt Sýnýr
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslatef(0, -OyunAlanBoyutuY, 0);
	glColor3f(0, 0, 0);
	glScalef(120, 1, 1);
	glutSolidCube(10);
	glPopMatrix();

	//Sað Sýnýr
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslatef(OyunAlanBoyutuX, 0, 0);
	glColor3f(0, 0, 0);
	glScalef(1, 80, 1);
	glutSolidCube(10);
	glPopMatrix();

	//Sol Sýnýr
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glTranslatef(-OyunAlanBoyutuX, 0, 0);
	glColor3f(0, 0, 0);
	glScalef(1, 80, 1);
	glutSolidCube(10);
	glPopMatrix();
}

//Oyun Skorunun Çizimi için Fonksiyon
void SkorCizimi()
{
	glColor3f(0, 0, 0);
	glRasterPos2f(YaziPozisyonuX - 50, YaziPozisyonuY + 20);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + SkorA);
	glRasterPos2f(YaziPozisyonuX + 30, YaziPozisyonuY + 20);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '0' + SkorB);
	glRasterPos2f(YaziPozisyonuX - 50, YaziPozisyonuY + 60);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
	glRasterPos2f(YaziPozisyonuX + 30, YaziPozisyonuY + 60);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'B');
	if (SkorA == 10)
	{
		glRasterPos2f(YaziPozisyonuX - 400, YaziPozisyonuY + 50);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'K');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Z');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'N');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'N');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
	}
	if (SkorB == 10)
	{
		glRasterPos2f(YaziPozisyonuX + 200, YaziPozisyonuY + 50);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'K');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Z');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'N');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'N');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'B');
	}
}

//Tuþlar
void BarKontrol()
{
	if ((BarSol.Up) && (!BarSol.Down))BarSol.vy = BarHizY;
	if ((!BarSol.Up) && (BarSol.Down))BarSol.vy = -BarHizY;
	if ((BarSag.Up) && (!BarSag.Down))BarSag.vy = BarHizY;
	if ((!BarSag.Up) && (BarSag.Down))BarSag.vy = -BarHizY;
}

//Klavye Fonksiyonu
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		BarSol.Up = true;
		break;
	case 'a':
		BarSol.Down = true;
		break;
	case 32:
		if (BarSol.TopTut)
		{
			BarSol.TopTut = false;
			Top.vx = TopHizX;
		}
		if (BarSag.TopTut)
		{
			BarSag.TopTut = false;
			Top.vx = -TopHizX;
			break;
		}
		break;
	case 't':
		BarSag.Up = true;
		break;
	case 'g':
		BarSag.Down = true;
		break;
	case 'k':
		gluLookAt(KameraX + 1, KameraY, KameraZ - 600, 0, 0, -20, 0, 1, 0);
		break;
	case 'h':
		gluLookAt(KameraX - 1, KameraY, KameraZ - 600, 0, 0, -20, 0, 1, 0);
		break;
	case 'u':
		gluLookAt(KameraX, KameraY + 1, KameraZ - 600, 0, 0, -20, 0, 1, 0);
		break;
	case 'j':
		gluLookAt(KameraX, KameraY - 1, KameraZ - 600, 0, 0, -20, 0, 1, 0);
		break;
	case 'n':
		gluLookAt(KameraX, KameraY, KameraZ - 610, 0, 0, -20, 0, 1, 0);
		break;
	case 'm':
		gluLookAt(KameraX, KameraY, KameraZ - 590, 0, 0, -20, 0, 1, 0);
		break;
	case 'z':
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		break;
	case 'x':
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		break;
	}
}

//Klavyede Tuþu Býrakýnca Çalýþan Fonksiyon
void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		BarSol.Up = false;
		break;
	case 'a':
		BarSol.Down = false;
		break;
	case 't':
		BarSag.Up = false;
		break;
	case 'g':
		BarSag.Down = false;
		break;
	}
}

//Süre ile Çalýþan Fonksiyonlar
void Timer(int value)
{
	Kazanma();
	BarKontrol();
	BarSol.Hareket();
	BarSag.Hareket();
	Top.Hareket();
	Top.TopSekmesi();
	BarSol.TopTutBarHareket();
	BarSag.TopTutBarHareket();
	BarSol.vy = 0;
	BarSag.vy = 0;
	glutPostRedisplay();
	glutTimerFunc(TepkiSuresi, Timer, 0);
}

//Obje Cizimleri
void Cizim()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BarSag.Cizim();
	BarSol.Cizim();
	AlanCizimi();
	Top.Cizim();
	SkorCizimi();
	glutSwapBuffers();
}

void init(void)
{
	glClearColor(0.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1.5, 600, 0);

	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_POSITION, isik_yer);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, isik_renk);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	GLuint image = loadBMP("brick.bmp");
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);


	gluLookAt(KameraX, KameraY, KameraZ, 0, 0, -20, 0, 1, 0);
}

int main(int argc, char ** argv)
{
	srand(time(NULL));
	BaslangicAyarlari();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(PencereGenislik, PencereYukseklik);
	glutInitWindowPosition(PencereXPozisyonu, PencereYPozisyonu);
	glutCreateWindow("PONG");
	init();
	glutDisplayFunc(Cizim);
	glutTimerFunc(TepkiSuresi, Timer, 0);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMainLoop();
	return 0;
}