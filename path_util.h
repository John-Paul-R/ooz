#pragma once

#include <string>
#include <vector>

struct PathNode {
	std::string dir_name;
	std::vector<PathNode>* children;
};

PathNode genDirTree(std::vector<std::string> paths);
int printTree(PathNode cnode, int cdepth, int max_depth, bool showFiles);