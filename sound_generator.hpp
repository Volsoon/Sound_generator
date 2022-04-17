#include <iostream>
#include <vector>
#include <map>


class Section {
    public:
        double Length;
        std::string envelope;
        std::string harmonic;
        int Volume;
        std::vector<std::string> C1;
        std::vector<std::string> C2;
};

class Score {
    public:
        std::string Name;
        std::string key;
        double Length;
        std::string envelope;
        std::string harmonic;
        int Volume;
        std::vector<Section> sections;
};

class Note {
    public:
        std::vector<std::string> pitch;
        std::vector<int> accidental;
        double Length;
        std::string env;
        std::string har;
        // std::string env_default;
        // std::string har_default;
        int Volume;

       std::vector<int> encode();
};

class Tune {
    public:
        std::string Key;
        std::vector<Note> ch1;
        std::vector<Note> ch2;

        std::vector<int> encode();
};

std::vector<int> stereo(std::vector<int> c1, std::vector<int> c2);
void setupPitches();
void setupKeys();
std::vector<int> noteData(double frequency, double duration, std::string env, std::string har, int vol);
double frequency(int step);
std::vector<int> rest(double duration);
void process(Note* n, std::string p);
Note makenote(std::string noteString, Section section, Score score);
std::vector<int> Parse(Score &s, unsigned char score[], std::string outfile);
std::vector<int> concat(std::vector<int> notes);
double flat(double input, double duration);
double drop(double input, double duration);
double rise(double input, double duration);
double round(double input, double duration);
double triangle(double input, double duration);
double tadpole(double input, double duration);
double combi(double input, double duration);
double diamond(double input, double duration);
double drawl(double input, double duration);
double tempered(double input, double duration);

double base(double input);
double first(double input);
double second(double input);
double third(double input);
double stringed(double input);