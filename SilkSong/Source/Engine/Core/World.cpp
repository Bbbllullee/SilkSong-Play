#include"World.h"
#include"GameplayStatics.h"
#include"Tools/LevelManager.h"
#include"Tools/Timer.h"
#include"Tools/VisualInterface.h"
#include"Components/Camera.h"
#include"Components/Collider.h"
#include"Components/RigidBody.h"
#include"Objects/Controller.h"
#include"UI/UserInterface.h"
#include"Objects/Level.h"



int Object::number = 0;
extern World mainWorld{};


bool ArtyEngine::LayerSort::operator()(const LayerInterface* a, const LayerInterface* b) const
{
	if (a->GetLayer() == b->GetLayer()) return a < b;
	else return a->GetLayer() < b->GetLayer();
}

bool ArtyEngine::ColliderSort::operator()(const Collider* a, const Collider* b)const
{
	if (a->GetLayer() == b->GetLayer()) return a < b;
	else return a->GetLayer() < b->GetLayer();
}




void World::Update(float deltaTime)
{
	if (pauseDelay > 0 && OverallClock->GetDelay() > lastPauseTime + pauseDelay)
	{
		pauseDelay = 0.f;
	}

	if (pauseDelay == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			ProcessCollisions(deltaTime * 0.25f);
		}
	}
	ProcessColliders();

	GameplayStatics::GetController()->PeekInfo();//���������������룩

	if (pauseDelay == 0 && !levelManager->level_to_delete)
	{
		currentLevel->Update(deltaTime);
		for (auto& obj : GameActors)obj->Update(deltaTime);
		for (auto& obj : OverallGameActors)obj->Update(deltaTime);
	}

	{
		std::lock_guard<std::mutex> lock(updateMutex);
		
		for (auto& obj : GameActors_to_add)
		{
			GameActors.insert(obj);
			obj->BeginPlay();
		}
		GameActors_to_add.clear();

		for (auto& obj : OverallGameActors_to_add)
		{
			GameActors.erase(obj);
			OverallGameActors.insert(obj);
		}
		OverallGameActors_to_add.clear();

		for (auto& obj : OverallGameActors_to_erase)
		{
			OverallGameActors.erase(obj);
			GameActors.insert(obj);
		}
		OverallGameActors_to_erase.clear();

		for (auto& obj : GameActors_to_delete)
		{
			obj->EndPlay();
			GameActors.erase(obj);
			OverallGameActors.erase(obj);//ȫ�ֶ����������٣����ض����ж�obj�Ƿ���ȫ�ֶ����Խ�ʡʱ��
			delete obj;
		}
		GameActors_to_delete.clear();
	}

	for (auto& obj : GameUIs)obj->Update(deltaTime);
	for (auto& obj : OverallGameUIs)obj->Update(deltaTime);

	{
		std::lock_guard<std::mutex> lock(updateMutex);

		for (auto& obj : GameUIs_to_add)
		{
			GameUIs.insert(obj);
		}
		GameUIs_to_add.clear();

		for (auto& obj : OverallGameUIs_to_add)
		{
			GameUIs.erase(obj);
			OverallGameUIs.insert(obj);
		}
		OverallGameUIs_to_add.clear();

		for (auto& obj : OverallGameUIs_to_erase)
		{
			OverallGameUIs.erase(obj);
			GameUIs.insert(obj);
		}
		OverallGameUIs_to_erase.clear();

		for (auto& obj : GameUIs_to_delete)
		{
			GameUIs.erase(obj);
			OverallGameUIs.erase(obj);//ȫ�ֶ����������٣����ض����ж�obj�Ƿ���ȫ�ֶ����Խ�ʡʱ��
			delete obj;
		}
		GameUIs_to_delete.clear();
	}

	if (pauseDelay == 0 && !levelManager->level_to_delete)
	{
		for (auto& obj : GameTimers)
		{
			obj->Execute();
		}
	}


	if (OverallGameActors_to_erase.empty() && OverallGameUIs_to_erase.empty())
	{
		std::lock_guard<std::mutex> lock(updateMutex);
		levelManager->RefreshLevel();//���ؿ�����
	}
}


void World::ProcessColliders()
{
	/**
	 * ��ײɾ����Ϣ����
	 **/
	for (auto& it : GameColliders)it->Erase();

	/**
	 * ��ײ���Ƴ�����
	 **/
	{
		std::lock_guard<std::mutex> lock(updateMutex);

		for (auto& it : GameColliders_to_clear)it->Clear();
		GameColliders_to_clear.clear();
	}
}

void World::ProcessCollisions(float deltaTime)
{
	for (auto& it : GameRigids)it->PreciseUpdate(deltaTime);

	for (auto& it : GameColliders)it->ColliderZoneTick();

	/**
     * ��ײ������Ϣ����
     **/
	for (auto& arr_i : ColliderZones)for (auto& arr_j : arr_i) if (!arr_j.empty())
		for (auto& me : arr_j)for (auto& he : arr_j) if (he != me) { me->Insert(he); if (levelManager->level_to_delete)return; }
}


void World::Render()
{
	if (videoLock)
	{
		return;
	}

	cleardevice();

	for (auto& obj : GameRenderers) obj->Render();
	for (auto& obj : GameUIs)obj->ShowInfoBoxs();

	ImageToolkit::ApplyGaussianFilterOnScreen();

	Debug();

	FlushBatchDraw(); 
}

void World::Update_()
{
	mainCamera->Calculate();
	GameplayStatics::GetController()->MouseTick();
}

void World::Input()
{
	GameplayStatics::GetController()->PeekInfo_();
}

void World::Debug()
{
	setlinestyle(PS_SOLID | PS_JOIN_BEVEL);
#ifdef _DEBUG
#endif


	static int FPS = 0;
	static int number = 0;
	number++;

	settextstyle(25, 10, "Arial");
	settextcolor(RGB(255, 200, 15));
	outtextxy(0, 0, (std::string("FPS: ") + std::to_string(FPS)).c_str());
	if (FPSClock->GetDelay() >= 1.0)
	{
		FPS = number;
		FPSClock->Reset();
		number = 0;
	}
}

void World::WipeData()
{
	for (auto& obj : GameActors)obj->EndPlay();
	for (auto& obj : GameActors)delete obj;
	for (auto& obj : GameUIs)delete obj;
	for (auto& obj : GameActors_to_add)delete obj;
	for (auto& obj : GameUIs_to_add)delete obj;

	GameActors.clear();
	GameActors_to_add.clear();
	GameActors_to_delete.clear();
	GameUIs.clear();
	GameUIs_to_add.clear();
	GameUIs_to_delete.clear();
	GameColliders.clear();
	GameColliders_to_clear.clear();
	GameRigids.clear();
	GameRenderers.clear();
	for (auto& obj : OverallRenders)
	{
		GameRenderers.insert(obj);
	}
	for (auto& obj : OverallColliders)
	{
		GameColliders.insert(obj);
		obj->Init();
	}
	for (auto& obj : OverallRigids)
	{
		GameRigids.insert(obj);
	}
	for (auto& arr_i : ColliderZones)for (auto& arr_j : arr_i)arr_j.clear();
	for (auto& arr_i : UIDetectZones)for (auto& arr_j : arr_i)arr_j.clear();
}