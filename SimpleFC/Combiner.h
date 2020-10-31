//
// Created by ZJZJ on 2020/10/1.
//

#ifndef COMBINELIBRARY_COMBINER_H
#define COMBINELIBRARY_COMBINER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class Combiner {

private:

	vector<string> Lines;

	vector<unsigned int> noteIndexs;
	vector<double> Positions;
	vector<double> Intervals;
	vector<double> Pitches;
	vector<string> Types;

	int Code;  // 作为保存运行结果的代码变量
	double autoPitchHalf;
	int Length{};
	double urLength{};
	double sumLength{};
	double tempo{};
	double beat{};
	unsigned int prevIndex{};
	unsigned int nextIndex{};

	void readFile(const string &path);

	void writeFile(const string &path);

	void split(const string &s, vector<string> &tokens, const string &delimiters);

	void getNotesIndex();

	double getSingleVar(const string &s);

	void Normalize();

	void reWrite();

	vector<double> ConvertPB(const string &s);

	vector<string> ConvertPBM(const string &s);

	string rConvertPB(const vector<double> &w, int l);

	string rConvertPBM(const vector<string> &w, int l);

	void deleteLatter();

public:

	Combiner();

	Combiner(const string &FileName);

	~Combiner() = default;

	static double Number(const string &s);

	static string String(const double &n);

	static bool isNumber(const string &s);

	static bool isNumber2(const string &s);

	int Main();

	void setDefaultPitch(double n);

	void Output(const string &FileName);

	void loadFile(const string &FileName);

	int getCode();
};

Combiner::Combiner() {
	Code = 0;
	autoPitchHalf = 25;
}

// 读取临时文件
void Combiner::readFile(const string &path) {

	fstream fs(path);
	string line;

	while (getline(fs, line)) {
		Lines.push_back(line);
	}

	fs.close();

}

// 输出临时文件
void Combiner::writeFile(const string &path) {

	fstream fs;
	fs.open(path, ios::out);

	unsigned int i;
	string str;
	for (i = 0; i < Lines.size(); ++i) {
		fs << Lines[i] << "\r\n";
	}

	fs.close();
}

// 分隔字符串
void Combiner::split(const string &s, vector<string> &tokens, const string &delimiters = " ") {
	string::size_type lastPos = s.find_first_not_of(delimiters, 0);
	string::size_type pos = s.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos) {
		tokens.push_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delimiters, pos);
		pos = s.find_first_of(delimiters, lastPos);
	}
}

// 初步解析临时文件内容
void Combiner::getNotesIndex() {

	unsigned int  i;
	bool getTempo = false;
	bool getMode = false;
	bool tempoError = false;
	bool modeError = false;

	prevIndex = 0;
	nextIndex = Lines.size();

	Code = 1;

	// 处理是否有前驱或后继音符
	for (i = 0; i < Lines.size(); ++i) {
		if (Lines[i].substr(0, 1) == "[") {
			if (isNumber(Lines[i].substr(2, Lines[i].size() - 3))) {
				noteIndexs.push_back(i);
			}
			if (Lines[i].substr(2, Lines[i].size() - 3) == "PREV") {
				prevIndex = i;
			}
			if (Lines[i].substr(2, Lines[i].size() - 3) == "NEXT") {
				nextIndex = i;
			}
		}
	}

	// 检查 Mode2 和曲速设定
	for (i = 0; i < Lines.size(); ++i) {
		if (Lines[i].substr(0, 5) == "Mode2" && !getMode) {
			getMode = true;
			if (Lines[i].substr(6, 1) == "F") {
				modeError = true;
				break;
			}
		}

		if (Lines[i].substr(0, 5) == "Tempo") {
			if (i < nextIndex) {
				tempo = getSingleVar(Lines[i]);
			}

			if (!getTempo) {
				//cout << "Global Tempo: " << tempo << "\n";
				getTempo = true;
			}
			else if (!noteIndexs.empty()) {
				if (i < noteIndexs[0]) {
					//cout << "Prev Tempo: " << tempo << "\n";
				}
				else if ((noteIndexs.size() == 1 && i < nextIndex) || (noteIndexs.size() > 1 && i < noteIndexs[1])) {
					//cout << "Current Tempo: " << tempo << "\n";
				}
				else if (i < nextIndex) {
					//cout << "Unexpected Tempo: " << tempo << "\n";
					tempoError = true;
					break;
				}
			}
		}
	}

	if (modeError || !getMode) {
		Code = 10;
		//cout << "\nPlease run the plugin-in in Mode 2. Press enter to exit.";
	}
	else if (tempoError) {
		Code = 11;
		//cout << "\nThe BPM is changed in the region. Press enter to exit.";
	}
	else if (noteIndexs.empty()) {
		Code = 13;
		//cout << "\nNo notes are selected. Press enter to exit.";
	}
}

