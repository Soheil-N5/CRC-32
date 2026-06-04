#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdint>  
#include <cstdlib>  

using namespace std;

const uint32_t ETHERNET_POLY = 0x04C11DB7;

vector<uint8_t> hexToBytes(const string& hexStr) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hexStr.length(); i += 2) {
        string byteString = hexStr.substr(i, 2);
        uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

void printBytes(const vector<uint8_t>& bytes) {
    for (uint8_t b : bytes) {
        cout << hex << uppercase << setw(2) << setfill('0') << (int)b << " ";
    }
    cout << dec << endl; }

uint32_t computeCRC32(const vector<uint8_t>& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t b : data) {
        crc ^= (b << 24);
        for (int i = 0; i < 8; i++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ ETHERNET_POLY;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc ^ 0xFFFFFFFF; }

// ================= Part A =================
vector<uint8_t> generateTransmittedFrame(const string& hexFrame) {
    vector<uint8_t> frameBytes = hexToBytes(hexFrame);
    
    uint32_t crc = computeCRC32(frameBytes);
    
    frameBytes.push_back((crc >> 24) & 0xFF);
    frameBytes.push_back((crc >> 16) & 0xFF);
    frameBytes.push_back((crc >> 8) & 0xFF);
    frameBytes.push_bac(crc & 0xFF);
    
    return frameBytes;
}

// ================= Part B =================
void verifyReceivedFrame(const vector<uint8_t>& receivedFrame) {
    if (receivedFrame.size() < 4) {
        cout << "[Error] Frame is too short to contain FCS!" << endl;
        return;
    }
    
    vector<uint8_t> payload(receivedFrame.begin(), receivedFrame.end() - 4);
    
    uint32_t receivedFCS = 
        (receivedFrame[receivedFrame.size() - 4] << 24) |
        (receivedFrame[receivedFrame.size() - 3] << 16) |
        (receivedFrame[receivedFrame.size() - 2] << 8) |
        (receivedFrame[receivedFrame.size() - 1]);
        
    uint32_t calculatedCRC = computeCRC32(payload);
    
    cout << "Extracted FCS: 0x" << hex << uppercase << setw(8) << setfill('0') << receivedFCS << endl;
    cout << "Calculated CRC: 0x" << hex << uppercase << setw(8) << setfill('0') << calculatedCRC << dec << endl;
    
    if (calculatedCRC == receivedFCS) {
        cout << "=> [Status]: SUCCESS! Frame is VALID." << endl;
    } else {
        cout << "=> [Status]: FAILED! Frame is CORRUPTED." << endl;
    }
}

int main() {
    string inputHexFrame = "AABBCCDDEEFF11223344556608004500002E0000000040060000";
    
    cout << "--- PART A: CRC Generation ---" << endl;
    cout << "Original Frame Data: " << inputHexFrame << endl;
    
    vector<uint8_t> readyToTransmitFrame = generateTransmittedFrame(inputHexFrame);
    
    cout << "Transmitted Frame (with FCS): ";
    printBytes(readyToTransmitFrame);
    cout << "\n--------------------------------\n";
    
    cout << "\n--- PART B: CRC Verification (Valid Scenario) ---" << endl;
    verifyReceivedFrame(readyToTransmitFrame);
    
    cout << "\n--- PART B: CRC Verification (Corrupted Scenario) ---" << endl;
    vector<uint8_t> corruptedFrame = readyToTransmitFrame;
    corruptedFrame[0] = 0xAB; 
    verifyReceivedFrame(corruptedFrame);

    return 0;
}
