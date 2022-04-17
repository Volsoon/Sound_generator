#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "sound_generator.hpp"
#include <vector>
#include <string>

// Riff
const std::string chunk_id("RIFF");
const std::string chunk_size("____");
const std::string format("WAVE");

// FMT
const std::string subchunk1_id("fmt ");
const int subchunk1_size(16);
const int audio_format(1);
const int num_channels(2);
const int sample_rate(44100);
const int byte_rate(sample_rate * num_channels * subchunk1_size/8);
const int block_align(num_channels * subchunk1_size/8);
const int bits_per_sample(16);

// Data
const std::string subchunk2_id("data");
const std::string subchunk2_size("____");

void write_as_byte(std::ofstream &file, int value, int byte_size) {
    file.write((const char*)(&value), byte_size);
}

void writeWAV(std::vector<int> data) {
    std::ofstream SoundFile;
    SoundFile.open("sound2.wav", std::ios::binary);

    if(SoundFile.is_open()){
        // RIFF
        SoundFile << chunk_id;
        SoundFile << chunk_size;
        SoundFile << format;

        // FMT
        SoundFile << subchunk1_id;
        write_as_byte(SoundFile, subchunk1_size, 4);
        write_as_byte(SoundFile, audio_format, 2);
        write_as_byte(SoundFile, num_channels, 2);
        write_as_byte(SoundFile, sample_rate, 4);
        write_as_byte(SoundFile, byte_rate, 4);
        write_as_byte(SoundFile, block_align, 2);
        write_as_byte(SoundFile, bits_per_sample, 2);
        
        // Data
        SoundFile << subchunk2_id;
        SoundFile << subchunk2_size;

        int start_audio(SoundFile.tellp());
        for(int i = 0;i < data.size();i++){
            write_as_byte(SoundFile, data[i], 2);
        }
        /*for(std::vector<int> item: c2) {
            for(int j: item) {
                write_as_byte(SoundFile, j, 2);
            }
        }*/

        int end_audio(SoundFile.tellp());
        SoundFile.seekp(start_audio - 4);
        write_as_byte(SoundFile, end_audio - start_audio, 4);
        SoundFile.seekp(4, std::ios::beg);
        write_as_byte(SoundFile, end_audio - 8, 4);

    }
    SoundFile.close();
}

std::vector<std::string> split(std::string Str, std::string delimiter) {
    std::vector<std::string> split_string;
    auto start = 0U;
    auto end = Str.find(delimiter);
    while (end != std::string::npos)
    {
        split_string.push_back(Str.substr(start, end - start));
        start = end + delimiter.length();
        end = Str.find(delimiter, start);
    }
    split_string.push_back(Str.substr(start, end));
    return split_string;
}

void process(Note* n, std::string p) {
    int p_size = int(p.size());
    if(p != "z") {
        n->pitch.push_back(p);
        if(p[p_size-1] == '#') {
            n->accidental.push_back(1);
        }
        if(p[p_size-1] == 'b') {    
            n->accidental.push_back(-1);
        }
        else {
            n->accidental.push_back(0);
        }
    }
}

Note makenote(std::string noteString, Section section, Score score) {
    double length = section.Length;
    // std::cout << length << "\n";
    if(length == 0.0) {
		length = score.Length;
	}
    //std::cout << length << "\n";
    int vol = section.Volume;
    if(vol == 0) {
		vol = score.Volume;
	}
    std::string env = section.envelope;
    if(env == "") {
        env = score.envelope;
    }
    std::string har = section.harmonic;
    if(har == "") {
        har = score.harmonic;
    }

    Note nt = {
		pitch:      std::vector<std::string>(),
		accidental: std::vector<int>(),
		Length:     length,
		env:  env,
		har:  har,
		Volume:     vol,
	};
    std::vector<std::string> nArray = split(noteString, ":");
    double note_length;
    std::string note_pitch;

    if(nArray.size() > 1){
        note_length = std::stod(nArray[0]);
        note_pitch = nArray[1];
    } else {
        note_length = 1.0;
        note_pitch = nArray[0];
    }
    nt.Length = note_length * length;
     // std::cout << note_pitch.size() << "\n";
    if(note_pitch.size() > 3) {
        std::vector<std::string> chord = split(note_pitch, "-");
        if(chord.size() < 2) {
            std::cout << "Error: wrong chords structure - " << note_pitch << "\n";
    }
        for(int i = 0;i < chord.size(); i++){
            process(&nt, chord[i]);
        }
    } else {
            process(&nt, note_pitch);
    }
    return nt;
}

