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
        
        //LFSR algorithm
        n = (n >> 1) ^ ((n & 1u) ? TAPS : 0u);

        OUTPUT_PACKET out_pkt;
        out_pkt.data = (ap_uint<64>)n;
        out_pkt.keep = 0xFF;
        out_pkt.strb = 0xFF;
        out_pkt.last = in_pkt.last;

        output_stream.write(out_pkt);

        if (in_pkt.last)
            break;
    }
}
