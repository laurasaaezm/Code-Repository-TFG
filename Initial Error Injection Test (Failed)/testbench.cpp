#include <iostream>
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_fixed.h>
#include <ap_axi_sdata.h>

#define SEED 19660809u
#define TAPS 0xd0000001u

typedef ap_ufixed<48, 32> fixed_t;
typedef ap_uint<1> bit_t;
typedef ap_uint<32> AXI_VALUE;


typedef ap_axiu<64, 0, 0, 0> INPUT_PACKET;
typedef ap_axiu<32, 0, 0, 0> OUTPUT_PACKET;

typedef hls::stream<INPUT_PACKET> INPUT_STREAM;
typedef hls::stream<OUTPUT_PACKET> OUTPUT_STREAM;


void top_module(INPUT_STREAM& input_packet, OUTPUT_STREAM& final_out);

int main() {
    // Prepare streams
    INPUT_STREAM input_stream;
    OUTPUT_STREAM output_stream;

    const double PROB_TEST = 0.5; // Prueba con probabilidad 0.5
    const bit_t INPUT_BIT = 0;

    // Scale probability to 32 bits
    uint64_t temp = static_cast<uint64_t>(PROB_TEST * 4294967295.0);
    ap_uint<32> prob_scaled = (ap_uint<32>)temp;

    std::cout << "Probabilidad real usada: " << PROB_TEST
              << " (prob_scaled = 0x" << std::hex << prob_scaled << std::dec << ")\n";

    // Creat AXI input packet
    INPUT_PACKET test_packet;
    test_packet.data = (ap_uint<64>(prob_scaled) << 32) | INPUT_BIT;
    test_packet.keep = -1; // todos los bytes válidos
    test_packet.last = 0;

    for (int i = 0; i < 10; i++) {
        input_stream << test_packet;
        top_module(input_stream, output_stream);

        OUTPUT_PACKET result_pkt = output_stream.read();
        ap_uint<32> out_data = result_pkt.data;

        bit_t result_bit = out_data[0];
        ap_uint<31> rand_bits = out_data.range(31,1);
        double rand_norm = ((double)(rand_bits.to_uint())) / ((double)(1UL << 31));


        std::cout << "Prueba " << (i + 1)
                  << " | Output bit: " << (int)result_bit
                  << " | Rand bits: 0x" << std::hex << rand_bits << std::dec
                  << " | Norm: " << rand_norm
                  << " | Error aplicado: " << ((result_bit != INPUT_BIT) ? "Sí" : "No")
                  << "\n";
    }

    std::cout << "\n Test completado (10 pruebas)\n";
    return 0;
}

