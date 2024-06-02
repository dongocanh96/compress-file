#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char character, int frequency) : ch(character), freq(frequency), left(nullptr), right(nullptr) {}
};

struct compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

Node* buildHuffmanTree(unordered_map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, compare> pq;

    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        Node* newNode = new Node('\0', sum);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

void generateHuffmanCodes(Node* root, string str, unordered_map<char, string>& huffmanCodes) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = str;
    }

    generateHuffmanCodes(root->left, str + "0", huffmanCodes);
    generateHuffmanCodes(root->right, str + "1", huffmanCodes);
}

void compressFile(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Could not open input file: " << inputFileName << endl;
        return;
    }

    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Could not open output file: " << outputFileName << endl;
        return;
    }

    unordered_map<char, int> freqMap;
    char ch;

    while (inputFile.get(ch)) {
        freqMap[ch]++;
    }

    Node* root = buildHuffmanTree(freqMap);

    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    inputFile.clear();
    inputFile.seekg(0);

    string compressedData = "";
    while (inputFile.get(ch)) {
        compressedData += huffmanCodes[ch];
    }

    outputFile << compressedData;
    
    inputFile.close();
    outputFile.close();
}

void compressDirectory(const string& directoryPath, const string& outputDir) {
    for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
        if (fs::is_regular_file(entry.path())) {
            string outputFileName = outputDir + "/" + entry.path().filename().string() + ".huff";
            compressFile(entry.path().string(), outputFileName);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input file/folder> <output folder>" << endl;
        return 1;
    }

    string inputPath = argv[1];
    string outputDir = argv[2];

    if (!fs::exists(inputPath)) {
        cerr << "Input path does not exist: " << inputPath << endl;
        return 1;
    }

    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    if (fs::is_directory(inputPath)) {
        compressDirectory(inputPath, outputDir);
    } else if (fs::is_regular_file(inputPath)) {
        string outputFileName = outputDir + "/" + fs::path(inputPath).filename().string() + ".huff";
        compressFile(inputPath, outputFileName);
    } else {
        cerr << "Invalid input path: " << inputPath << endl;
        return 1;
    }

    cout << "Compression completed successfully." << endl;
    return 0;
}
