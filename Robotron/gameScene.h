//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2005 - 2016 Media Design School
//
// File Name : gameScene.h
// Description : Game scene header file - singleton game scene class that creates the models
// Mail : Mitchell.Currie@mediadesignschool.com
//

#pragma once

#include "camera.h"
#include "entity.h"
#include "clock.h"
#include "TextLabel.h"
#include <vector>

class GameScene
{
public:

	GameScene();
	~GameScene();
	static GameScene& GetInstance();
	void AddCamera(Camera _camera);
	void AddEntity(Entity _entity);
	void AddText(TextLabel _text);
	void RenderEntities(); // For entities
	void RenderText(); // For text
	void CreateEntities();
	void CreateText();
	void SetPositions(float _fDeltaTick);
	void CheckCubeCollisions();
//	void ExecuteOneFrame();
	CClock* GetClock();


protected:
	CClock* m_pClock;

private:

	static GameScene* s_pGameScene;
	Camera m_Camera;
	std::vector<Entity> m_entities;
	//vec3 m_PlayerPosition;
	std::vector<TextLabel> m_textLabels;

};
