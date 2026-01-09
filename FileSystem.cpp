#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

class File {
private:
    string content;
public:
    void addContent(string c) {
        content += c;
    }
    string getContent() {
        return content;
    }
};

class Directory {
public:
    // Using map keeps keys sorted automatically!
    map<string, Directory*> dirs;
    map<string, File*> files;
};

class FileSystem {
private:
    Directory* root;

    vector<string> split(string path) {
        vector<string> res;
        stringstream ss(path);
        string token;
        while (getline(ss, token, '/')) {
            if (!token.empty()) res.push_back(token);
        }
        return res;
    }

public:
    FileSystem() {
        root = new Directory();
    }

    void mkdir(string path) {
        vector<string> parts = split(path);
        Directory* curr = root;

        for (string& part : parts) {
            if (curr->dirs.find(part) == curr->dirs.end()) {
                curr->dirs[part] = new Directory();
            }
            curr = curr->dirs[part];
        }
    }
    
    vector<string> ls(string path) {
        vector<string> result;
        vector<string> parts = split(path);
        Directory* curr = root;

        // Traverse down the path
        for (string& part : parts) {
            if (curr->files.count(part)) {
                return {part};
            }
            
            if (curr->dirs.find(part) == curr->dirs.end()) {
                return {};
            }
            curr = curr->dirs[part];
        }

        // Collect Directory Names
        for (auto& pair : curr->dirs) {
            result.push_back(pair.first);
        }

        // Collect File Names
        for (auto& pair : curr->files) {
            result.push_back(pair.first);
        }

        // Final sort to mix files and directories A-Z
        sort(result.begin(), result.end());

        return result;
    }

    void addContentToFile(string path, string content) {
        vector<string> parts = split(path);
        Directory* curr = root;
        int n = parts.size();

        // Traverse to parent directory
        for (int i = 0; i < n - 1; i++) {
            string part = parts[i];
            if (curr->dirs.find(part) == curr->dirs.end()) {
                curr->dirs[part] = new Directory();
            }
            curr = curr->dirs[part];
        }

        // Handle the file
        string fileName = parts[n - 1];
        if (curr->files.find(fileName) == curr->files.end()) {
            curr->files[fileName] = new File();
        }
        curr->files[fileName]->addContent(content);
    }

    string readContentFromFile(string path) {
        vector<string> parts = split(path);
        Directory* curr = root;
        int n = parts.size();

        // Traverse to parent directory
        for (int i = 0; i < n - 1; i++) {
            string part = parts[i];
            if (curr->dirs.find(part) == curr->dirs.end()) {
                return ""; // Directory doesn't exist
            }
            curr = curr->dirs[part];
        }

        string fileName = parts[n - 1];
        if (curr->files.find(fileName) != curr->files.end()) {
            return curr->files[fileName]->getContent();
        }
        
        return "";
    }
};

int main() {
    FileSystem fs;

    // 1. Create Directories
    fs.mkdir("/a/b/c");
    
    // 2. Add Content
    fs.addContentToFile("/a/b/c/d", "Hello");
    
    // 3. Read Content
    cout << "Content: " << fs.readContentFromFile("/a/b/c/d") << endl; // Output: Hello
    
    // 4. List Directory
    vector<string> list = fs.ls("/a/b/c");
    cout << "LS Result: ";
    for (string s : list) cout << s << " "; // Output: d
    cout << endl;

    return 0;
}