std::map<std::string, int> Pitch;

void setupPitches() {
    std::vector<std::string> notes = {"c", "d", "e", "f", "g", "a", "b"};
    std::vector<int> nums = {-57, -55, -53, -52, -50, -48, -46};
    for(int i = 1;i < 8;i++) {
        for(int j=0;j < notes.size();j++) {
            nums[j] = nums[j] + 12;
            std::string pitch_index = notes[j] + std::to_string(i);
            Pitch[pitch_index] = nums[j];
        }
    }
}

std::map<std::string, std::vector<int>> tuneKey;
std::vector<std::string> sharpKeys;
std::vector<std::string> flatKeys;

void setupKeys() {
    tuneKey["C"] = std::vector<int>{};
    sharpKeys = {"G", "D", "A", "E", "B", "F#", "C#"};
    std::vector<int> sharpNotes = {Pitch["f1"], Pitch["c1"], Pitch["g1"], Pitch["d1"], 
    Pitch["a1"], Pitch["e1"], Pitch["b1"]};
    for(int i = 0;i < sharpKeys.size();i++) {
        std::vector<int> k;
        for(int j =0;j < i+1;j++) {
            for(int l =0;l < 6;l++) {
                k.push_back(sharpNotes[j]+(12*l));
            }
        }
        std::string index = sharpKeys[i];
        tuneKey[index] = k; 
    }

    flatKeys = {"F", "Bb", "Eb", "Ab", "Db", "Gb", "Cb"};
    std::vector<int> flatNotes = {Pitch["b1"], Pitch["e1"], Pitch["a1"], Pitch["d1"], 
    Pitch["g1"], Pitch["c1"], Pitch["f1"]};

    for(int i = 0;i < flatKeys.size();i++) {
        std::vector<int> k;
        for(int j =0;j < i+1;j++) {
            for(int l =0;l < 6;l++) {
                k.push_back(flatNotes[j]+(12*l));
            }
        }
        std::string index = flatKeys[i];
        tuneKey[index] = k; 
    }
}

std::vector<int> concat(std::vector<std::vector<int>> notes) {
    std::vector<int> data;
    int l = notes[0].size();
    for(std::vector<int> note: notes) {
        if(note.size() != l){
            std::cout << "Error : length of notes are not the same" << "\n";
            return data;
        }
    }

    for(int i =0;i < l;i++) {
        int d = 0;
        for(int j = 0;j < notes.size();j++) {
             // std::cout << "index : " << j << "\n";
            d += notes[j][i];
            // std::cout << "d : " << d << "\n";
        }
        data.push_back(d);
        // std::cout << "data : " << data[i] << "\n";
    }
    return data;
}

double flat(double input, double duration) {
    return 1;
}
double drop(double input, double duration) {
    return std::cos((3.14 * input) / (2 * duration));
}
double rise(double input, double duration) {
    return std::sin((3.14 * input) / (2 * duration));
}
double round(double input, double duration) {
    return std::sin(3.14 * input / duration);
}   
double triangle(double input, double duration) {
    return (2 / 3.14) * std::asin(std::sin(2*3.14*input/duration));
}
double tadpole(double input, double duration) {
    return std::sin(3.14*input/duration) - (0.5*std::sin(2*3.14*input/duration)) + (0.333 * std::sin(3*3.14*input/duration)) - (0.25 * std::sin(4 * 3.14*input/duration));
}
double combi(double input, double duration) {
    return std::sin((3.14*input)/(duration))/2 + std::cos((3.14*input)/(2*duration))/3;
}
double diamond(double input, double duration) {
    return (2/3.14) * std::asin(std::sin(3.14*input/duration))/4 + (2/3.14)*std::asin(std::sin(2*3.14*input/duration))/4;
}
double drawl(double input, double duration) {
    return 1 / std::log10((2*3.14*input/duration)+1.9);
}
double tempered(double input, double duration) {
    return drop(input, duration) * drawl(input, duration);
}

