
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <unordered_map>

//using fspath = std::filesystem:path;
using namespace std;

struct PathNode {
public:
	string dir_name;
	vector<PathNode> *children;

	PathNode(string dir_name, vector<PathNode> *children) {
		this->dir_name = dir_name;
		this->children = children;
	}

	friend bool operator==(const PathNode& lhs, const PathNode& rhs);
	friend bool operator==(const PathNode& lhs, const string& rhs);
};
bool operator==(const PathNode& lhs, const PathNode& rhs) { return !(lhs.dir_name.compare(rhs.dir_name)); };
bool operator==(const PathNode& lhs, const string& rhs) { return !(lhs.dir_name.compare(rhs)); };

vector<string> split(const std::string& s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> elems;
	while (getline(ss, item, delim)) {
		elems.push_back(move(item));
	}
	return elems;
}

// In its current form, this is mostly for generation of tree from file paths, and should NOT be used for getting directory paths in tree
PathNode findOrCreatePathInTree(vector<string> path_tokens, int cTokenIndex, PathNode dir) {
	vector<PathNode>::iterator it = find(dir.children->begin(), dir.children->end(), path_tokens[cTokenIndex]);
	PathNode* nextDir;
	// Effeciency optimization: if current token is a file (not a directory), assume it doesn't exist in the dir (don't bother searching)
	// If the current token is not a file (leaf node):
	if (cTokenIndex < path_tokens.size() - 1) {
		// If existing dir vector contains current path token...
		if (it != dir.children->end()) {
			nextDir = &(*it);
		}
		else {
			// add new dir vector to its parent dir vector
			nextDir = new PathNode(move(path_tokens[cTokenIndex]), new vector<PathNode>());
			dir.children->push_back(*nextDir);
		}
		// If this is NOT the last path (directory) token, then recurse
		nextDir = &findOrCreatePathInTree(path_tokens, cTokenIndex + 1, *nextDir);
	}
	else {
		// Don't create vector for file nodes
		nextDir = new PathNode(move(path_tokens[cTokenIndex]), nullptr);
		dir.children->push_back(*nextDir);
	}
	
	return *nextDir;
}

// Construct tree of paths
extern PathNode genDirTree(vector<string> paths) {
	auto start = chrono::steady_clock::now();
	// test unordered_map and other optimizations LATER
	PathNode rootDir = PathNode("", new vector<PathNode>());//tr1::unordered_map<string, tr1::unordered_map> rootDir();
	for (string path : paths) {
		vector<string> path_tokens = split(path, '/');
		PathNode cdir = findOrCreatePathInTree(path_tokens, 0, rootDir);
	}
	auto diff = chrono::steady_clock::now() - start;
	fprintf(stdout, "Tree build time: %f ms", chrono::duration<double, milli>(diff).count());
	return rootDir;
}

extern int printTree(PathNode cnode, int cdepth, int max_depth, bool showFiles) {
	
	if (cdepth <= max_depth) {
		string indentString = "";
		for (int i = 1; i < cdepth ; i++) {// i == 1 because root node (cdepth == 0) name is empty str.
			indentString += "\t";
		}
		fprintf(stdout, "%s%s\n", indentString.c_str(), cnode.dir_name.c_str());
		
		// Don't try to iterate through the subdirectories of files (can't exist)
		if (cnode.children != nullptr) {
			for (PathNode node : *cnode.children) {
				// Call print function if node is a directory, or if it is a file and showFiles is enabled
				if (node.children != nullptr || showFiles) {
					printTree(node, cdepth + 1, max_depth, showFiles);
				}
			}
		}
	}

	return 0;
}
