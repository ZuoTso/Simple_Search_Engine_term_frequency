#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <set>
#include <cmath>
#include <unordered_set>

// comparison function
/*
bool compareDocumentID(const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) {
	return a.first < b.first;
}

// Sort corpus in ascending order by document ID
void sortCorpus(std::vector<std::pair<int, std::string>>& corpus) {
	std::sort(corpus.begin(), corpus.end(), compareDocumentID);
}*/

// Remove punctuation and convert to lowercase
std::string preprocessString(const std::string& str) {
	std::string processedStr;
	for (char c : str) {
		// || c == ' '
		if (std::isalpha(c) || c == ' ') {
			processedStr += std::tolower(c);
		}
	}
	return processedStr;
}

// Split strings with spaces	
std::vector<std::string> splitString(const std::string& str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (iss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

// Build a word search engine
/*std::unordered_map<std::string, std::set<int>> buildIndex(const std::vector<std::pair<int, std::string>>& corpus) {
	std::unordered_map<std::string, std::set<int>> index;
	for (const auto& item : corpus) {
		std::vector<std::string> words = splitString(preprocessString(item.second));
		for (const auto& word : words) {
			index[word].insert(item.first);
		}
	}
	return index;
}*/

// Calculate Sum IDF
/*double calculateSumIDF(const std::string& text, const std::unordered_map<std::string, double>& idfMap) {
	double sumIDF = 0.0;
	std::vector<std::string> words = splitString(preprocessString(text));
	for (const std::string& word : words) {
		auto it = idfMap.find(word);
		if (it != idfMap.end()) {
			sumIDF += it->second;
		}
		else if (!sumIDF)
			return -1;
	}
	return sumIDF;
}
*/

// Build a string search engine
/*std::unordered_map<std::string, std::set<int>> buildIndex(const std::vector<std::pair<int, std::string>>& corpus) {
	std::unordered_map<std::string, std::set<int>> index;
	for (const auto& item : corpus) {
		std::string lowercaseStr = preprocessString(item.second);
		index[lowercaseStr].insert(item.first);
	}
	return index;
}*/

// execute query - intersection
// Pass in the unordered parent data, split the data to be searched into keywords, and find "completely included" by intersection
/*std::set<int> executeQuery(const std::vector<std::string>& keywords, const std::unordered_map<std::string, std::set<int>>& index) {
	std::set<int> result;
	for (const auto& keyword : keywords) {
		auto it = index.find(keyword);
		if (it == index.end()) {			//if not find
			return std::set<int>{-1};
		}
		if (result.empty()) {				//found for the first time
			result = it->second;
		}
		else {
			std::set<int> intersection;
			std::set_intersection(result.begin(), result.end(), it->second.begin(), it->second.end(), std::inserter(intersection, intersection.begin()));
			if (intersection.empty())
				return std::set<int>{-1};	//if empty collection
			else
				result = intersection;
		}
	}
	return result;
}*/

// exrcute query - find the top-k strings (shown its S_ID) 
std::vector<int> findTopThreeSumIDF(const std::string& line, const std::vector<std::pair<int, std::string>>& corpus, const std::unordered_map<std::string, double>& idfMap) {
	std::vector<int> topThreeIDFs;
	std::vector<std::string> keywords = splitString(preprocessString(line));
	std::vector<std::pair<int, double>> sumIDFList;

	for (const auto& item : corpus) {
		int documentID = item.first;
		const std::string& documentText = item.second;
		std::vector<std::string> documentKeywords = splitString(preprocessString(documentText));

		double sumIDF = 0.0;
		for (const std::string& keyword : keywords) {
			auto it = idfMap.find(keyword);
			if (it != idfMap.end()) {
				if (std::find(documentKeywords.begin(), documentKeywords.end(), keyword) != documentKeywords.end()) {
					sumIDF += it->second;
				}
			}
		}
		if (sumIDF)
			sumIDFList.emplace_back(documentID, sumIDF);
		else
			sumIDFList.emplace_back(-1, sumIDF);
	}

	// Sort sumIDFList in descending order of sum_idf values
	std::sort(sumIDFList.begin(), sumIDFList.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
		if (a.second == b.second) {
			return a.first < b.first;
		}
		return a.second > b.second;
		});

	// Retrieve top three document IDs with highest sum_idf values
	for (int i = 0; i < std::min(3, static_cast<int>(sumIDFList.size())); ++i) {
		topThreeIDFs.push_back(sumIDFList[i].first);
	}

	// If there are less than three document IDs, fill the remaining slots with -1
	while (topThreeIDFs.size() < 3) {
		topThreeIDFs.push_back(-1);
	}

	return topThreeIDFs;
}



// Calculate IDF
// Calculate IDF for each term in the corpus
std::unordered_map<std::string, double> calculateIDF(const std::vector<std::pair<int, std::string>>& corpus) {
	std::unordered_map<std::string, double> idfMap;
	const int totalDocuments = corpus.size();

	// Count the document frequency (DF) for each term
	std::unordered_map<std::string, int> dfMap;
	for (const auto& document : corpus) {
		std::unordered_set<std::string> uniqueTerms;
		std::vector<std::string> terms = splitString(preprocessString(document.second));
		for (const auto& term : terms) {
			uniqueTerms.insert(term);
		}
		for (const auto& term : uniqueTerms) {
			dfMap[term]++;
		}
	}
	/*for (auto& it : dfMap) {
		std::cout << it.second << " ";
		for (const auto& str : it.first) {
			std::cout << str;
		}
		std::cout << std::endl;
	}*/

	// Calculate IDF for each term
	for (const auto& entry : dfMap) {
		std::string term = entry.first;
		int documentFrequency = entry.second;
		double idf = std::log10(static_cast<double>(totalDocuments) / (documentFrequency));
		idfMap[term] = idf;
	}

	return idfMap;
}

// find normalize term frequency map
std::unordered_map<int, std::unordered_map<std::string, double>> calculateNormalizeTermFrequency(const std::vector<std::pair<int, std::string>>& corpus) {
	std::unordered_map<int, std::unordered_map<std::string, double>> termFrequencyMap;

	for (const auto& document : corpus) {
		int documentID = document.first;
		const std::string& documentText = document.second;
		std::unordered_map<std::string, double> documentTermFrequency;

		std::vector<std::string> words = splitString(preprocessString(documentText));
		int wordCount = words.size();

		// Count the frequency of each word in the document
		for (const auto& word : words) {
			documentTermFrequency[word]++;
		}

		// Calculate term frequency by dividing the frequency by the document length
		for (auto& entry : documentTermFrequency) {
			entry.second /= wordCount;
		}

		termFrequencyMap[documentID] = std::move(documentTermFrequency);
	}

	return termFrequencyMap;
}

// find Top-K query rank
std::vector<std::pair<int, double>> findTopKRanks(const std::string& line, const std::unordered_map<std::string, double>& idfMap, const std::unordered_map<int, std::unordered_map<std::string, double>>& termFrequencyMap, int k) {
	std::vector<std::pair<int, double>> topKRanks;
	std::vector<std::string> keywords = splitString(preprocessString(line));

	for (const auto& entry : termFrequencyMap) {
		int documentID = entry.first;
		const auto& documentTermFrequency = entry.second;

		double rank = 0.0;
		for (const std::string& keyword : keywords) {
			auto itIDF = idfMap.find(keyword);
			if (itIDF != idfMap.end()) {
				auto itTermFrequency = documentTermFrequency.find(keyword);
				if (itTermFrequency != documentTermFrequency.end()) {
					rank += itIDF->second * itTermFrequency->second;
				}
			}
		}

		topKRanks.emplace_back(documentID, rank);
	}

	std::partial_sort(topKRanks.begin(), topKRanks.begin() + k, topKRanks.end(),
		[](const std::pair<int, double>& a, const std::pair<int, double>& b) {
			if (a.second != b.second) {
				return a.second > b.second;
			}
			return a.first < b.first;
		});


	topKRanks.resize(k);

	return topKRanks;
}



int main(int argc, char* argv[]) {

	// command argument input file
	if (argc < 4) {
		std::cout << "Usage: ./hw5 <corpus_file> <query_file> top-k\n";
		return 1;
	}

	int top_k = 3;
	//int top_k = argv[3][0] - '0';
	// open file
	//std::ifstream corpusFile(argv[1]); 
	std::ifstream corpusFile("C:\\Users\\chius\\OneDrive\\桌面\\Simple_Search_Engine_3\\corpus1.txt");
	if (!corpusFile) {
		std::cout << "Failed to open corpus file.\n";
		return 1;
	}

	// Read the file and make initial settings
	std::vector<std::pair<int, std::string>> corpus;
	std::string line;
	while (std::getline(corpusFile, line)) {
		std::istringstream iss(line);
		int id;
		std::string content;
		if (iss >> id && std::getline(iss, content)) {
			// Remove the double quotes, find the first "next position and set it to 0, and find the last position that is not "+1, because of the substr feature
			content = content.substr(content.find_first_of("\"") + 1);
			content = content.substr(0, content.find_last_not_of("\"") + 1);
			corpus.emplace_back(id, content);
		}
	}
	corpusFile.close();

	//sortCorpus(corpus);

	// Build search engine
	//std::unordered_map<std::string, std::set<int>> index = buildIndex(corpus);

	/*std::cout << "print index\n";
	for (const auto& print : index) {

		for (const auto& num : print.second) {
			std::cout << num << ' ';
		}
		std::cout << print.first << '\t';
		std::cout << std::endl;
	}*/
	/*std::cout << "print corpus\n";
	for (const auto& print : corpus) {
		std::cout << print.first << ' ';
		for (const auto& num : print.second) {
			std::cout << num;
		}
		std::cout << std::endl;
	}*/

	// execute query
	//std::ifstream queriesFile(argv[2]);
	std::ifstream queriesFile("C:\\Users\\chius\\OneDrive\\桌面\\Simple_Search_Engine_3\\query_1.txt");
	if (!queriesFile) {
		std::cout << "Failed to open query file.\n";
		return 1;
	}
	std::unordered_map<std::string, double> idfMap = calculateIDF(corpus);
	std::unordered_map<int, std::unordered_map<std::string, double>> termFrequencyMap = calculateNormalizeTermFrequency(corpus);
	/*std::cout << "print idfMap\n";
	for (auto& it : idfMap) {
		std::cout << it.first << "\t\t" << it.second << std::endl;
	}*/

	//std::vector<std::vector<std::pair<int, double>>> topSumIDFs;
	while (std::getline(queriesFile, line)) {

		// find top rank of new tf*IDFs
		std::vector<std::pair<int, double>> topKRanks = findTopKRanks(line, idfMap, termFrequencyMap, top_k);

		int isFisrt = 0;
		for (const auto& rank : topKRanks) {
			if (rank.second > 0)
				if (!isFisrt) {
					std::cout << rank.first;
					isFisrt++;
				}
				else
					std::cout << " " << rank.first;
			else
				if (!isFisrt) {
					std::cout << "-1";
					isFisrt++;
				}
				else
					std::cout << " " << -1;
		}
		isFisrt = 0;
		std::cout << std::endl;

		/*for (const auto& rank : topKRanks) {
			if (rank.second > 0)
				std::cout << "Document ID: " << rank.first << ", Rank: " << rank.second << std::endl;
			else
				std::cout << "Document ID: " << -1 << ", Rank: " << rank.second << std::endl;
		}
		std::cout << std::endl;*/

		// find top sum IDFs
		/*
		std::vector<int> topThreeIDFs = findTopThreeSumIDF(line, corpus, idfMap);

		for (int i = 0; i < top_k; ++i) {
			if (i == 0) {
				std::cout << topThreeIDFs[i];
			}
			else {
				std::cout << " " << topThreeIDFs[i];
			}
		}
		std::cout << std::endl;
		*/

		// execute query - intersection
		/*int temp = 0;
		std::set<int> result = executeQuery(keywords, index);
		for (int id : result) {
			if (temp == 0) {
				std::cout << id;
				temp++;
			}
			else
				std::cout << " " << id;
		}
		std::cout << std::endl;
		*/
	}

	queriesFile.close();

}