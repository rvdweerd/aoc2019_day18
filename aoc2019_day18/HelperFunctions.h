#pragma once
#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <conio.h>
#include <iomanip>
#include <utility>
#include <queue>

struct mazeHistory
{
	int x = 0;
	int y = 0;
	int recDepth = 0;
};
struct Coi2
{
	int x;
	int y;
	int steps = 0;
	std::vector<mazeHistory> history;
	int recursiveDepthLevel = 0;
	friend bool operator==(const Coi2& p1, const Coi2& p2)
	{
		return (p1.x == p2.x) && (p1.y == p2.y);
	}
	friend bool operator<(const Coi2& p1, const Coi2& p2)
	{
		return (p1.x + p1.y) < (p2.x + p2.y);
	}
};
static bool route_compare(std::pair<std::string, int> a, std::pair<std::string, int> b)
{
	return (a.second < b.second);
}
struct Key
{
	char keychar;
	Coi2 coordinates;
};
struct KeyPosition
{
	Key key;
	std::string keys;
	int steps = 0;
	std::string path;
};
struct KeyPosition4D
{
	std::vector<Key> key = { {}, {}, {}, {} };
	std::string keys;
	int steps = 0;
	std::string path;
};
struct CacheInfo
{
	std::vector<std::pair<Key, int>> cachedAvailablePositions;
	int cumulativeSteps;
};
struct CacheInfo4D
{
	std::vector<std::vector<std::pair<Key, int>>> cachedAvailablePositions;
	int cumulativeSteps;
};


struct DiagnosticData
{
	int loopCount_OuterBFS = 0;
	int loopCount_InnerBFS = 0;
	int MaxQueueSize_OuterBFS = 0;
	int MaxQueueSize_InnerBFS = 0;
};
struct Vei2
{
	std::vector<int> coord = { 0,0 };
	std::string keys;
	int steps = 0;
};
bool IsKey(char c)
{
	return ((c > 96 && c <= 122) || (c == '@'));
}
bool IsDoor(char c)
{
	return (c > 64 && c <= 90);
}
std::string Hash(const KeyPosition& p) // used to enable a hash lookup of visited nodes in the BFS
{
	std::string keys = p.keys;
	std::sort(keys.begin(), keys.end());
	std::string Hash = "";
	Hash += p.key.keychar;
	Hash.append(keys);
	return Hash;
}
std::string Hash(const KeyPosition4D& p) // used to enable a hash lookup of visited nodes in the BFS
{
	std::string keys = p.keys;
	std::sort(keys.begin(), keys.end());
	std::string Hash = "";
	Hash += p.key[0].keychar;
	Hash += p.key[1].keychar;
	Hash += p.key[2].keychar;
	Hash += p.key[3].keychar;
	Hash.append(keys);
	return Hash;
}
std::string Hash(const Vei2& p0)
{
	return std::to_string(p0.coord[0]) + "_" + std::to_string(p0.coord[1]);
}
void PrintField(std::vector<char> field, const int& fieldWidth)
{
	//std::cout << '\n';
	for (int i = 0; i < (int)field.size(); i++)
	{
		if (i % fieldWidth == 0) std::cout << '\n';
		/*if ((i > 2 * fieldWidth && (i < (int)field.size() - 2 * fieldWidth)) && //away from top/bottom
			(field[i - fieldWidth] == '.' || field[i - fieldWidth] > 65) &&
			(field[i + fieldWidth] == '.' || field[i + fieldWidth] > 65) && // above and below is corridor
			(field[i] == '#')) // cell is barrier
		{
			std::cout << '-';
		}
		//else if (field[max(0,i - 1)] == '.' && field[i + 1] == '.' && field[i] == '#')
	//	{
	//		std::cout << '|';
	//	}
		else if (field[i] == '#')
		{
			std::cout << '+';
		}
		else if (field[i] == '.')
		{
			std::cout << ' ';
		}
		else*/
		{
			std::cout << field[i];
		}
	}
}

