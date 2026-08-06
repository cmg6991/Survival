///
/// 인덱스 오류가 발생하지만
/// 정상적으로 작동한다.
/// 
/// 다른 작업의 오류 직관성을 위해
/// 컴파일러 오류 무시
///

#include "PathFinding.h"

#include "FindPosition.h"

#include <list>
#include <fstream>

namespace Algorithm
{
	PathFinding::PathFinding()
		: m_nodes(nullptr)
		, m_mapWidth(0)
		, m_mapHeight(0)
		, m_startNode(nullptr)
		, m_endNode(nullptr)
		, m_finalPath{}
		, m_findPotition{new FindPosition()}
		, m_isTherePath(true)
	{

	}
	PathFinding::~PathFinding()
	{
		if (m_nodes) { delete[] m_nodes; }
		if (m_findPotition) { delete m_findPotition; }
	}

	// 파일을 읽어서 맵 만들기
	void PathFinding::ReadSource(std::string path)
	{
		std::wifstream ifs;
		ifs.open(path);

		std::wstring mapName;
		ifs >> mapName;
		std::wstring temp;
		float size;
		ifs >> temp >> temp >> temp >> size;

		int* tempArr = new int[(int)size * (int)size];

		MathEngine::Vector2 center = { 72.f, 108.f };
		MathEngine::Vector2 nowCenter;

		for (int x = 0; x < (int)size; ++x)
		{
			for (int y = 0; y < (int)size; ++y)
			{
				// 파일 읽어오기 
				int index = x * (int)size + y;
				ifs >> tempArr[index];

				// codinatetion
				MathEngine::Vector2 nowCoodinates = {(float)x, (float)y};

				// location
				nowCenter = { center.x + 72.f * (float)y, center.y + 36.f * (float)y};
				MathEngine::Vector2 newLocation = {nowCenter.x, nowCenter.y};

				// 길찾기 데이터에 보관
				m_findPotition->Insert(nowCoodinates, nowCenter);
				
				// 다음 location
			}
			center = { center.x - 72 , center.y + 36 };
		}
		ifs.close();

		CreateMap(tempArr, (int)size, (int)size);

		delete[] tempArr;
	}

	// 수동으로 맵만들기
	void PathFinding::CreateMap(const int* mapArr, const int& sizeOfX, const int& sizeOfY)
	{
		m_mapWidth = sizeOfX;
		m_mapHeight = sizeOfY;
		m_nodes = new Node[m_mapWidth * m_mapHeight];


		//타일 별 노드 초기화
		for (int x = 0; x < m_mapWidth; ++x)
		{
			for (int y = 0; y < m_mapHeight; ++y)
			{
				int index = x * (int)m_mapWidth + y;

				Node& currentNode = m_nodes[index];
				currentNode.x = x;
				currentNode.y = y;
				
				// 장애물 설정
				if (mapArr[index] == 0) { currentNode.obstacle = false; }
				else if (mapArr[index] == 1) { currentNode.obstacle = true;}

				currentNode.visited = false;
				currentNode.globalGoal = FLT_MAX;
				currentNode.localGoal = FLT_MAX;
				currentNode.parent = nullptr;
			}
		}
		
		// 이웃하는 노드 초기화
		for (int x = 0; x < m_mapWidth; ++x)
		{
			for (int y = 0; y < m_mapHeight; ++y)
			{
				int index = x * m_mapWidth + y;

				if (x > 0)
					m_nodes[index].neigbours.push_back(
						&m_nodes[(x - 1) * m_mapWidth + (y - 0)]);

				if (x < m_mapWidth - 1)
					m_nodes[index].neigbours.push_back(
						&m_nodes[(x + 1) * m_mapWidth + (y + 0)]);

				if (y > 0)  // 첫 번째 열을 제외
					m_nodes[index].neigbours.push_back(
						&m_nodes[(x - 0) * m_mapWidth + (y - 1)]);

				if (y < m_mapHeight - 1) // 마지막 열 제외
					m_nodes[index].neigbours.push_back(
						&m_nodes[(x + 0) * m_mapWidth + (y + 1)]);
			}
		}
	}

	void PathFinding::Init()
	{
		// 최종 경로 비우기
		m_finalPath.clear();

		// 모든 노드 설정 초기화
		for (int x = 0; x < m_mapWidth; x++)
		{
			for (int y = 0; y < m_mapHeight; y++)
			{
				Node& currentNode = m_nodes[x * m_mapWidth + y];
				currentNode.visited = false;
				currentNode.globalGoal = FLT_MAX;
				currentNode.localGoal = FLT_MAX;
				currentNode.parent = nullptr;
			}
		}
	}

