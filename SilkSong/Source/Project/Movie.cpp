#include "Movie.h"
#include "Components/VideoPlayer.h"
#include "GameplayStatics.h"
#include <graphics.h>


Movie::Movie()
{
	player = ConstructComponent<VideoPlayer>();
	player->AttachTo(root);

	player->AddPosition({0, -5});
	player->SetRect(FRect({}, WIN_WIDTH, WIN_HEIGHT - 50));
	player->Load("video_start");
	player->Play();

	player->OnVideoEnd.AddLambda([]() {GameplayStatics::OpenLevel("MossHole"); });
}

void Movie::Update(float deltaTime)
{
	Actor::Update(deltaTime);
	if (bSkipped) return;

	settextstyle(28, 0, _T("Arial"));
	settextcolor(RGB(255, 255, 255));
	setbkmode(TRANSPARENT);
	LPCTSTR tip = _T("Press Enter to Skip");
	int tw = textwidth(tip);
	outtextxy(WIN_WIDTH - tw - 30, WIN_HEIGHT - 80, tip);

	if (GetAsyncKeyState(0x0D) & 0x8000)
	{
		bSkipped = true;
		GameplayStatics::OpenLevel("MossHole");
	}
}
