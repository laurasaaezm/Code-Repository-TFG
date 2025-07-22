#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <iostream>

typedef ap_axis<64,2,5,6> INPUT_PACKET;
typedef ap_axis<64,2,5,6> OUTPUT_PACKET;

typedef hls::stream<INPUT_PACKET> INPUT_STREAM;
typedef hls::stream<OUTPUT_PACKET> OUTPUT_STREAM;


void top_module(INPUT_STREAM& input_stream, OUTPUT_STREAM& output_stream);

int main() {
    INPUT_STREAM input_stream;
    OUTPUT_STREAM output_stream;

    // Probability 0.5. Integer format: (2^32-1)*0.5
    ap_uint<32> prob_fixed = 0xFFFFFFFF / 2; // 0.5 scaled

    // 10 input packets
    for (int i = 0; i < 10; i++) {
        INPUT_PACKET pkt;
        pkt.data.range(63,32) = prob_fixed;
        pkt.data.range(31,1) = 0;
        pkt.data[0] = 0;   // bit de entrada = 0
        pkt.keep = 0xFF;
        pkt.strb = 0xFF;
        pkt.last = (i == 9) ? 1 : 0;  // marcar el último paquete
        input_stream.write(pkt);
    }

    
    top_module(input_stream, output_stream);

    //Read and show output
    for (int i = 0; i < 10; i++) {
        OUTPUT_PACKET out_pkt = output_stream.read();
        ap_uint<32> rand_num = out_pkt.data.range(63,32);
        bool bit_out = out_pkt.data[0];

        std::cout << "Salida " << i << ": rand_num = " << rand_num
                  << ", bit_out = " << bit_out << std::endl;
    }

    return 0;
}
