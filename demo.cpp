// Code made by Vishnu Mani Hema : Andrew ID : vmanihem
#include "yspng.h"
#include "yspngenc.h"
#include "ysglfontdata.h"
#include "fssimplewindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>
#include <string> 
#include <vector>
#include <string>
#include "yssimplesound.h"
#define PI 3.14159265
// The pixel to len ratio
double pixel_len = 10;
int m4 = 0;
// ****************** For music ************************
char* MyFgets(char str[], int maxn, FILE* fp)
{
	auto r = fgets(str, maxn, fp);
	if (nullptr != r)
	{
		for (int i = strlen(str) - 1; 0 <= i; --i)
		{
			if (str[i] < ' ')
			{
				str[i] = 0;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		str[0] = 0;
	}
	return r;
}


// *************************************************


void DrawMissile(double cx, double cy)
{
	glColor3ub(206, 40, 40);

	glBegin(GL_LINES);
	glVertex2d(cx, cy - 15);
	glVertex2d(cx + 15, cy - 15);

	glVertex2d(cx, cy + 15);
	glVertex2d(cx + 15, cy + 15);
	glEnd();
}
void Drawlaser(double cx, double cy)
{
	glColor3ub(206, 40, 40);

	glBegin(GL_POLYGON);
	glVertex2d(cx - 50, cy - 15);
	glVertex2d(0, cy - 15);
	glVertex2d(0, cy - 25);
	glVertex2d(cx - 50, cy - 25);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2d(cx - 50, cy + 15);
	glVertex2d(0, cy + 15);
	glVertex2d(0, cy + 25);
	glVertex2d(cx - 50, cy + 25);
	glEnd();


}

void DrawLifebar(int life, int n)
{
	if (n == 1)
	{

		for (int i = 0; i < life; i++)
		{
			double k = i * 2 * pixel_len;
			glColor3ub(0, 255, 0);

			glBegin(GL_QUADS);
			glVertex2d(k, 0);
			glVertex2d(k + 10, 0);
			glVertex2d(k + 10, 10);
			glVertex2d(k, 10);
			glEnd();
		}
	}
	if (n == 2)
	{

		for (int i = 0; i < life; i++)
		{
			double k = i * 2 * pixel_len;
			glColor3ub(255, 0, 0);

			glBegin(GL_QUADS);
			glVertex2d(800 - k, 0);
			glVertex2d(800 - (k + 10), 0);
			glVertex2d(800 - (k + 10), 10);
			glVertex2d(800 - k, 10);
			glEnd();
		}
	}
}



void DrawExplosion(double cx, double cy, double rad, double fill)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(50));

	glColor3ub(206, 32, 41);
	const double YS_PI = 3.1415927;
	if (0 != fill)
	{
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	double i;
	for (i = 0; i < 64; i++)
	{
		double angle = (double)i * YS_PI / 32.0;
		double x = (double)cx + cos(angle) * (double)rad;
		double y = (double)cy + sin(angle) * (double)rad;
		glVertex2d(x, y);
	}

	glEnd();
	FsSwapBuffers();
}


// ******************************** Rendering space ship begining ******************************************
std::vector <std::string> Remcontrolcode(std::string input)
{
	std::vector <std::string> args;

	int state = 0;
	for (auto c : input)
	{
		if (0 == state)
		{
			if (' ' != c && '\t' != c && '\n' != c && '.' != c)
			{
				args.push_back("");
				args.back().push_back(c);
				state = 1;
			}
		}
		else
		{
			if (' ' != c && '\t' != c && '\n' != c && '.' != c)
			{
				args.back().push_back(c);
			}
			else
			{
				state = 0;
			}
		}
	}

	return args;
}

std::vector <std::string> Parse(std::string input)
{
	std::vector <std::string> args;

	int state = 0;
	for (auto c : input)
	{
		if (0 == state)
		{
			if (' ' != c && '\t' != c)
			{
				args.push_back("");
				args.back().push_back(c);
				state = 1;
			}
		}
		else
		{
			if (' ' != c && '\t' != c)
			{
				args.back().push_back(c);
			}
			else
			{
				state = 0;
			}
		}
	}

	return args;
}

class FilePointer
{
public:
	FILE* fp = nullptr;

	FilePointer();
	~FilePointer();
	void Open(const char fName[], const char mode[]);
	void Close(void);

};

FilePointer::FilePointer()
{
	fp = nullptr;
}
FilePointer::~FilePointer()
{
	Close();
}
void FilePointer::Open(const char fName[], const char mode[])
{
	fp = fopen(fName, mode);
}
void FilePointer::Close(void)
{
	if (nullptr != fp)
	{
		fclose(fp);
		fp = nullptr;
	}
}



class Line2D
{
protected:
	int x[2], y[2], c[3];

public:



	Line2D();
	~Line2D();
	void Clear(void);
	void MakeFromString(std::string, double hx, double hy, int flipx, int flipy, int scal);

	void Draw(double xc, double yc) const;
	double Hx, Hy;
	int fx, fy;

};

Line2D::Line2D()
{
	Hx = -350, Hy = 300;
	fx = 1, fy = 1;

	for (int i = 0; i < 2; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}
	for (int i = 0; i < 3; i++)
	{
		c[i] = 0;
	}

}

Line2D::~Line2D()
{
	Clear();
}
void Line2D::Draw(double xc, double yc) const
{
	// NOt printing here  - values are becoming 0
	//std::cout << x[0] << y[0] << x[1] << y[1] << "\n";
	//std::cout << c[0] << c[1] << c[2] << "\n";
	glColor3ub(c[0], c[1], c[2]);
	glBegin(GL_LINE_LOOP);

	glVertex2d(xc + x[0], yc + y[0]);
	glVertex2d(xc + x[1], yc + y[1]);

	glEnd();

	glColor3ub(0, 0, 200);
	glBegin(GL_POINTS);
	glVertex2d(xc, yc);
	glVertex2d(xc, yc + 10);
	glVertex2d(xc, yc + 20);
	glVertex2d(xc, yc - 10);
	glVertex2d(xc, yc - 20);
	glVertex2d(xc + 10, yc);
	glVertex2d(xc + 20, yc);
	glVertex2d(xc - 10, yc);
	glVertex2d(xc - 20, yc);

	glEnd();

}

void Line2D::Clear(void)
{
	Hx = -350, Hy = 300;
	fx = 1, fy = 1;

	for (int i = 0; i < 2; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}
	for (int i = 0; i < 3; i++)
	{
		c[i] = 0;
	}

}
int idx = 0;
void Line2D::MakeFromString(std::string str, double hx, double hy, int flipx, int flipy, int scal)
{
	auto args = Parse(str);
	if (2 <= args.size())
	{

		if (idx == 0)
		{
			//std::cout << hx << hy << flipx << flipy << "\n";
			idx++;
		}
		x[0] = (hx + flipx * atoi(args[0].c_str())) / scal;
		y[0] = (hy - flipy * atoi(args[1].c_str())) / scal;
		x[1] = (hx + flipx * atoi(args[2].c_str())) / scal;
		y[1] = (hy - flipy * atoi(args[3].c_str())) / scal;
		c[0] = atoi(args[4].c_str());
		c[1] = atoi(args[5].c_str());
		c[2] = atoi(args[6].c_str());
	}
	//std::cout << x[0] << y[0] << x[1] << y[1] <<"\n";
	//std::cout << c[0] << c[1] << c[2] << "\n";
}



class Drawing2D
{
protected:
	Line2D* l;

	int numlines = 0;
public:

	Drawing2D();
	~Drawing2D();
	double hx, hy;
	int flipx, flipy, scal = 1;
	void ClearUp(void);
	void Draw(double xc, double yc);
	bool ReadFile(const char fname[]);
};

Drawing2D::Drawing2D()
{
	l = nullptr;
	numlines = 0;
	hx = -300; hy = 300;
	flipx = 1; flipy = 1, scal = 5;

}

Drawing2D::~Drawing2D()
{
	ClearUp();
}

void Drawing2D::ClearUp(void)
{
	if (l != nullptr)
	{
		delete[] l;
		l = nullptr;
		numlines = 0;
		hx = -300; hy = 300;
		flipx = 1; flipy = 1, scal = 5;

	}

}

void Drawing2D::Draw(double xc, double yc)
{
	//std::cout << "The number of lines are : " << numlines;

	//std::cout << hx << hy << flipx << flipy << "\n";
	for (int i = 0; i < numlines; i++)
	{


		l[i].Draw(xc, yc);
	}


}

bool Drawing2D::ReadFile(const char fname[])
{
	ClearUp();

	FilePointer f;
	f.Open(fname, "r");
	if (nullptr != f.fp)
	{
		char str[256];
		bool firstLine = true;
		int nele = 0;
		int ne = 0;
		while (nullptr != fgets(str, 255, f.fp))
		{

			auto args = Parse(str);
			if (0 < args.size())
			{

				if (args.size() < 2)
				{
					numlines = atoi(args[0].c_str());
					ne = 0;
					l = new Line2D[numlines];
					//printf("Total number of lines %d \n", numlines);


				}
				else if (args.size() < 8)
				{
					if (ne < numlines)
					{
						//std::cout << "flag 2" << hx << hy << flipx << flipy << "\n";

						l[ne].MakeFromString(str, hx, hy, flipx, flipy, scal);
						ne++;
						//printf("%d \n", ne);
					}

				}
				else
					return false;

			}
			else
				return false;

		}
		return true;
	}
	return false;
}
// ************************* Rendering space ships ends ***************************************************


void Drawobstructions(double cx, double cy, double rad)
{


	glColor3ub(30, 1, 1);
	const double YS_PI = 3.1415927;
	glBegin(GL_POLYGON);

	double i;
	for (i = 0; i < 64; i++)
	{
		double angle = (double)i * YS_PI / 32.0;
		double x = (double)cx + cos(angle) * (double)rad;
		double y = (double)cy + sin(angle) * (double)rad;
		glVertex2d(x, y);
	}

	glEnd();
}



double CheckHit(
	double missileX, double missileY,
	double targetX, double targetY, double targetW, double targetH)
{
	double relX = missileX - targetX;
	double relY = missileY - targetY;
	/*
	glColor3ub(255, 0, 0);
	glBegin(GL_LINE_LOOP);

	glVertex2d(targetX - targetW / 2, targetY - targetH / 2);
	glVertex2d(targetX - targetW/2, targetY + targetH / 2);

	glVertex2d(targetX + targetW / 2, targetY + targetH / 2);

	glVertex2d(targetX + targetW / 2, targetY - targetH / 2);

	glEnd();
	*/

	if (-targetW / 2 <= relX && relX < targetW / 2 && -targetH / 2 <= relY && relY < targetH / 2)
	{
		return 1;
	}
	return 0;
}

double CheckHitObj(
	double missileX, double missileY,
	double targetX, double targetY, double targetW, double targetH)
{
	double relX = (missileX - targetX);
	double relY = (missileY - targetY);

	if (0 <= relX && relX < targetW && 0 <= relY && relY < targetH)
	{
		return 1;
	}
	return 0;
}
double CheckHitSpace(
	double ObjX, double ObjY,
	double ShipX, double ShipY, double objr, double shipr)
{
	double relX = (ObjX - ShipX) * (ObjX - ShipX);
	double relY = (ObjY - ShipY) * (ObjY - ShipY);
	double sum = sqrt(relX + relY);
	double sumr = objr + shipr;
	if (0 <= sum && sum < sumr)
	{
		return 1;
	}
	return 0;
}

// ********************** Predicting loss for obstruction **************************
int Predictloss(int dir, double xc, double yc, double vcx, double vcy, double ox, double oy, double ovx, double ovy, double orad, double sr, double dt, double tmy)
{
	int epoch = 30, loss = 0;
	double osc = 0;
	double scale = 7.5;
	//std::cout << tmy<<"\n";
	switch (dir)
	{
	case 1:
		vcx += scale * ovx;
		vcy += 0;
		break;
	case 2:
		vcx += 0;
		vcy += scale * ovy;
		break;
	case 3:
		vcx += -scale * ovx;
		vcy += 0;
		break;
	case 4:
		vcx += 0;
		vcy += -scale * ovy;
	default:
		vcx += scale * ovx;
		vcy += -scale * ovy;
		break;
	}
	for (int i = 1; i <= epoch; i++)
	{
		// Updating spaceship parameters
		osc = 300 * sin(PI / 20 * i);
		//std::cout << "\n Oscillation = " << osc << "\n";

		vcx = vcx - (10 + fabs(vcx / 500)) * vcx * dt;
		xc = xc + vcx * dt;
		vcy = vcy - (10 + fabs(vcy / 500)) * vcy * dt + osc;
		yc = yc + vcy * dt;
		// Updating object parameters 
		oy += ovy * dt;
		ox += ovx * dt;
		if (oy < 0
			|| oy > 600)
			ovy = -ovy;
		if (ox > 800 || ox < 0)
			ovx = -ovx;

		if ((CheckHitSpace(xc, yc, ox, oy, orad * 1.75, sr) == 1))
		{
			loss++;
		}



	}
	return loss;

}

int main(void)
{
	FsOpenWindow(0, 0, 800, 600, 1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	YsRawPngDecoder png1;
	YsRawPngDecoder png2;
	YsRawPngDecoder png3;
	if (YSOK == png1.Decode("Darth vader.png"))
	{
		png1.Flip();
		printf("%d x %d\n", png1.wid, png1.hei);
	}
	else
	{
		printf("Could not read the image.\n");
	}
	if (YSOK == png2.Decode("Win.png"))
	{
		png2.Flip();
		printf("%d x %d\n", png2.wid, png2.hei);
	}
	else
	{
		printf("Could not read the image.\n");
	}
	if (YSOK == png3.Decode("asteroid.png"))
	{
		png3.Flip();
		printf("%d x %d\n", png3.wid, png3.hei);
	}
	else
	{
		printf("Could not read the image.\n");
	}



	//FsChangeToProgramDir();
	YsSoundPlayer player;
	YsSoundPlayer::SoundData wav1;
	YsSoundPlayer::SoundData wav2;
	YsSoundPlayer::SoundData wav3;
	YsSoundPlayer::SoundData wav4;
	YsSoundPlayer::SoundData wav5;
	player.MakeCurrent();
	if (YSOK != wav1.LoadWav("hayden-folker-surrounded.wav"))
	{
		printf("Failed to read %s\n", "hayden-folker-surrounded.wav");

	}
	if (YSOK != wav2.LoadWav("bensound-epic.wav"))
	{
		printf("Failed to read %s\n", "bensound-epic.wav");

	}
	if (YSOK != wav3.LoadWav("laser.wav"))
	{
		printf("Failed to read %s\n", "laser.wav");

	}
	if (YSOK != wav4.LoadWav("Imperial song  John Williams.wav"))
	{
		printf("Failed to read %s\n", "Imperial song  John Williams.wav");

	}
	if (YSOK != wav5.LoadWav("bensound-newdawn.wav"))
	{
		printf("Failed to read %s\n", "bensound-newdawn.wav");

	}
	player.Start();
	player.PlayOneShot(wav1);

	// loop variables
	int terminate = 0, nshot = 0;
	//const double nTarget = 5;

	// missile variables
	const int n_mis = 5, noOfObtruct = 12;
	double mstate = 0, mx = 0, my = 0;
	double  mvx = 80, mvy; // If missle velocity is changed, pls change it in the prediction as well
	// target variables
	double  tx = 65 * pixel_len, ty = 600 - 60 * pixel_len, tw = 50, th = 100, tv = 10 * pixel_len, lifet = 0;
	double tstate = 0, tmstate = 0, tmx = tx, tmy = ty;
	//  Obstruction varialbles
	double ox[noOfObtruct], oy[noOfObtruct], orad[noOfObtruct], ostate[noOfObtruct], ovx[noOfObtruct], ovy[noOfObtruct], n_o = 0, nolv2 = 0, lv2 = 0;
	// Trajectory array
	double Trajmisx[160]; int l = 0;
	Trajmisx[l] = mx;
	l++;
	// Spcae ship variables 
	double xc = 100, yc = 100, vcx = 20 * pixel_len, vcy = 20 * pixel_len, life = 8;
	unsigned int initial_t = FsPassedTime();
	// music
	int music = 0;
	srand(time(nullptr));
	// Initializing obstructions
	for (int k = 0; k < noOfObtruct; k++)
	{
		if (k < 7)
		{
			ostate[k] = 1; // change it back to 1 or 2
			n_o = 7;
		}
		else
			ostate[k] = 0;


	label1:
		orad[k] = 5 * pixel_len;
		ox[k] = 300 + (rand() % 20) * pixel_len;
		oy[k] = 100 + (rand() % 300) * pixel_len;

		if (ox[k] + orad[k] > 800 - 95 || oy[k] + orad[k] > 600 - 95)
		{
			goto label1;
		}
		ovx[k] = (rand() % 10 + 50) * pixel_len;
		ovy[k] = (rand() % 10 + 50) * pixel_len;
		//std::cout << ow[k] << " : " << ox[k] << " : " << oh[k] << " : " << oy[k] << " : " << ov[k] << " \n";
	}
	//********************************************************************************************************
	Drawing2D mainspaceship, targetspaceship;
	mainspaceship.hx = -400; mainspaceship.hy = 300;
	mainspaceship.flipx = 1; mainspaceship.flipy = 1; mainspaceship.scal = 5;
	targetspaceship.hx = 350; targetspaceship.hy = 305;
	targetspaceship.flipx = -1; targetspaceship.flipy = 1; targetspaceship.scal = 2.5;

	if (true == mainspaceship.ReadFile("shape4.txt"))
	{
		printf("Friendly spaceship Read success.\n");
	}
	if (true == targetspaceship.ReadFile("shape3.txt"))
	{
		printf("Enemy spaceship Read success.\n");
	}
	// Moving spaceship based on time
	double errx, erry, magerrx = 0, magerry = 0, serrx, serry;
	int itr = 0, itr1 = 0, itr2 = 0, itrl1 = 0, itrl2 = 0;
	for (;;)
	{

		double  dt = 0.025;
		FsPollDevice();

		auto  key = FsInkey();

		itr++;
		itr1++;
		itr2++;
		itrl1++;
		itrl2++;
		//std::cout << " The value if itr is :" << itr << "\n";
		if (itr > 20 || itr1 > 30)
		{
			//std::cout << "Flag Update \n";
			itr = 0;
			itr1 = 0;

		labelr:
			errx = ((16 - xc) * 0.15), erry = (300 - yc) * 0.15;
			magerrx = sqrt(fabs(errx)), magerry = sqrt(fabs(erry));
			serrx = (errx) / (magerrx), serry = (erry) / (magerry);
			//std::cout << "The valu of errors are " << magerrx<< "  " << magerry << "\n";
			vcx += magerrx * serrx * ((rand() % 10) * pixel_len /* - (rand() % 10) * pixel_len*/);
			vcy += magerry * serry * ((rand() % 10) * pixel_len /* - (rand() % 10) * pixel_len */);

			//std::cout << " THe value of vcx and vcy: " << vcx << vcy << "\n";
			if (0 == vcx || 0 == vcy)
			{
				goto labelr;
			}
		}
		switch (key)
		{
		case FSKEY_ESC:
			return 0;

			break;

		case FSKEY_Q:
			terminate = 1;
			break;
		case FSKEY_W:
			terminate = 2;
			break;

			/*case FSKEY_LEFT:

				vcx -= 4 * pixel_len * 10;
				break;
			case FSKEY_RIGHT:
				vcx += 4 * pixel_len *10;
				break;
			case FSKEY_UP:
				vcy += 4 * pixel_len *10;
				break;
			case FSKEY_DOWN:
				vcy -= 4 * pixel_len *10;
				break;

			case FSKEY_SPACE:

				// Missle shooting
				if (0 == mstate)
				{
					l = 0;
					mstate = 1;
					mx = xc;
					my = yc;
					nshot += 1;
					mvx = 80; mvy = 0;
					//std::cout << "\n Number of strike taken till now : " << nshot;
				}
				//*************************************************************
				break;
				*/
		}

		//std::cout << "flag 2";
		// Position calculation of the space ship

		double osc = 400 * sin(PI / 20 * itr2);
		//std::cout << "\n Oscillation = " << osc << "\n";

		vcx = vcx - (10 + fabs(vcx / 500)) * vcx * dt;
		xc = xc + vcx * dt;
		vcy = vcy - (10 + fabs(vcy / 500)) * vcy * dt + osc;
		yc = yc + vcy * dt;
		// ********************************************************************

		// Boundary conditions for y motion of the spaceship
		if (yc > 540)
		{
			yc = 540;
		}
		else if (yc < 60)
		{
			yc = 60;
		}
		if (xc > 750)
		{
			xc = 750;
		}
		else if (xc < 60)
		{
			xc = 60;
		}
		if (tstate != 0)
		{
			if (xc > tx - 150)
				xc = tx - 150;
		}
		// Motion of the target 
		if (tstate != 0)
		{
			ty += tv * dt;
			if (ty < 0 || ty > 600)
				tv = -tv;

		}
		//************************************************************************************************
		// Motion of the obstructions :
		for (int m = 0; m < noOfObtruct; m++)
		{
			if (ostate[m] > 0)
			{
				oy[m] += ovy[m] * dt;
				ox[m] += ovx[m] * dt;
				if (oy[m] - 65 < 0
					|| oy[m] + 65 > 600)
					ovy[m] = -ovy[m];
				if (ox[m] + 65 > 800 || ox[m] - 65 < 0)
					ovx[m] = -ovx[m];
			}
		}
		// ***********************************************************************************************
		// Rendering >>
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ***********************************************************************************************
		//************************************************************************************************
		// Drawing obstructions
		for (int k = 0; k < noOfObtruct; k++)
		{
			if (ostate[k] > 0)
			{
				//Drawobstructions(ox[k], oy[k], orad[k]);
				glRasterPos2i(ox[k] - 50, oy[k] + 50);
				glDrawPixels(png3.wid, png3.hei, GL_RGBA, GL_UNSIGNED_BYTE, png3.rgba);
				glColor3ub(0, 0, 200);
				glBegin(GL_POINTS);
				glVertex2d(ox[k], oy[k]);
				glVertex2d(ox[k], oy[k] + 10);
				glVertex2d(ox[k], oy[k] + 20);
				glVertex2d(ox[k], oy[k] - 10);
				glVertex2d(ox[k], oy[k] - 20);
				glVertex2d(ox[k] + 10, oy[k]);
				glVertex2d(ox[k] + 20, oy[k]);
				glVertex2d(ox[k] - 10, oy[k]);
				glVertex2d(ox[k] - 20, oy[k]);

				glEnd();

			}

		}
		//************************************************************************************************
		// Drawing spaceship with center 

		 // flipping the image

		mainspaceship.Draw(xc, yc);
		DrawLifebar(life, 1);
		// ******************************
		// Missile trajectory
		if (mstate != 0)
		{


			if (my < 0 || my > 600 || mx > 800 || mx < 0)
			{
				//std::cout << " flag 4 : ";
				mstate = 0;
				for (int i = 0; i < l - 1; i++)
				{
					Trajmisx[i] = 0;

				}
				l = 0;
			}



			DrawMissile(mx, my);

			mvx = mvx;
			mvy = mvy;

			mx = mx + mvx * dt * pixel_len;
			my = my + mvy * dt * pixel_len;
			//std::cout << "flag l : " << l << " \n ";//<< my << "  " << mvx << "  " << mvy;
			//std::cout << "\n";
			// Printing Trajecotry 
			Trajmisx[l] = mx;

			l++;
			// Print Line Trajecotry
			for (int i = 0; i < l - 3; i++)
			{
				//std::cout << Trajmisx[i] << "\n";
				glEnable(GL_LINE_STIPPLE);
				glLineStipple(1, 0xcccc);
				glBegin(GL_LINES);
				glColor3ub(255, 0, 0);
				glVertex2d(Trajmisx[i], my + 15);
				glVertex2d(Trajmisx[i] + 15, my + 15);
				glEnd();
				glDisable(GL_LINE_STIPPLE);

				glEnable(GL_LINE_STIPPLE);
				glLineStipple(1, 0xff00);
				glBegin(GL_LINES);
				glVertex2d(Trajmisx[i], my - 15);
				glVertex2d(Trajmisx[i] + 15, my - 15);
				glEnd();
				glDisable(GL_LINE_STIPPLE);
			}

		}
		// ****************************************************************************************
		// Drawing target
		if (0 == tstate && n_o <= 0)
		{

			targetspaceship.Draw(tx, ty);

			tstate = 15;
			DrawLifebar(tstate, 2);
			//std::cout << "The value of enemy state is " << tstate << "\n";
		}
		if (tstate != 0 && n_o <= 0)
		{
			// tstate not reducing properly so as of now its life is inf

			player.Stop(wav1);
			if (music == 0)
			{
				player.PlayOneShot(wav2);
				music = 1;
			}
			targetspaceship.Draw(tx, ty);
			DrawLifebar(tstate, 2);
			//printf("Flag !!! \n");
			//std::cout << "The value of enemy state is in 842 " << tstate << "\n";
			if (lv2 == 0 && nolv2 <= 0)
			{

				nolv2 = 12;
				//std::this_thread::sleep_for(std::chrono::milliseconds(150));
				for (int k = 0; k < noOfObtruct; k++)
				{
					if (k <= nolv2)
					{
						ostate[k] = 1;
						ox[k] = tx;
					}
					else
						ostate[k] = 0;
				}
				lv2 = 1;
			}
			//std::cout << "The value of enemy state is in 860 " << tstate << "\n";
			//std::cout << "flag 5 : ";
		}
		//std::cout << "The value of enemy state is in 863 " << tstate << "\n";
		// ********************* Darwing laser from Target *********************************


		//printf("Flag x \n");
		if ((tstate <= 5 || nolv2 <= 4) && n_o <= 0)
		{
			//printf("Flag !!! \n");
			//printf("Not logical value at line number %d in file %s\n", __LINE__, __FILE__);
			if (itrl1 > 40)
			{
				//printf("Not logical value at line number %d in file %s\n", __LINE__, __FILE__);
				if (itrl1 == 41)
				{
					//player.Stop(wav2);
					player.PlayOneShot(wav3);
				}

				tmstate = 1, tmx = tx, tmy = ty;

			}
			if (itrl1 > 80)
			{
				//printf("Not logical value at line number %d in file %s\n", __LINE__, __FILE__);

				if (itrl1 == 80)
				{
					player.Stop(wav3);
					//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					//layer.PlayOneShot(wav2);
				}
				tmstate = 0;
				itrl1 = 0;
				tmx = 0;

			}
			if (tmstate == 1)
				Drawlaser(tmx, tmy);



			tmx = tmx - 80 * dt * pixel_len;

		}
		// ***************Check for laser hit on spaceship **************************************
		//std::cout << tmstate << "\n";
		if (tmstate != 0 && (tmy - 25 < yc) && (yc < tmy + 25))
		{
			printf("Value at line number %d in file %s\n", __LINE__, __FILE__);
			life--;

			yc = 800 - tmy;
			if (tmy < 500 && tmy >300)
			{
				if (tv < 0)
					yc = 800;
				if (tv > 0)
					yc = 0;
			}
			for (int i = 1; i < 5; i++)
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
				DrawExplosion(xc, yc, 5 * i, 1);

			}
		}

		// ****************************************************************************************
		// Checking Hits in both the cases target and  space ship

		if ((CheckHit(mx, my + 15, tx, ty, tw * 2.5, th * 1.5) == 1 || CheckHit(mx, my - 15, tx, ty, tw * 2.5, th * 1.5) == 1) && (tstate != 0) && (mstate != 0))
		{
			printf("Hit!\n");



			//std::cout << "The value of enemy state is in 874 " << tstate << "\n";
			lifet++;
			tstate = tstate - 1;
			mstate = 0;
			//std::cout << "The value of enemy state is in 878 " << tstate << "\n";



		// Explosion parameters
			for (int i = 1; i < 10; i++)
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
				DrawExplosion(mx, my, 5 * i, 1);

			}

			if (tstate <= 0)
			{
				for (int i = 1; i < 10; i++)
				{
					//std::this_thread::sleep_for(std::chrono::milliseconds(50));
					DrawExplosion(mx, my, 5 * i, 1);

				}
				for (int i = 10; i > 0; i--)
				{
					//std::this_thread::sleep_for(std::chrono::milliseconds(50));
					DrawExplosion(mx, my, 5 * i, 1);

				}
				terminate = 2;
			}


		}
		// For obstructions 
		for (int m = 0; m < noOfObtruct; m++)
		{
			if (ostate[m] > 0)
			{
				if ((CheckHitObj(mx, my + 15, ox[m], oy[m], orad[m] * 1.3, orad[m] * 1.3) == 1 || CheckHitObj(mx, my - 15, ox[m], oy[m], orad[m] * 1.3, orad[m] * 1.3) == 1) && mstate != 0)
				{


					//printf("\nHit an obtructions !\n");
					//printf("You shot %d set of missile till now.\n", nshot);
					//std::cout << "\n flag ostate \n" << ostate[m];
					ostate[m] --;

					if (ostate[m] <= 0)
					{
						n_o -= 1;
						//std::cout << "Flag g the num obstructions are: " << n_o << "\n";

						if (n_o <= 0)
							nolv2--;
						//std::cout << "Flag g the lvl2 num obstructions are: " << nolv2 << "\n";
						for (int i = 1; i < 5; i++)
						{

							DrawExplosion(mx, my, 5 * i, 1);

						}
						// for (int i = 10; i > 0; i--)
						// {

						// 	DrawExplosion(mx, my, 5 * i, 1);

						// }
						mstate = 0;
						break;
					}

					//printf("Number of obstructions Remaining %d .\n", noOfObtruct);
					mstate = 0;
					for (int i = 0; i < l - 1; i++)
					{
						Trajmisx[i] = 0;

					}
					l = 0;

				}
			}

		}
		int loss1 = 0, loss2 = 0, loss3 = 0, loss4 = 0;
		if (tmstate != 0)
		{
			if ((tmy - 25 > yc) && tv > 0)
			{
				loss2 = 15;
				//std::cout << "flag\n";
			}

			if ((yc > tmy + 25) && tv < 0)
			{
				loss4 = 15;
				//std::cout << "flag\n";
			}

		}

		for (int m = 0; m < noOfObtruct; m++)
		{
			if (ostate[m] > 0)
			{
				// ****************** Automating the motion of the spaceship******************************
				if ((CheckHitSpace(xc, yc, ox[m], oy[m], orad[m] * 2.25, 100) == 1 && life > 0))// Considering the sapce ship as a circle with effective radius 30
				{
					itr = 0;// for avoid change in direction during random times 
					int dir = 0; // checking losses in future for 4 direction addition of velocity in +- vx and +- vy

					loss1 = Predictloss(1, xc, yc, vcx, vcy, ox[m], oy[m], ovx[m], ovy[m], orad[m], 100, dt, tmy);
					loss2 = Predictloss(2, xc, yc, vcx, vcy, ox[m], oy[m], ovx[m], ovy[m], orad[m], 100, dt, tmy);
					loss3 = Predictloss(3, xc, yc, vcx, vcy, ox[m], oy[m], ovx[m], ovy[m], orad[m], 100, dt, tmy);
					loss4 = Predictloss(4, xc, yc, vcx, vcy, ox[m], oy[m], ovx[m], ovy[m], orad[m], 100, dt, tmy);
					//std::cout << "loss values are"<< loss1 << loss2 << loss3 << loss4 <<"\n";



					if (loss4 <= loss1 && loss4 <= loss2 && loss4 <= loss3)
						dir = 4;
					if (loss3 <= loss1 && loss3 <= loss2 && loss3 <= loss4)
						dir = 3;
					if (loss2 <= loss1 && loss2 <= loss3 && loss2 <= loss4)
						dir = 2;
					if (loss1 <= loss2 && loss1 <= loss3 && loss1 <= loss4)
						dir = 1;
					int scale = 5;
					switch (dir)
					{
					case 1:
						vcx += scale * ovx[m];
						vcy += 0;
						break;
					case 2:
						vcx += 0;
						vcy += scale * ovy[m];
						break;
					case 3:
						vcx += -scale * ovx[m];
						vcy += 0;
						break;
					case 4:
						vcx += 0;
						vcy += -scale * ovy[m];
					default:
						vcx += -scale * ovx[m];
						vcy += +scale * ovy[m];
						break;
					}

					/*
					printf("\nHit an obtructions !\n");
					std::cout << life;
					--life;
					xc = 16;
					yc = 16;
					ostate[m] = 0;
					n_o--;*/




				}
				/*if ((CheckHitSpace(xc, yc, ox[m], oy[m], orad[m], 100) == 1 && life > 0))// Considering the sapce ship as a circle with effective radius 30
				{
					--life;

					ostate[m] = 0;
					n_o--;
					break;

				}*/
			}


		}
		// **************Automating the missile shooting**************************************

		for (int m = 0; m < noOfObtruct; m++)
		{
			int er = -10;
			if ((ox[m] - xc) > 0 && ostate[m] != 0 && mstate == 0)
			{


				if ((fabs(oy[m] - yc) <= (fabs(ovy[m] / (oy[m] - yc)) * 80)) && ((fabs(oy[m] + ovy[m] * dt - yc)) < fabs(oy[m] - yc)))
				{
					if (0 == mstate)
					{
						l = 0;
						mstate = 1;
						mx = xc;
						my = yc;
						nshot += 1;
						mvx = 80; mvy = 0;
						//std::cout << "\n Number of strike taken till now : " << nshot;
					}
				}
			}
			if ((xc - tx) < 0 && tstate != 0 && mstate == 0)
			{


				if ((fabs(ty - yc) <= (fabs(tv / (ty - yc)) * 80) + er) && ((fabs(ty + ty * dt - yc)) < fabs(ty - yc)))
				{
					if (0 == mstate)
					{
						l = 0;
						mstate = 1;
						mx = xc;
						my = yc;
						nshot += 1;
						mvx = 80; mvy = 0;
						//std::cout << "\n Number of strike taken till now : " << nshot;
					}
				}
			}
		}

		// ***********************************************************************************
		// *********************** Checking life and termination *********************************

		if (life == 0 || life < 0)
		{
			//std::cout << "flag";
			terminate = 1;


		}
		//terminate = 2;
		if (terminate == 1)
		{


			player.Stop(wav1);
			player.Stop(wav2);
			player.Stop(wav3);
			if (m4 == 0)
			{
				player.PlayOneShot(wav4);
				m4 = 1;
			}
			time_t start_time = time(NULL);
			while (int(time(NULL) - start_time) < 39)
			{
				FsPollDevice();
				auto  key = FsInkey();
				if (key == FSKEY_ESC)
					return 0;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glRasterPos2i(0, 599);
				glDrawPixels(png1.wid, png1.hei, GL_RGBA, GL_UNSIGNED_BYTE, png1.rgba);

				glColor3f(1, 0, 0);
				glRasterPos2i(300, 300);
				YsGlDrawFontBitmap20x28("Game Over");
				FsSwapBuffers();
			}
			break;

		}

		if (terminate == 2)
		{

			player.Stop(wav1);
			player.Stop(wav2);
			player.Stop(wav3);
			if (m4 == 0)
			{
				player.PlayOneShot(wav5);
				m4 = 1;
			}
			time_t start_time = time(NULL);
			while (int(time(NULL) - start_time) < 30)
			{
				FsPollDevice();
				auto  key = FsInkey();
				if (key == FSKEY_ESC)
					return 0;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glRasterPos2i(0, 599);
				glDrawPixels(png2.wid, png2.hei, GL_RGBA, GL_UNSIGNED_BYTE, png2.rgba);

				glColor3f(0, 1, 0);
				glRasterPos2i(325, 525);
				YsGlDrawFontBitmap20x28("You Won");
				FsSwapBuffers();
			}
			break;
		}
		FsSwapBuffers();
		//FsSleep(10); 
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	return terminate;
}