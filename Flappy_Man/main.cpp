
#include <iostream>
#include <string>
#include <fstream>
#include "olcConsoleGameEngine.h"

using namespace std;

class FlappyManConsole : public olcConsoleGameEngine
{
public:
	FlappyManConsole()
	{
		m_sAppName = L"Flappy Man";
	}
private:
	float fManPosition = 0.0f;
	float fManVelocity = 0.0f;
	float fManAcceleration = 0.0f;
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
			fManAcceleration = 0.0f;
			fManVelocity = 0.0f;
			fManPosition = ScreenHeight() / 2.0f;
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

			if (m_keys[VK_SPACE].bPressed && fManVelocity >= fGravity / 15.0f)
			{
				fManAcceleration = 0.0f;
				fManVelocity = -fGravity / 4.5f;
				nFlapCount++;
				if (nFlapCount > nMaxFlapCount)
					nMaxFlapCount = nFlapCount;
			}
			else
			{
				fManAcceleration += fGravity * fElapsedTime;
			}


			if (fManAcceleration >= fGravity)
				fManAcceleration = fGravity;

			fManVelocity += fManAcceleration * fElapsedTime;
			fManPosition += fManVelocity * fElapsedTime;

			//Log << "fManVelocity = " << fManVelocity << " fManPosition = "   << fManPosition << "\n";

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
			//Log << "fLevelPosition apr�s " << fLevelPosition << std::endl;
			int nSection = 0;
			for (auto s : listSection)
			{
				if (s != 0 && nFlapCount < 10)
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
				else if (s!= 0  && nFlapCount >= 10  && nFlapCount<20)
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

			int nManX = (int)(ScreenWidth() / 3.0f); //Position of the Man 

			// Collision Detection
			bHasCollided = fManPosition < 2 || fManPosition > ScreenHeight() - 2 ||
				m_bufScreen[(int)(fManPosition + 0) * ScreenWidth() + nManX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fManPosition + 1) * ScreenWidth() + nManX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fManPosition + 2) * ScreenWidth() + nManX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fManPosition + 0) * ScreenWidth() + nManX + 6].Char.UnicodeChar != L' ';

			//Draw Man
			if (fManVelocity > 0) // Man falling
			{
				DrawString(nManX, fManPosition + 0, L" U");
				DrawString(nManX, fManPosition + 1, L"-|-");
				DrawString(nManX, fManPosition + 2, L" @ ");
			}
			else
			{
				DrawString(nManX, fManPosition + 0, L">-|-@");
				//DrawString(nManX, fManPosition + 1, L"///");
			}

			DrawString(1, 1, L"Attempts : " + to_wstring(nAttemptCount) + L"     CurrentScore : " + to_wstring(nFlapCount) + L"      High Score : " + to_wstring(nMaxFlapCount));

		}
		return true;
	}

};


int main()
{
	FlappyManConsole game;
	game.ConstructConsole(100, 70, 8, 8); //80 Width , 100 Height , 8x8 pixels
	game.Start();

	return 0;
}