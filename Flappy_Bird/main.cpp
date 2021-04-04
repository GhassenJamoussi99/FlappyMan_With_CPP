
#include <iostream>
#include <string>
#include <fstream>
#include "olcConsoleGameEngine.h"

using namespace std;

class FlappyBirdConsole : public olcConsoleGameEngine
{
public:
	FlappyBirdConsole()
	{
		m_sAppName = L"Flappy Bird";
	}
private:
	float fBirdPosition = 0.0f;
	float fBirdVelocity = 0.0f;
	float fBirdAcceleration = 0.0f;
	float fGravity = 100.0f;

	float fSectionWidth;
	list<int> listSection;
	float fLevelPosition = 0.0f;

	bool bHasCollided = false;
	bool bResetGame = false;

	int nAttemptCount = 0;
	int nFlapCount = 0;
	int nMaxFlapCount = 0;


protected:
	virtual bool OnUserCreate()
	{
		listSection = { 0, 0, 0, 0};
		fSectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);
		bResetGame = true;
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bResetGame)
		{
			bHasCollided = false;
			bResetGame = false;
			listSection = { 0, 0, 0, 0 };
			fBirdAcceleration = 0.0f;
			fBirdVelocity = 0.0f;
			fBirdPosition = ScreenHeight() / 2.0f;
			nFlapCount = 0;
			nAttemptCount++;
		}

		if (bHasCollided)
		{
			if (m_keys[VK_SPACE].bReleased) // Or pressed ?
				bResetGame = true;
		}
		else
		{

			std::ofstream Log;
			Log.open("Logfile.txt", std::ios_base::app);

			if (m_keys[VK_SPACE].bPressed && fBirdVelocity >= fGravity / 15.0f)
			{
				fBirdAcceleration = 0.0f;
				fBirdVelocity = -fGravity / 4.5f;
				nFlapCount++;
				if (nFlapCount > nMaxFlapCount)
					nMaxFlapCount = nFlapCount;
			}
			else
			{
				fBirdAcceleration += fGravity * fElapsedTime;
			}


			if (fBirdAcceleration >= fGravity)
				fBirdAcceleration = fGravity;

			fBirdVelocity += fBirdAcceleration * fElapsedTime;
			fBirdPosition += fBirdVelocity * fElapsedTime;

			//Log << "fBirdVelocity = " << fBirdVelocity << " fBirdPosition = "   << fBirdPosition << "\n";

			fLevelPosition += 14.0f * fElapsedTime;
			//Log << "fLevelPosition avant " << fLevelPosition << std::endl;


			if (fLevelPosition > fSectionWidth)
			{
				fLevelPosition -= fSectionWidth;
				listSection.pop_front();
				int i = rand() % (ScreenHeight() - 20);
				if (i <= 10) i = 0;
				listSection.push_back(i);
			}

			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
			//Log << "fLevelPosition aprés " << fLevelPosition << std::endl;
			int nSection = 0;
			for (auto s : listSection)
			{
				if (s != 0)
				{
					//Bottom rectangle
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(),
						PIXEL_SOLID, FG_GREEN);

					//Upper rectangle
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, 0,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 20,
						PIXEL_SOLID, FG_GREEN);
				}
				else if (s!= 0  && nFlapCount > 10  && nFlapCount<20)
				{
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(),
						PIXEL_SOLID, FG_RED);

					//Upper rectangle
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, 0,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 20,
						PIXEL_SOLID, FG_RED);

				}
				else if (s != 0 && nFlapCount >= 20)
				{
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(),
						PIXEL_SOLID, FG_CYAN);

					//Upper rectangle
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, 0,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 20,
						PIXEL_SOLID, FG_CYAN);

				}
				nSection++;

			}

			int nBirdX = (int)(ScreenWidth() / 3.0f); //Position of the bird 

			// Collision Detection
			bHasCollided = fBirdPosition < 2 || fBirdPosition > ScreenHeight() - 2 ||
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ';

			//Draw Bird
			if (fBirdVelocity > 0) // Bird falling
			{
				DrawString(nBirdX, fBirdPosition + 0, L"\\\\\\");
				DrawString(nBirdX, fBirdPosition + 1, L"<\\\\\\=Qq");
			}
			else
			{
				DrawString(nBirdX, fBirdPosition + 0, L"<///=Q");
				DrawString(nBirdX, fBirdPosition + 1, L"///");
			}

			DrawString(1, 1, L"Attempts : " + to_wstring(nAttemptCount) + L"     CurrentScore : " + to_wstring(nFlapCount) + L"      High Score : " + to_wstring(nMaxFlapCount));

		}
		return true;
	}

};


int main()
{
	FlappyBirdConsole game;
	game.ConstructConsole(100, 70, 8, 8); //80 Width , 100 Height , 8x8 pixels
	game.Start();

	return 0;
}