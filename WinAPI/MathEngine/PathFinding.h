///
/// 길찾기 알고리즘
/// 참고: https://www.youtube.com/watch?v=icZj67PTFhc
/// 

#pragma once

#include "Vector2.h"

#include <vector>
#include <iostream>

namespace MathEngine { struct Vector2; }

namespace Algorithm 
{
	class FindPosition;

	class PathFinding
	{
	public:
		struct Node
		{
			int x = 0;
			int y = 0;
			bool obstacle = false;
			bool visited = false;
			float globalGoal = FLT_MAX;
			float localGoal = FLT_MAX;
			Node* parent = nullptr;
			std::vector<Node*> neigbours{};
		};

		PathFinding();
		~PathFinding();

		// 파일 읽어오기 
		void ReadSource(std::string path);

		// 시작 위치
		void SetStartNode(const MathEngine::Vector2& startPos);
		void SetStartNode(const int& x, const int& y);

		// 목표 위치
		void SetEndNode(const MathEngine::Vector2& endPos);
		void SetEndNode(const int& x, const int& y);

		
		// 장애물 설정
		void SetObstacle(const int& x, const int& y);
		void RemoveObstacle(const int& x, const int& y);

		// 가야할 경로 가져오기
		int GetFinalPathNodeSize() const;
		MathEngine::Vector2 GetLocation(const int& indx) const;
		MathEngine::Vector2 NearestLocation() const;

		// 시작 위치와 목표 위치가 같은 지 검사
		bool onDestination();

		bool GetIsTherePath() const { return m_isTherePath; }

		// 디버깅용
		void Show();

		// temp
		FindPosition* m_findPotition;
		std::vector<Node*> m_finalPath;

		void SolveAstar();

	private:
		Node* m_nodes; // 노드 배열
		int m_mapWidth;
		int m_mapHeight;

		Node* m_startNode;
		Node* m_endNode;

		bool m_isTherePath;
		void CreateMap(const int* mapArr, const int& sizeOfX, const int& sizeOfY);

		void Init();
		void UpdateFinalPath();

		// 좌표 변환 알고리즘
		//FindPosition* m_findPotition;

		// 최종 경로
	};
}