Coi2 LoadField(const std::string filename, std::vector<char>& field, int& fieldWidth, int& nKeys, std::map<char, int>& keyIndices, std::map<char, Coi2>& keyPositions)
{
	fieldWidth = 1; // we don't know this yet, this is the starting assumption
	nKeys = 0;
	bool fieldWidthSet = false;
	std::ifstream in(filename);
	Coi2 pos0;
	while (!in.eof())
	{
		char ch;
		std::string str;
		int i = 0;
		for (ch = in.get(); !in.eof(); ch = in.get())
		{
			if (ch == '\n')
			{
				if (!fieldWidthSet)
				{
					fieldWidth = i;
					fieldWidthSet = true;
				}
			}
			else
			{
				if (IsKey(ch))
				{
					keyIndices[ch] = i;
					int keyY = i / fieldWidth;
					int keyX = i - keyY * fieldWidth;
					keyPositions[ch] = { keyX , keyY };
					nKeys++;
				}
				if (ch == '@')
				{
					{
						pos0.y = i / fieldWidth;
						pos0.x = i - pos0.y * fieldWidth;
					}
				}
				field.push_back(ch);
				i++;
			}
		}
	}
	return pos0;
}
std::vector<Coi2> LoadField4D(
	const std::string filename,
	std::vector<char>& field,
	int& fieldWidth,
	int& nKeys,
	std::map<char, int>& keyIndices,
	std::map<char, Coi2>& keyPositions,
	std::vector<std::vector<char>>& subFields,
	int& subFieldWidth)
{
	fieldWidth = 1; // we don't know this yet, this is the starting assumption
	nKeys = 0;
	bool fieldWidthSet = false;
	std::ifstream in(filename);
	while (!in.eof())
	{
		char ch;
		std::string str;
		int i = 0;
		for (ch = in.get(); !in.eof(); ch = in.get())
		{
			if (ch == '\n')
			{
				if (!fieldWidthSet)
				{
					fieldWidth = i;
					fieldWidthSet = true;
				}
			}
			else
			{
				if (IsKey(ch))
				{
					keyIndices[ch] = i;
					int keyY = i / fieldWidth;
					int keyX = i - keyY * fieldWidth;
					keyPositions[ch] = { keyX , keyY };
					nKeys++;
				}
				/*if (ch == '@')
				{
					{
						pos0.y = i / fieldWidth;
						pos0.x = i - pos0.y * fieldWidth;
					}
				}*/
				field.push_back(ch);
				i++;
			}
		}
		nKeys -= 3;
	}
	int fieldHeight = field.size() / fieldWidth;
	subFields = { {},{},{},{} };
	subFieldWidth = fieldWidth / 2 + 1;
	int subFieldHeight = fieldHeight / 2 + 1;
	int subSize = subFieldWidth * subFieldHeight;
	std::vector<Coi2> pos0_vec = {  };
	Coi2 p0;
	for (int i = 0; i < subSize; i++) // SubField 0 (NW), p0 in local (subfield) coordinates
	{
		int y = i / subFieldWidth;
		int x = i % subFieldWidth;
		char ch = field[y * fieldWidth + x];
		subFields[0].push_back(ch);
		if (ch == '@')
		{
			//p0 = { i - (i / subFieldWidth) * subFieldWidth, i / subFieldWidth };
			p0 = { x,y };
			pos0_vec.push_back(p0);
		}
	}
	int offsetX = subFieldWidth - 1;
	int offsetY = subFieldHeight - 1;
	for (int i = 0; i < subSize; i++) // SubField 1 (NE), p0 in local (subfield) coordinates
	{
		int y = i / subFieldWidth;
		int x = i % subFieldWidth + offsetX;
		char ch = field[y * fieldWidth + x];
		subFields[1].push_back(ch);
		if (ch == '@')
		{
			//p0 = { i - (i / subFieldWidth) * subFieldWidth, i / subFieldWidth };
			p0 = { x,y };
			pos0_vec.push_back(p0);
		}
	}
	for (int i = 0; i < subSize; i++) // SubField 3 (SW), p0 in local (subfield) coordinates
	{
		int y = (i / subFieldWidth) + offsetY;
		int x = i % subFieldWidth;
		char ch = field[y * fieldWidth + x];
		subFields[2].push_back(ch);
		if (ch == '@')
		{
			//p0 = { i - (i / subFieldWidth) * subFieldWidth, i / subFieldWidth };
			p0 = { x,y };
			pos0_vec.push_back(p0);
		}
	}
	for (int i = 0; i < subSize; i++) // SubField 4 (SE), p0 in local (subfield) coordinates
	{
		int y = (i / subFieldWidth) + offsetY;
		int x = i % subFieldWidth + offsetX;
		char ch = field[y * fieldWidth + x];
		subFields[3].push_back(ch);
		if (ch == '@')
		{
			//p0 = { i - (i / subFieldWidth) * subFieldWidth, i / subFieldWidth };
			p0 = { x,y };
			pos0_vec.push_back(p0);
		}
	}
	PrintField(field, fieldWidth); std::cout << '\n';
	PrintField(subFields[0], subFieldWidth); std::cout << '\n';
	PrintField(subFields[1], subFieldWidth); std::cout << '\n';
	PrintField(subFields[2], subFieldWidth); std::cout << '\n';
	PrintField(subFields[3], subFieldWidth); std::cout << '\n';
	return pos0_vec;
}