	void PathFinding::SolveAstar()
	{
		Init();

		if (!m_startNode || !m_endNode) 
		{
			m_isTherePath = false;
			return;
		}

		// 거리재는 람다함수
		auto distance = [](Node* a, Node* b)
		{
			return sqrtf(
				(float)(a->x - b->x) * (float)(a->x - b->x) +
				(float)(a->y - b->y) * (float)(a->y - b->y));
		};

		// 휴리스틱 구하는 람다함수
		auto heuristic = [distance](Node* a, Node* b)
		{
			return distance(a, b);
		};

		// 시작 노드 초기화
		Node* currentNode = m_startNode;
		m_startNode->localGoal = 0.0f;
		m_startNode->globalGoal = heuristic(m_startNode, m_endNode);

		// 탐색되지 않는 리스트
		std::list<Node*> listNotTestedNodes;
		listNotTestedNodes.push_back(currentNode);

		// 검사 시작
		while (!listNotTestedNodes.empty() && // 탐색 리스트가 비어있지 않고
			currentNode != m_endNode) // 검사 중인 노드가 목표노드가 아닐 떄
		{
			// 오름차순 순으로 정렬
			listNotTestedNodes.sort([](const Node* lhs, const Node* rhs){
				return lhs->globalGoal < rhs->globalGoal; });

			// 방문한 노드드 제외
			while (!listNotTestedNodes.empty() && // 탐색 리스트가 비어있지 않고
				listNotTestedNodes.front()->visited) // 리스트 맨 앞 노드가 방문되었다면
			{
				listNotTestedNodes.pop_front(); // 리스트 맨 앞 원소 하나 제거
			}

			// 비였으면 루프 나가기
			if (listNotTestedNodes.empty())
			{
				std::cout << "<! 경로를 찾을 수 없음 !>" << std::endl;
				m_isTherePath = false;

				break;
			}

			// 현재 검사 노드 재설정
			currentNode = listNotTestedNodes.front();
			currentNode->visited = true; // 미리 검사했음으로 설정

			// 이웃 노드들 검사
			for (auto neighborNode : currentNode->neigbours)
			{
				if (!neighborNode->visited && // 방문하지 않는 노드고
					neighborNode->obstacle == false) // 방해물이 아니면
				{
					// 검사 목록에 넣음
					listNotTestedNodes.push_back(neighborNode); 
				}

					// 최단 거리 검사
				float possiblyLowerGoal = currentNode->localGoal +
					distance(currentNode, neighborNode);

				// 최단거리가 기존 거리보다 짧다면
				if (possiblyLowerGoal < neighborNode->localGoal)
				{
					// 이웃 노드의 부모 노드의 부모 노드를 현재노드로
					neighborNode->parent = currentNode;
					// 최단 거리 업데이트
					neighborNode->localGoal = possiblyLowerGoal;
					// 휴리스틱 업뎃: 최단거리 + 목적지까지거리
					neighborNode->globalGoal =
						neighborNode->localGoal + heuristic(neighborNode, m_endNode);
				}
			}
		}

		UpdateFinalPath();
	}

	void PathFinding::UpdateFinalPath()
	{
		Node* currentPathNode = m_endNode;

		while (currentPathNode != nullptr)
		{
			m_finalPath.push_back(currentPathNode);
			currentPathNode = currentPathNode->parent;
		}
	}

	MathEngine::Vector2 PathFinding::NearestLocation() const
	{
		return GetLocation(m_finalPath.size() - 2);
	}

	bool PathFinding::onDestination()
	{
		int range = 1;
		return 
			(m_startNode->x - m_endNode->x <= range) && 
			(m_startNode->x - m_endNode->x >= -range) &&
			(m_startNode->y - m_endNode->y <= range) && 
			(m_startNode->y - m_endNode->y >= -range);
	}

	void PathFinding::Show()
	{		
		for (int x = 0; x < m_mapWidth; ++x)
		{
			for (int y = 0; y < m_mapHeight; ++y)
			{
				Node* currentNode = &m_nodes[x * m_mapWidth + y];

				if (currentNode == m_startNode)
					std::cout << "S "; // 시작 노드
				else if (currentNode == m_endNode)
					std::cout << "E "; // 목표 노드
				else if (currentNode->obstacle)
					std::cout << "# "; // 장애물
				else if (std::find(m_finalPath.begin(), m_finalPath.end(), currentNode) != m_finalPath.end())
					std::cout << "* "; // 최종 경로에 포함되는 노드
				else if (currentNode->visited)
					std::cout << "_ "; // 방문한 노드
				else
					std::cout << ". "; // 빈 공간
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	void PathFinding::SetStartNode(const int& x, const int& y)
	{
		int index = x * m_mapWidth + y;
		//if (m_nodes[index].obstacle == false)
		//{
		//	m_startNode = &m_nodes[index];
		//}
		//else
		//{
		//	std::cout << "해당 위치에 장애물이 있음" << std::endl;
		//	m_isTherePath = false;
		//}
		m_startNode = &m_nodes[index];
	}

	void PathFinding::SetStartNode(const MathEngine::Vector2& startPos)
	{
		std::pair<MathEngine::Vector2, MathEngine::Vector2> temp = m_findPotition->FindNearest(startPos);
		SetStartNode((int)temp.first.x, (int)temp.first.y);
	}

	void PathFinding::SetEndNode(const int& x, const int& y)
	{
		int index = x * m_mapWidth + y;
		//if (m_nodes[index].obstacle == false)
		//{
		//	m_endNode = &m_nodes[index];
		//}
		//else
		//{
		//	std::cout << "해당 위치에 장애물이 있음" << std::endl;
		//	m_isTherePath = false;
		//}
		m_endNode = &m_nodes[index];
	}

	void PathFinding::SetEndNode(const MathEngine::Vector2& endPos)
	{
		std::pair<MathEngine::Vector2, MathEngine::Vector2> temp = m_findPotition->FindNearest(endPos);
		SetEndNode((int)temp.first.x, (int)temp.first.y);
	}

	void PathFinding::SetObstacle(const int& x, const int& y)
	{
		int index = y * m_mapWidth + x;
		m_nodes[index].obstacle = true;
	}

	void PathFinding::RemoveObstacle(const int& x, const int& y)
	{
		int index = y * m_mapWidth + x;
		m_nodes[index].obstacle = false;
	}

	int PathFinding::GetFinalPathNodeSize() const
	{
		return m_finalPath.size();
	}

	MathEngine::Vector2 PathFinding::GetLocation(const int& indx) const
	{
		Node* backNode = m_finalPath[indx];
		MathEngine::Vector2 key = { (float)backNode->x, (float)backNode->y };
		MathEngine::Vector2 val = m_findPotition->m_mapData[key];

		return val;
	}

}