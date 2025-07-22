#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

#define SEED 19660809u
#define TAPS 0xd0000001u

typedef ap_uint<32> AXI_VALUE;
typedef ap_axis<64,2,5,6> INPUT_PACKET;
typedef ap_axis<64,2,5,6> OUTPUT_PACKET;

typedef hls::stream<INPUT_PACKET> INPUT_STREAM;
typedef hls::stream<OUTPUT_PACKET> OUTPUT_STREAM;

void top_module(INPUT_STREAM& input_stream, OUTPUT_STREAM& output_stream) {
#pragma HLS INTERFACE axis port=input_stream
#pragma HLS INTERFACE axis port=output_stream
#pragma HLS INTERFACE s_axilite port=return

    static AXI_VALUE n = SEED;

    while (1) {
        INPUT_PACKET in_pkt = input_stream.read();

        // Extract probability and input bit
        AXI_VALUE prob = in_pkt.data.range(63,32);
        bool bit_entrada = in_pkt.data[0];

        // update LFSR 32 bits
        n = (n >> 1) ^ ((n & 1u) ? TAPS : 0u);

        // Comparison between the probability and the output bit
        bool resultado = (n < prob);

        // Output bit = XOR between bit_entrada & resultado
        bool bit_salida = bit_entrada ^ resultado;

        // Output packet construction
        OUTPUT_PACKET out_pkt;
        out_pkt.data.range(63,32) = n;
        out_pkt.data.range(31,1) = 0;
        out_pkt.data[0] = bit_salida;

        out_pkt.keep = 0xFF;  // validate all bytes
        out_pkt.strb = 0xFF;
        out_pkt.last = in_pkt.last;

        output_stream.write(out_pkt);

        if (in_pkt.last)
            break;
    }
}