std::vector<Vei2> GetNewCoordinates(Vei2& curPos, const std::vector<char>& field, const int& fieldWidth, std::set<std::string>& visited)
{
	std::vector<Vei2> NewCoordinates;
	for (unsigned dir = 1; dir < 5; dir++)
	{
		Vei2 newPos = curPos;
		if (dir == 1) newPos.coord[1]--;		// NORTH
		else if (dir == 2) newPos.coord[1]++;	// SOUTH
		else if (dir == 3) newPos.coord[0]--;	// WEST
		else if (dir == 4) newPos.coord[0]++;	// EAST

		char ch = field[newPos.coord[1] * fieldWidth + newPos.coord[0]];
		if (ch != '#')
		{
			if (visited.find(Hash(newPos)) == visited.end()) // New position was not previsously visited in current state
			{
				NewCoordinates.push_back(newPos);
			}
		}
	}
	return NewCoordinates;
}

std::vector<std::pair<Key, int>> GetAvailableKeyPositions(
	KeyPosition& curKeyPos,
	const std::vector<char>& field,
	const int& fieldWidth,
	std::set<std::string>& visitedKeys,
	std::map<std::string, CacheInfo4D>& cache,
	DiagnosticData& diagData)
{
	std::vector<std::pair<Key, int>> availableKeyOptions;

	//auto cache_it = cache.find(Hash(curKeyPos));
	//if (cache_it == cache.end())
	{

		Vei2 p0 = { {curKeyPos.key.coordinates.x,curKeyPos.key.coordinates.y},curKeyPos.keys,curKeyPos.steps };
		std::set<std::string> visited;
		visited.insert(Hash(p0));
		std::queue<Vei2> queue;
		queue.push(p0);

		while (!queue.empty())
		{
			if (queue.size() > (size_t)diagData.MaxQueueSize_InnerBFS) diagData.MaxQueueSize_InnerBFS = (int)queue.size();
			diagData.loopCount_InnerBFS++;
			Vei2 curPos = queue.front(); queue.pop();
			std::vector<Vei2> newPositions = GetNewCoordinates(curPos, field, fieldWidth, visited);
			for (Vei2 newPos : newPositions)
			{
				// New status allocation
				newPos.steps++;
				char ch = field[newPos.coord[1] * fieldWidth + newPos.coord[0]];

				if (IsKey(ch) && (newPos.keys.find(ch) == std::string::npos)) // If newpos is a new key => add it to newKeyPositions, don't push to queue
				{
					Key newkey = { ch,{newPos.coord[0],newPos.coord[1]} };
					availableKeyOptions.push_back({ newkey,newPos.steps - curKeyPos.steps });
				}
				else if (IsDoor(ch) && (newPos.keys.find(std::tolower(ch)) == std::string::npos)) // If Door is found, you don't have the key => don't push to queue
				{
				}
				else // continue with new position
				{
					visited.insert(Hash(newPos));
					queue.push(newPos);
				}
			}
		}
		// Update cache
		//cache[Hash(curKeyPos)] = { availableKeyOptions, curKeyPos.steps };
	}
	//else
	//{
	//	if (cache_it->second.cumulativeSteps <= curKeyPos.steps)
	//	{
	//		return {};
	//	}
	//	else
	//	{
	//		cache_it->second.cumulativeSteps = curKeyPos.steps;
	//		availableKeyOptions = cache_it->second.cachedAvailablePositions;
	//		//std::cout << "cache used.";
	//	}
	//}
	return availableKeyOptions;
}

std::vector<std::vector<std::pair<Key, int>>> GetAvailableKeyPositionsForSubFields(
	KeyPosition4D& curKeyPos4D,
	const std::vector<char>& field,
	const int& fieldWidth,
	std::set<std::string>& visitedKeys,
	std::map<std::string, CacheInfo4D>& cache,
	DiagnosticData& diagData)
{
	std::vector<std::vector<std::pair<Key, int>>> newKeys;
	auto cache_it = cache.find(Hash(curKeyPos4D));
	if (cache_it == cache.end())
	{
		for (int i = 0; i < 4; i++) // loop the 4 subfields
		{
			KeyPosition curKeyPos = { curKeyPos4D.key[i], curKeyPos4D.keys, curKeyPos4D.steps, curKeyPos4D.path };
			std::vector<std::pair<Key, int>> newVec = GetAvailableKeyPositions(curKeyPos, field, fieldWidth, visitedKeys, cache, diagData);
			newKeys.push_back(newVec);
		}
		cache[Hash(curKeyPos4D)] = { newKeys, curKeyPos4D.steps };
	}
	else
	{
		if (cache_it->second.cumulativeSteps <= curKeyPos4D.steps)
		{
			return {};
			//newKeys = cache_it->second.cachedAvailablePositions;
		}
		else
		{
			cache_it->second.cumulativeSteps = curKeyPos4D.steps;
			newKeys = cache_it->second.cachedAvailablePositions;
			//std::cout << "cache used.";
		}
	}
	return newKeys;
}