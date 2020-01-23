#pragma once
#include "HelperFunctions.h"

int PathCost(const std::vector<KeyPosition4D>& path)
{
	if (path.size() > 0)
	{
		return (path.end() - 1)->steps;// -path[0].steps;
	}
	return 0;

}
struct GreaterPathCost
{
	bool operator()(const std::vector<KeyPosition4D>& lhs, const std::vector<KeyPosition4D>& rhs) const
	{
		return PathCost(lhs) > PathCost(rhs);
	}
};
std::vector<KeyPosition4D> GetAllAvailableKeyPositions(KeyPosition4D start,
	const std::vector<char>& field,
	const int& fieldWidth,
	std::set<std::string>& visitedKeys,
	std::map<std::string, CacheInfo4D>& cache,
	DiagnosticData& diagData)
{
	std::vector<std::vector<std::pair<Key, int>>> newKeys = GetAvailableKeyPositionsForSubFields(start, field, fieldWidth, visitedKeys, cache, diagData);
	static std::map<std::string, std::vector<KeyPosition4D>> cache_internal;
	std::string startHash = Hash(start);
	auto it = cache_internal.find(startHash);
	if (it != cache_internal.end())
	{
		return it->second;
	}
	else
	{
		std::vector<KeyPosition4D> newPositions;
		if (!newKeys.size() == 0)
		{
			for (int i = 0; i < 4; i++) // loop the 4 subFields // for (std::vector<std::pair<Key, int>> pVec : newKeys)
			{
				std::vector<std::pair<Key, int>> pVec = newKeys[i];
				for (std::pair<Key, int> p : pVec)
				{
					KeyPosition4D newKeyPosition = start;
					newKeyPosition.key[i] = p.first;
					newKeyPosition.steps += p.second;
					std::string newkeys = start.keys + p.first.keychar;
					//std::sort(newkeys.begin(), newkeys.end());
					newKeyPosition.keys = newkeys;
					newKeyPosition.path += p.first.keychar;
					newPositions.push_back(newKeyPosition);
				}
			}
		}
		cache_internal.insert({ startHash,newPositions });
		return newPositions;
	}
}
std::vector<KeyPosition4D> FindShortestPath(KeyPosition4D start, KeyPosition4D finish,
	const std::vector<char>& field,
	const int& fieldWidth,
	std::set<std::string>& visitedKeys,
	std::map<std::string, CacheInfo4D>& cache,
	DiagnosticData& diagData)
{
	std::vector<KeyPosition4D> path;
	std::priority_queue< std::vector<KeyPosition4D>, std::vector<std::vector<KeyPosition4D>>, GreaterPathCost> queue;
	std::map<std::string, int> fixed;

	

	while (start.keys.size() != finish.keys.size())
	{
		if (fixed.find(start.keys) == fixed.end())
		{
			fixed.insert({ start.keys, PathCost(path) });
			std::vector<KeyPosition4D> newKeyPositions = GetAllAvailableKeyPositions(start, field, fieldWidth, visitedKeys, cache, diagData);
			for (KeyPosition4D p : newKeyPositions)
			{
				path.push_back(p);
				queue.push(path);
				path.erase(path.end() - 1);
			}
		}
		else
		{
			if (start.steps < fixed.find(start.keys)->second)
			{
				int k = 0;
			}
		}
		if (queue.empty())
		{
			path.clear();
			return path;
		}
		path = queue.top(); queue.pop();
		start = *(path.end() - 1);
	}
	return path;
}


void Day18b()
{
	// Init global data structs===========
	std::vector<char> field;
	int fieldWidth;
	int nKeys;
	std::map<char, int> keyIndices;
	std::map<char, Coi2> keyFieldCoordinates;
	std::vector<std::vector<char>> subFields; // 0 (NW), 1 (NE), 2(SW), 3(SE)
	int subFieldWidth;
	// Load the field & ------------------
	std::vector<Coi2> pos0_vec = { }; // in local (subfield) coordinates
	pos0_vec = LoadField4D("day18binput.txt", field, fieldWidth, nKeys, keyIndices, keyFieldCoordinates, subFields, subFieldWidth);

	// Initialize for Dijkstra ----------------
	std::set<std::string> visitedKeys;
	std::map<std::string, CacheInfo4D> cache;
	DiagnosticData diagData;

	// Apply Dijkstra
	KeyPosition4D P0 = { {{'1',pos0_vec[0]},{'2',pos0_vec[1]},{'3',pos0_vec[2]},{'4',pos0_vec[3]}},"@",0,"@" };
	KeyPosition4D Target = { {},"@abcdefghijklmnopqrstuvwxyz",0,"" };
	std::vector<KeyPosition4D> path = FindShortestPath(P0, Target, field, fieldWidth, visitedKeys, cache, diagData);
	std::cout << (path.end() - 1)->steps;


	return;
}


int main()
{
	Day18b();
	while (!_kbhit());
	return 0;
}


