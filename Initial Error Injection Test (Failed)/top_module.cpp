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

void top_module(INPUT_STREAM& input_packet, OUTPUT_STREAM& final_out) {
#pragma HLS INTERFACE axis port=input_packet
#pragma HLS INTERFACE axis port=final_out
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS PIPELINE

    static AXI_VALUE n = SEED;

    INPUT_PACKET packet_axi = input_packet.read();

    // Generate random number with LFSR
    n = (n >> 1) ^ ((n & 1u) ? TAPS : 0u);
    AXI_VALUE rand_val = n;

    // Obtain input and normalize
    ap_uint<32> prob_raw = packet_axi.data.range(63, 32);
    fixed_t prob = fixed_t(prob_raw) / fixed_t(4294967295.0);
    bit_t input_bit = packet_axi.data[0];
    fixed_t rand_norm = fixed_t(rand_val) / fixed_t(4294967295.0);

    // Compare with probability
    bit_t error_bit = (rand_norm < prob) ? 1 : 0;
    bit_t output_bit = input_bit ^ error_bit;

    // Packed the final result: [31:1] = rand_val[30:0], [0] = output_bit
    ap_uint<32> debug_out = (rand_val.range(30, 0) << 1) | output_bit;

    OUTPUT_PACKET out_pkt;
    out_pkt.data = debug_out;
    out_pkt.last = 1;
    final_out << out_pkt;
}