// 计算合并音符的属性
void Combiner::Normalize() {
	unsigned int  i;
	unsigned int  j;
	string curStr;
	double curX;
	double curL;
	double curP;
	double lastP;
	double PBS;
	vector<double> PBW;
	vector<double> PBY;
	vector<string> PBM;
	bool getPBS;
	bool getPBW;
	bool getPBY;
	bool getPBM;

	int NoteNum = 0;
	int referNoteNum = 0;
	int lastLength = 0;

	beat = 60 / tempo * 1000;
	PBS = 0;
	curP = 0;
	lastP = 0;
	curX = 0;
	curL = 0;

	sumLength = 0;
	if (prevIndex > 0) {
		j = prevIndex + 1;
		while (j < Lines.size() && Lines[j].substr(0, 1) != "[") {
			if (Lines[j].substr(0, 6) == "Length") {
				lastLength = floor(getSingleVar(Lines[j]) / 480 * beat);
			}
			j++;
		}
	}
	for (i = 0; i < noteIndexs.size(); ++i) {
		j = noteIndexs[i] + 1;
		getPBS = false;
		getPBW = false;
		getPBY = false;
		getPBM = false;

		while (j < Lines.size() && Lines[j].substr(0, 1) != "[") {
			if (Lines[j].substr(0, 6) == "Length") {
				urLength = getSingleVar(Lines[j]) / 480 * beat;
				Length = floor(urLength);
				curL += urLength;
				sumLength += getSingleVar(Lines[j]);
			}
			if (Lines[j].substr(0, 7) == "NoteNum") {
				NoteNum = int(getSingleVar(Lines[j]));
				if (referNoteNum == 0) {
					referNoteNum = NoteNum;
				}
				curP = (NoteNum - referNoteNum) * 10;
			}
			if (!getPBS && Lines[j].substr(0, 3) == "PBS") {
				PBS = getSingleVar(Lines[j]);
				getPBS = true;
			}
			if (!getPBW && Lines[j].substr(0, 3) == "PBW") {
				PBW = ConvertPB(Lines[j]);
				getPBW = true;
			}
			if (!getPBY && Lines[j].substr(0, 3) == "PBY") {
				PBY = ConvertPB(Lines[j]);
				getPBY = true;
			}
			if (!getPBM && Lines[j].substr(0, 3) == "PBM") {
				PBM = ConvertPBM(Lines[j]);
				getPBM = true;
			}
			j++;
		}

		// 添加默认滑音时需要考虑前后音符的长度，不超过长度的一半
		if (!getPBS) {
			PBS = -((autoPitchHalf > lastLength / 2) ? (lastLength / 2) : autoPitchHalf);
		}
		if (!getPBW) {
			PBW = { fabs(PBS) + ((autoPitchHalf > Length / 2) ? (Length / 2) : autoPitchHalf) };
		}
		if (!getPBY) {
			PBY = { 0 };
		}
		// 用 x 代替空白位置
		if (!getPBM) {
			PBM = { "x" };
		}

		// 补足后面的 0 和 x
		while (PBY.size() < PBW.size()) {
			PBY.push_back(0);
		}
		while (PBM.size() < PBW.size()) {
			PBM.push_back("x");
		}
		curX += PBS;
		Positions.push_back(floor(curX * 10) / 10);
		Pitches.push_back(lastP);
		Types.push_back("x");

		for (j = 0; j < PBW.size(); ++j) {
			curX += PBW[j];
			Positions.push_back(curX);
			Pitches.push_back(curP + PBY[j]);
			Types.push_back(PBM[j]);
		}

		curX = round(curL * 10) / 10;
		lastP = curP;
		lastLength = Length;
	}

	if (Pitches[Pitches.size() - 1] != 0) {
		Pitches.push_back(Pitches[Pitches.size() - 1]);
		Pitches.push_back(0);
		Positions.push_back(curL);
		Positions.push_back(curL);
		Types.push_back("x");
		Types.push_back("x");
	}
	for (i = 1; i < Positions.size(); ++i) {
		Intervals.push_back(Positions[i] - Positions[i - 1]);
		Intervals[Intervals.size() - 1] = round(Intervals[Intervals.size() - 1] * 10) / 10;
	}

	for (i = 0; i < Intervals.size(); ++i) {
		if (Intervals[i] < 0) {
			Code = 2;
		}
	}

	// 如果音符只有一个，那么删除所有多余控制点
	if (noteIndexs.size() == 1) {
		Code = 0;
		for (i = 1; i < Pitches.size(); ++i) {

			if (i < Pitches.size() - 1 && i >= 2) {
				if (Pitches[i - 1] == Pitches[i] && Pitches[i] == Pitches[i + 1]) {
					Intervals[i] += Intervals[i - 1];
					Intervals.erase(Intervals.begin() + i - 1);
					Pitches.erase(Pitches.begin() + i);
					Types.erase(Types.begin() + i);
					i--;
					Code = 3;
				}
			}

		}
	}
};

