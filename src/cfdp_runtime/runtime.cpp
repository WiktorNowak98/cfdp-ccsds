#include <cfdp_core/pdu_enums.hpp>

#include <iostream>

void printHelloWorld() { std::cout << (int)cfdp::pdu::header::PduType::FileData; }
