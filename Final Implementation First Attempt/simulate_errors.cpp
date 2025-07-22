#define AP_INT_MAX_W 32768
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <ap_int.h>

#define SIZE 16
#define K 4
#define SIZE_OUT (SIZE / K)

#define SEED 19660809u
#define TAPS 0xd0000001u

typedef ap_axis<64, 2, 5, 6> axis_in_t;
typedef ap_axis<64 * K, 2, 5, 6> axis_out_t;

void simulate_errors(hls::stream<axis_in_t> &in_stream,
                     hls::stream<axis_out_t> &out_stream) {
#pragma HLS INTERFACE axis port=in_stream
#pragma HLS INTERFACE axis port=out_stream
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL


    static ap_uint<64> bram[SIZE];
#pragma HLS BIND_STORAGE variable=bram type=ram_2p impl=bram
#pragma HLS ARRAY_PARTITION variable=bram cyclic factor=K dim=1

    // BRAM Storage
    read_loop:
    for (int i = 0; i < SIZE; i++) {
    #pragma HLS PIPELINE II=1
        bram[i] = in_stream.read().data;
    }


    ap_uint<32> lfsr_states[K];
#pragma HLS ARRAY_PARTITION variable=lfsr_states complete dim=1

    // Reinitializate LFSR
    init_lfsr:
    for (int i = 0; i < K; i++) {
    #pragma HLS UNROLL
    	lfsr_states[i] = SEED ^ (i * 0x9E3779B9);
    }


    write_loop:
    for (int i = 0; i < SIZE_OUT; i++) {
    #pragma HLS PIPELINE II=1

        ap_uint<64 * K> packed_output = 0;

        process_k:
        for (int j = 0; j < K; j++) {
        #pragma HLS UNROLL

            int idx = i * K + j;
            ap_uint<64> data = bram[idx];

            // Extract probability and input bit
            ap_uint<32> prob = data.range(63, 32);
            bool bit_entrada = data[0];

            // Update LFSR
            ap_uint<32> &n = lfsr_states[j];
            n = (n >> 1) ^ ((n & 1u) ? TAPS : 0u);

            // Comparison
            bool resultado = (n < prob);

            // XOR
            bool bit_salida = bit_entrada ^ resultado;


            ap_uint<64> result = 0;
            result.range(63, 32) = n;
            result.range(31, 1) = 0;
            result[0] = bit_salida;

            // Output packed
            packed_output.range((j + 1) * 64 - 1, j * 64) = result;
        }


        axis_out_t out_pkt;
        out_pkt.data = packed_output;
        out_pkt.keep = -1;  // todos bytes válidos
        out_pkt.strb = -1;
        out_pkt.last = (i == SIZE_OUT - 1) ? 1 : 0;

        out_stream.write(out_pkt);
    }
}