// 判断是不是整数
bool Combiner::isNumber(const string &s) {
	unsigned int i;
	bool flag = true;
	for (i = 0; i < s.size(); ++i) {
		if (s.substr(i, 1) < "0" || s.substr(i, 1) > "9") {
			flag = false;
			break;
		}
	}
	return flag;
}

// 判断是不是整数、小数、负数
bool Combiner::isNumber2(const string &s) {
	unsigned int i;
	bool flag = true;

	for (i = 0; i < s.size(); ++i) {
		if ((s.substr(i, 1) < "0" || s.substr(i, 1) > "9") && s.substr(i, 1) != "-" && s.substr(i, 1) != ".") {
			flag = false;
			break;
		}
	}
	return flag;
};

// 获取等号后面的内容
double Combiner::getSingleVar(const string &s) {

	unsigned int i = 0;
	unsigned int j;
	string curStr;

	double num;

	while (s.substr(i, 1) != "=") {
		++i;
	}
	i++;
	j = i;

	while (isNumber2(s.substr(j, 1)) && j < s.size()) {
		++j;
	}

	curStr = s.substr(i, j - i + 1);
	num = Number(curStr);

	return num;
}

// 分隔字符串到线性表中（数字）
vector<double> Combiner::ConvertPB(const string &s) {
	unsigned int i = 0, j;
	string curStr, curStr0;
	bool space;
	vector<string> strArr;
	vector<double> numArr;

	while (s.substr(i, 1) != "=") {
		++i;
	}
	i++;
	curStr0 = s.substr(i);

	space = true;

	for (j = 0; j < curStr0.size(); ++j) {
		if (curStr0.substr(j, 1) == ",") {
			if (space) {
				curStr += "0,";
			}
			else {
				curStr += ",";
				space = true;
			}
		}
		else {
			curStr += curStr0.substr(j, 1);
			space = false;
		}
	}
	if (space) {
		curStr += "0";
	}

	split(curStr, strArr, ",");

	for (i = 0; i < strArr.size(); ++i) {
		numArr.push_back(Number(strArr[i]));
	}
	return numArr;
};

// 分隔字符串到线性表中（字符串）
vector<string> Combiner::ConvertPBM(const string &s) {
	unsigned int i = 0, j;
	string curStr, curStr0;
	bool space;
	vector<string> strArr;

	while (s.substr(i, 1) != "=") {
		++i;
	}
	i++;
	curStr0 = s.substr(i);

	space = true;

	for (j = 0; j < curStr0.size(); ++j) {
		if (curStr0.substr(j, 1) == ",") {
			if (space) {
				curStr += "x,";
			}
			else {
				curStr += ",";
				space = true;
			}
		}
		else {
			curStr += curStr0.substr(j, 1);
			space = false;
		}
	}
	if (space) {
		curStr += "x";
	}

	split(curStr, strArr, ",");

	return strArr;
};

