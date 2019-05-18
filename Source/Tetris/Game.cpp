#include "Game.h"
#include "stdafx.h"

Game::Game(DrawEngine &de) :
de(de), isPaused(false)
{
	level = new Level(de, 10, 20);
}

Game::~Game()
{
	delete level;
}

//void Game::newGame()
//{
//	//level = new Level(de, 10, 20);
//	//isPaused = false;
//	repaint();
//}

void Game::restart()
{
	delete level;
	level = new Level(de, 10, 20);
	isPaused = false;
	repaint();
}

bool Game::keyPress(int vk)
{
	// When pausing, ignore keys other than ESCAPE and ENTER
	if (vk != VK_ESCAPE && vk != VK_RETURN && isPaused)
		return false;
	switch (vk)
	{
	case VK_UP:
		level->rotate();
		break;
	case VK_DOWN:
		level->move(0, -1);
		break;
	case VK_LEFT:
		level->move(-1, 0);
		break;
	case VK_RIGHT:
		level->move(1, 0);
		break;
	case VK_SPACE:
		level->move(0, -1);
		level->move(0, -2);
		level->move(0, -3);
		level->move(0, -4);
		level->move(0, -5);
		level->move(0, -6);
		level->move(0, -7);
		level->move(0, -8);
		level->move(0, -9);
		level->move(0, -10);
		level->move(0, -11);
		level->move(0, -12);
		level->move(0, -13);
		level->move(0, -14);
		level->move(0, -15);
		level->move(0, -16);
		level->move(0, -17);
		level->move(0, -18);
		level->move(0, -19);
		break;
	case 0x43: // VK_KEY_C
		level->holdPiece();
		repaint();
		break;
	case VK_ESCAPE:
		pause(!isPaused);
		break;
	case VK_RETURN:
		// Restart on game over
		if (level->isGameOver())
			restart();		
	default:
		return false;
	}
	return true;
}

void Game::timerUpdate()
{
	// Don't update game when pausing
	if (isPaused)
		return;

	// If game is over, show GAME OVER
	if (level->isGameOver())
	{
		isPaused = true;
		MessageBox(NULL, L"GAME OVER", L"!!!", MB_OK);
		drawGameOver();
		return;
	}


	// Update game data
	level->timerUpdate();

	// Redraw
	level->drawBoard();
}

void Game::pause(bool paused)
{
	// Don't pause if game is over
	if (isGameOver())
		return;

	isPaused = paused;
	if (paused)
	{
		mciSendString(L"pause mp3", NULL, 0, NULL);
		drawPause();
	}
	else
	{
		mciSendString(L"resume mp3", NULL, 0, NULL);
		level->drawScore();
		level->drawSpeed();
		level->drawLine();
	}
}


void Game::repaint() const
{
	de.drawInterface();
	level->drawScore();
	level->drawSpeed();
	level->drawLine();
	level->drawNextPiece();
	level->drawHoldPiece();
	level->drawBoard();
	if (level->isGameOver())
	{
		MessageBox(NULL, L"GAME OVER", L"!!!", MB_OK);
		drawGameOver();
	}
	else if (isPaused)
		drawPause();
}

bool Game::isGameOver() const
{
	return level->isGameOver();
}