double base(double input) {
    return 2 * 3.14 * input;
}
double first(double input) {
    return std::sin(base(input));
}
double second(double input) {
    return std::sin(base(input)) + std::sin(base(input)*2);
}
double third(double input) {
    return std::sin(base(input)) + std::sin(base(input)*2) + std::sin(base(input)*3);
}
double stringed(double input) {
    return 3*std::sin(base(input)) + 0.5*std::sin(base(input)*0.5) + 1.5*std::sin(base(input)*2) + 0.25*std::sin(base(input)*3) + 0.125*std::sin(base(input)*4);
}

double envelope_selector(std::string env, double input, double duration) {
    if(env == "flat") {
        return flat(input, duration);
    }
    if(env == "drop") {
        return drop(input, duration);
    }
    if(env == "rise") {
        return rise(input, duration);
    }
    if(env == "round") {
        return round(input, duration);
    }
    if(env == "triangle") {
        return triangle(input, duration);
    }
    if(env == "tadpole") {
        return tadpole(input, duration);
    }
    if(env == "combi") {
        return combi(input, duration);
    }
    if(env == "diamond") {
        return diamond(input, duration);
    }
    if(env == "drawl") {
        return drawl(input, duration);
    }
    if(env == "tempered") {
        return tempered(input, duration);
    }
}
double harmonic_selector(std::string har, double input) {
    if(har == "base") {
        return base(input);
    }
    if(har == "first") {
        return first(input);
    }
    if(har == "second") {
        return second(input);
    }
    if(har == "third") {
        return third(input);
    }
    if(har == "stringed") {
        return stringed(input);
    }
}

std::vector<int> noteData(double frequency, double duration, std::string env, std::string har, int vol) {
    std::vector<int> data;
    for(double i = 0.0;i < duration;i = i + (1.0 / (double)sample_rate)) {
        int x = int(vol * envelope_selector(env, i, duration) * harmonic_selector(har, i * frequency));
        data.push_back(x);
    }
    return data;
}

double frequency(int step) {
    return 440.0 * (std::pow(2, step / 12.0));
}

std::vector<int> rest(double duration) {
    std::vector<int> data;
    for(double i =0;i < duration;i = i + (1.0/(double)sample_rate)) {
        data.push_back(0);
    }
    return data;
}

std::vector<int> Note::encode() {
    std::vector<int> data;
    if(Note::pitch.size() != Note::accidental.size()) {
        std::cout << "length of pitches and accidentals not the same" << "\n";
        return data;
    }

    if(Note::pitch.size() == 0 && Note::Length != 0) {
        data = rest(Note::Length);
        return data;
    }
    std::vector<std::vector<int>> notes;
    for(int i =0;i < Note::pitch.size();i++) {
        int note_pitch = frequency(Pitch[Note::pitch[i]] + Note::accidental[i]);
        notes.push_back(noteData(note_pitch, Note::Length, Note::env, 
        Note::har, Note::Volume));
        // std::cout << Note::Length << "\n";
    }

    data = concat(notes);
    return data;
}

const int tolerance = 1500;

std::vector<int> stereo(std::vector<std::vector<int>> c1, std::vector<std::vector<int>> c2) {
    std::vector<int> data;
    std::cout << c2.size() << "\n";
    if(c2.size() == 0) {
        c2 = c1;
    }
    int d1 = c1.size() - c2.size();
    int d2 = c2.size() - c1.size();
    if(d1 > 0 && d1 < tolerance )  {
        for(int i =0;i < d1;i++) {
            c1.pop_back();
        }
    }
    if(d2 > 0 && d2 < tolerance )  {
        for(int i =0;i < d2;i++) {
            c2.pop_back();
        }
    }
    if(c1.size() == c2.size()) {
        for(int i=0;i < c1.size();i++){
            int counter;
            if(c1[i].size() < c2[i].size()) {
                counter = c2[i].size();
            } else {
                counter = c1[i].size();
            }
            for(int j=0;j < counter;j++){
                if(i < c1[i].size()) {
                    data.push_back(c1[i][j]);
                }
                if(i < c2[i].size()) {
                    data.push_back(c2[i][j]);
                }
            }
        }
    }
    return data;
}