// 将线性表中的内容组合为字符串（数字)
string Combiner::rConvertPB(const vector<double> &w, int l = 0) {
	unsigned int  i = 0;
	string s0;

	for (i = l; i < w.size() - 1; ++i) {
		s0 += String(w[i]) + ",";
	}
	s0 += String(w[i]);
	return s0;
};

// 将线性表中的内容组合为字符串（字符串)
string Combiner::rConvertPBM(const vector<string> &w, int l = 0) {
	unsigned int  i = 0;
	string s0;
	bool flag = true;

	for (i = l; i < w.size() - 1; ++i) {
		s0 += (w[i] == "x" ? "" : w[i]) + ",";
	}

	s0 += (w[i] == "x" ? "" : w[i]);

	for (i = l; i < w.size() - 1; ++i) {
		if (w[i] != "x") {
			flag = false;
			break;
		}
	}
	if (flag) {
		s0 = "";
	}
	return s0;
};

// 修改属性内容
void Combiner::reWrite() {
	int i;
	unsigned int  j;
	bool getPBS = false;
	bool getPBW = false;
	bool getPBY = false;
	bool getPBM = false;
	string strPBS = "PBS=" + String(Positions[0]);
	string strPBW = "PBW=" + rConvertPB(Intervals);
	string strPBY = "PBY=" + rConvertPB(Pitches, 1);
	string strPBM = "PBM=" + rConvertPBM(Types, 1);
	i = noteIndexs[0];
	j = i + 1;
	while (j < Lines.size() && Lines[j].substr(0, 1) != "[") {
		if (Lines[j].substr(0, 6) == "Length") {
			Lines[j] = "Length=" + String(sumLength);
		}
		if (!getPBS && Lines[j].substr(0, 3) == "PBS") {
			getPBS = true;
		}
		if (!getPBW && Lines[j].substr(0, 3) == "PBW") {
			Lines[j] = strPBW;
			getPBW = true;
		}
		if (!getPBY && Lines[j].substr(0, 3) == "PBY") {
			Lines[j] = strPBY;
			getPBY = true;
		}
		if (!getPBM && Lines[j].substr(0, 3) == "PBM") {
			if (strPBM == "PBM=") {
				Lines[j] = "";
			}
			else {
				Lines[j] = strPBM;
			}
			getPBM = true;
		}
		j++;
	}
	if (!getPBS) {
		Lines[j - 1] += "\r\n" + strPBS;
	}
	if (!getPBW) {
		Lines[j - 1] += "\r\n" + strPBW;
	}
	if (!getPBY) {
		Lines[j - 1] += "\r\n" + strPBY;
	}
	if (!getPBM && strPBM != "PBM=") {
		Lines[j - 1] += "\r\n" + strPBM;
	}
	deleteLatter();

	//cout << "Combine Success.\n";
};

// 删除后面所有音符
void Combiner::deleteLatter() {
	unsigned int  i;
	for (i = 1; i < noteIndexs.size(); ++i) {
		Lines[noteIndexs[i]] = "[#DELETE]";
	}
};

// 将字符串转换为数字
double Combiner::Number(const string &s) {
	stringstream ss;
	double n;

	ss << s;
	ss >> n;

	return n;
}

// 将数字转换为字符串
string Combiner::String(const double &n) {
	stringstream ss;
	string s;

	ss << n;
	ss >> s;

	return s;
}

// 象征性的处理入口函数
int Combiner::Main() {

	getNotesIndex();

	if (Code < 10) {
		Normalize();
		reWrite();
	}

	return 0;
}

Combiner::Combiner(const string &FileName) {
	Code = 0;
	autoPitchHalf = 25;

	readFile(FileName);
}

void Combiner::setDefaultPitch(double n) {
	autoPitchHalf = n;
}

void Combiner::Output(const string &FileName) {
	writeFile(FileName);
}

void Combiner::loadFile(const string &FileName) {
	readFile(FileName);
}

int Combiner::getCode() {

	return Code;
}

#endif //COMBINELIBRARY_COMBINER_H