bool inKey(std::vector<std::string> key, std::string note) {
    for(std::string item: key) {
        if(item == note) {
            return true;
        }
    }
    return false;
}

bool inNote(std::vector<int> notes, int note) {
    for(int item: notes) {
        if(item == note) {
            return true;
        }
    }
    return false;
}

std::vector<int> Tune::encode() {
    std::vector<int> data;
    int acc = 0;
    if(inKey(sharpKeys, Tune::Key)) {
        acc = 1;
    }
    if(inKey(flatKeys, Tune::Key)) {
        acc = -1;
    }
    std::vector<std::vector<Note>> channels = {Tune::ch1, Tune::ch2};
    for(std::vector<Note> channel: channels) {
        for(Note nt: channel) {
            for(int i=0;i < nt.pitch.size();i++) {
                if(inNote(tuneKey[Tune::Key], nt.pitch[i][0])) {
                    nt.accidental[i] += acc;
                }
            }
        }
    }

    std::vector<std::vector<int>> channel1, channel2;
    std::vector<int> n;
    for(Note note: Tune::ch1) {
        n = note.encode();
        channel1.push_back(n);
    }
    for(Note note: Tune::ch2) {
        n = note.encode();
        channel2.push_back(n);
    }

    data = stereo(channel1, channel2);
    
    writeWAV(data);
    return data;
}

void Parse(std::string FileName) {
    YAML::Node file = YAML::LoadFile(FileName);
    std::vector<Section> sections;
    for(int i = 0;i < file["sections"].size(); i++){
        double length;
        std::string envelope;
        std::string harmonic;
        int volume;
        if(!file["sections"][i]["length"]) {
            length = file["length"].as<double>();
        } else {
            length = file["sections"][i]["length"].as<double>();
        }
        if(!file["sections"][i]["envelope"]) {
            envelope = file["envelope"].as<std::string>();
        } else {
            envelope = file["sections"][i]["envelope"].as<std::string>();
        }
        if(!file["sections"][i]["harmonic"]) {
            harmonic = file["harmonic"].as<std::string>();
        } else {
            harmonic = file["sections"][i]["harmonic"].as<std::string>();
        }
        if(!file["sections"][i]["volume"]) {
            volume = file["volume"].as<int>();
        } else {
            volume = file["sections"][i]["volume"].as<int>();
        }

        Section section = {
            Length: length,
            envelope: envelope,
            harmonic: harmonic,
            Volume: volume,
            C1: file["sections"][i]["C1"].as<std::vector<std::string>>(),
            C2: file["sections"][i]["C2"].as<std::vector<std::string>>()
        };
        sections.push_back(section);
    }

    Score s = {
        Name: file["name"].as<std::string>(),
        key: file["key"].as<std::string>(),
        Length: file["length"].as<double>(),
        envelope: file["envelope"].as<std::string>(),
        harmonic: file["harmonic"].as<std::string>(),
        Volume: file["volume"].as<int>(),
        sections: sections
    };

    Tune t = {
        Key: s.key,
        ch1: {},
        ch2: {}
    };

    for(int i=0;i < s.sections.size(); i++) {

        for(int n = 0;n < s.sections[i].C1.size(); n++) {
            Note note = makenote(s.sections[i].C1[n], s.sections[i], s);
            t.ch1.push_back(note);
        }

        for(int n = 0;n < s.sections[i].C2.size(); n++) {
            Note note = makenote(s.sections[i].C2[n], s.sections[i], s);
            t.ch2.push_back(note);
        }
    }
    
    std::vector<int> data = t.encode();
    //writeWAV(data);
}


int main(){
    setupKeys();
    setupPitches();
    Parse("test.yaml");
    return 0;
}